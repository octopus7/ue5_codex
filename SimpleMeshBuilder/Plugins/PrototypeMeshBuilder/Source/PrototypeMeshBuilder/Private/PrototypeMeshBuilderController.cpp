#include "PrototypeMeshBuilderController.h"

#include "AssetToolsModule.h"
#include "Components/DynamicMeshComponent.h"
#include "ContentBrowserModule.h"
#include "Dom/JsonObject.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DynamicMeshActor.h"
#include "Editor.h"
#include "Engine/Engine.h"
#include "Engine/Selection.h"
#include "FileHelpers.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Misc/PackageName.h"
#include "PrototypeCodexMeshBridge.h"
#include "PrototypeMeshBuilderBridge.h"
#include "PrototypeMeshGeneration.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Containers/Ticker.h"

namespace
{
	constexpr double PreviewOffsetCm = 200.0;
}

FPrototypeMeshBuilderController::FPrototypeMeshBuilderController()
	: Bridge(MakeUnique<FPrototypeCodexMeshBridge>())
	, ContentPath(TEXT("/Game/Generated/PrototypeMeshes"))
	, ReasoningEffort(TEXT("medium"))
	, StatusMessage(TEXT("Enter a prompt to queue generation jobs. Select a preview actor in the level before saving."))
{
	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FPrototypeMeshBuilderController::Tick));
	RebuildJobDisplayItems();
}

FPrototypeMeshBuilderController::~FPrototypeMeshBuilderController()
{
	if (TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	}

	if (Bridge)
	{
		Bridge->CancelAllGenerateDsl();
	}

	CleanupPreview();
}

void FPrototypeMeshBuilderController::SetPrompt(const FString& InPrompt)
{
	Prompt = InPrompt;
}

void FPrototypeMeshBuilderController::SetAssetName(const FString& InAssetName)
{
	AssetName = InAssetName;
}

void FPrototypeMeshBuilderController::SetContentPath(const FString& InContentPath)
{
	ContentPath = PrototypeMeshBuilder::NormalizeContentPath(InContentPath);
}

void FPrototypeMeshBuilderController::SetReasoningEffort(const FString& InReasoningEffort)
{
	if (InReasoningEffort.Equals(TEXT("high")) || InReasoningEffort.Equals(TEXT("xhigh")))
	{
		ReasoningEffort = InReasoningEffort;
		return;
	}

	ReasoningEffort = TEXT("medium");
}

const FString& FPrototypeMeshBuilderController::GetPrompt() const
{
	return Prompt;
}

const FString& FPrototypeMeshBuilderController::GetAssetName() const
{
	return AssetName;
}

const FString& FPrototypeMeshBuilderController::GetContentPath() const
{
	return ContentPath;
}

const FString& FPrototypeMeshBuilderController::GetReasoningEffort() const
{
	return ReasoningEffort;
}

FText FPrototypeMeshBuilderController::GetStatusText() const
{
	return FText::FromString(StatusMessage);
}

FText FPrototypeMeshBuilderController::GetSelectedActorText() const
{
	FString Error;
	AActor* SelectedActor = GetSingleSelectedActor(Error);
	if (!SelectedActor)
	{
		return FText::FromString(Error);
	}

	const bool bSavable = GetSavableDynamicMeshComponent(SelectedActor) != nullptr;
	if (const FPreviewRecord* PreviewRecord = FindPreviewRecord(SelectedActor))
	{
		return FText::FromString(FString::Printf(
			TEXT("Selected Actor: %s (%s, %d triangle(s))"),
			*SelectedActor->GetActorLabel(),
			bSavable ? TEXT("saveable preview") : TEXT("preview only"),
			PreviewRecord->TriangleCount));
	}

	return FText::FromString(FString::Printf(
		TEXT("Selected Actor: %s (%s)"),
		*SelectedActor->GetActorLabel(),
		bSavable ? TEXT("saveable dynamic mesh") : TEXT("not saveable")));
}

TArray<TSharedPtr<FString>>& FPrototypeMeshBuilderController::GetJobDisplayItems()
{
	return JobDisplayItems;
}

const TArray<TSharedPtr<FString>>& FPrototypeMeshBuilderController::GetJobDisplayItems() const
{
	return JobDisplayItems;
}

bool FPrototypeMeshBuilderController::CanSave() const
{
	FString Error;
	AActor* SelectedActor = GetSingleSelectedActor(Error);
	return SelectedActor && GetSavableDynamicMeshComponent(SelectedActor) != nullptr;
}

void FPrototypeMeshBuilderController::Generate()
{
	if (Prompt.TrimStartAndEnd().IsEmpty())
	{
		SetStatus(TEXT("Prompt is required."));
		return;
	}

	FPrototypeMeshRequest Request;
	Request.Prompt = Prompt;
	Request.AssetName = PrototypeMeshBuilder::SanitizeAssetName(AssetName);
	Request.ContentPath = PrototypeMeshBuilder::NormalizeContentPath(ContentPath);
	Request.Locale = TEXT("ko-KR");
	Request.ReasoningEffort = ReasoningEffort;
	Request.MaxPrimitiveCount = 32;

	FQueuedGenerateRequest& QueuedJob = PendingJobs.AddDefaulted_GetRef();
	QueuedJob.Request = MoveTemp(Request);
	QueuedJob.EnqueuedAtSeconds = FPlatformTime::Seconds();

	TryStartNextQueuedJob();
	RebuildJobDisplayItems();

	const int32 QueuedCount = PendingJobs.Num() + (ActiveJob.IsSet() ? 1 : 0);
	if (QueuedCount > 0)
	{
		SetStatus(FString::Printf(TEXT("Queued generation job. Active + queued jobs: %d."), QueuedCount));
	}
}

void FPrototypeMeshBuilderController::Save()
{
	FString SelectionError;
	AActor* SelectedActor = GetSingleSelectedActor(SelectionError);
	if (!SelectedActor)
	{
		SetStatus(SelectionError);
		return;
	}

	UDynamicMeshComponent* DynamicMeshComponent = GetSavableDynamicMeshComponent(SelectedActor);
	if (!DynamicMeshComponent)
	{
		SetStatus(TEXT("Selected actor does not contain a savable DynamicMesh component."));
		return;
	}

	const FString NormalizedContentPath = PrototypeMeshBuilder::NormalizeContentPath(ContentPath);
	if (!FPackageName::IsValidLongPackageName(NormalizedContentPath))
	{
		SetStatus(TEXT("Content path must be a valid /Game package path."));
		return;
	}

	const FPreviewRecord* PreviewRecord = FindPreviewRecord(SelectedActor);
	const FString SuggestedAssetName = PreviewRecord
		? (PreviewRecord->RequestedAssetName.IsEmpty() ? PreviewRecord->Dsl.MeshName : PreviewRecord->RequestedAssetName)
		: SelectedActor->GetActorLabel();
	const FString SanitizedAssetName = PrototypeMeshBuilder::SanitizeAssetName(AssetName.IsEmpty() ? SuggestedAssetName : AssetName);

	const FString BaseMeshPackagePath = NormalizedContentPath / SanitizedAssetName;
	FString FinalMeshPackagePath = BaseMeshPackagePath;
	FString FinalMeshAssetName = SanitizedAssetName;
	if (FPackageName::DoesPackageExist(BaseMeshPackagePath))
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
		AssetToolsModule.Get().CreateUniqueAssetName(BaseMeshPackagePath, TEXT("_001"), FinalMeshPackagePath, FinalMeshAssetName);
	}

	const UE::Geometry::FDynamicMesh3* SelectedMesh = DynamicMeshComponent->GetMesh();
	if (!SelectedMesh || SelectedMesh->TriangleCount() == 0)
	{
		SetStatus(TEXT("Selected actor has no mesh triangles to save."));
		return;
	}

	UE::Geometry::FDynamicMesh3 MeshCopy(*SelectedMesh);

	UStaticMesh* SavedMesh = nullptr;
	FString SaveError;
	if (!PrototypeMeshBuilder::BuildStaticMeshAsset(FinalMeshPackagePath, FinalMeshAssetName, MeshCopy, SavedMesh, SaveError))
	{
		SetStatus(FString::Printf(TEXT("Save failed: %s"), *SaveError));
		return;
	}

	const FString BaseMaterialPackagePath = NormalizedContentPath / FString::Printf(TEXT("%s_MAT"), *FinalMeshAssetName);
	FString FinalMaterialPackagePath = BaseMaterialPackagePath;
	FString FinalMaterialAssetName = FString::Printf(TEXT("%s_MAT"), *FinalMeshAssetName);
	if (FPackageName::DoesPackageExist(BaseMaterialPackagePath))
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
		AssetToolsModule.Get().CreateUniqueAssetName(BaseMaterialPackagePath, TEXT("_001"), FinalMaterialPackagePath, FinalMaterialAssetName);
	}

	UMaterialInstanceConstant* SavedMaterial = nullptr;
	if (!PrototypeMeshBuilder::CreateVertexColorMaterialAsset(FinalMaterialPackagePath, FinalMaterialAssetName, SavedMaterial, SaveError))
	{
		SetStatus(FString::Printf(TEXT("Material save failed: %s"), *SaveError));
		return;
	}

	if (!PrototypeMeshBuilder::ApplyStaticMeshMaterial(SavedMesh, SavedMaterial, SaveError))
	{
		SetStatus(FString::Printf(TEXT("Material assignment failed: %s"), *SaveError));
		return;
	}

	FString MetadataJson;
	if (!BuildSelectedActorMetadataJson(*SelectedActor, PreviewRecord, FinalMeshPackagePath, FinalMaterialPackagePath, MetadataJson))
	{
		SetStatus(TEXT("Failed to build mesh metadata."));
		return;
	}

	if (!PrototypeMeshBuilder::WriteAssetMetadata(SavedMesh, MetadataJson, SaveError)
		|| !PrototypeMeshBuilder::WriteAssetMetadata(SavedMaterial, MetadataJson, SaveError))
	{
		SetStatus(FString::Printf(TEXT("Metadata save failed: %s"), *SaveError));
		return;
	}

	TArray<UPackage*> PackagesToSave;
	PackagesToSave.AddUnique(SavedMesh->GetOutermost());
	PackagesToSave.AddUnique(SavedMaterial->GetOutermost());
	if (!UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true))
	{
		SetStatus(TEXT("Assets were created but package save failed."));
		return;
	}

	TArray<UObject*> AssetsToSync;
	AssetsToSync.Add(SavedMesh);
	AssetsToSync.Add(SavedMaterial);

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	ContentBrowserModule.Get().SyncBrowserToAssets(AssetsToSync);

	SetStatus(FString::Printf(TEXT("Saved StaticMesh to %s and material to %s"), *FinalMeshPackagePath, *FinalMaterialPackagePath));
}

void FPrototypeMeshBuilderController::Clear()
{
	if (Bridge)
	{
		Bridge->CancelAllGenerateDsl();
	}

	ActiveJob.Reset();
	PendingJobs.Reset();
	DestroyAllPreviewActors();
	PreviewRecords.Reset();
	RebuildJobDisplayItems();
	SetStatus(TEXT("Cancelled queued jobs and cleared preview actors."));
}

void FPrototypeMeshBuilderController::CleanupPreview()
{
	if (Bridge)
	{
		Bridge->CancelAllGenerateDsl();
	}

	ActiveJob.Reset();
	PendingJobs.Reset();
	DestroyAllPreviewActors();
	PreviewRecords.Reset();
	JobDisplayItems.Reset();
}

bool FPrototypeMeshBuilderController::Tick(float DeltaTime)
{
	TryStartNextQueuedJob();
	PollActiveJob();
	RebuildJobDisplayItems();
	return true;
}

void FPrototypeMeshBuilderController::TryStartNextQueuedJob()
{
	if (ActiveJob.IsSet() || PendingJobs.IsEmpty() || !Bridge)
	{
		return;
	}

	const FQueuedGenerateRequest QueuedJob = PendingJobs[0];
	PendingJobs.RemoveAt(0);

	FPrototypeBridgeJobHandle Handle;
	FString StartError;
	if (!Bridge->StartGenerateDslAsync(QueuedJob.Request, Handle, StartError))
	{
		SetStatus(FString::Printf(TEXT("Failed to start Codex job: %s"), *StartError));
		return;
	}

	ActiveJob.Emplace();
	FActiveGenerateJob& RunningJob = ActiveJob.GetValue();
	RunningJob.Handle = Handle;
	RunningJob.Request = QueuedJob.Request;
	RunningJob.EnqueuedAtSeconds = QueuedJob.EnqueuedAtSeconds;
	RunningJob.StartedAtSeconds = FPlatformTime::Seconds();
}

void FPrototypeMeshBuilderController::PollActiveJob()
{
	if (!ActiveJob.IsSet() || !Bridge)
	{
		return;
	}

	bool bCompleted = false;
	FPrototypeBridgeResult BridgeResult;
	FString PollError;
	if (!Bridge->PollGenerateDsl(ActiveJob->Handle, bCompleted, BridgeResult, PollError))
	{
		SetStatus(FString::Printf(TEXT("Bridge polling failed: %s"), *PollError));
		ActiveJob.Reset();
		return;
	}

	if (!bCompleted)
	{
		return;
	}

	const FActiveGenerateJob CompletedJob = ActiveJob.GetValue();
	ActiveJob.Reset();
	FinalizeCompletedJob(CompletedJob, BridgeResult);
}

void FPrototypeMeshBuilderController::FinalizeCompletedJob(const FActiveGenerateJob& CompletedJob, const FPrototypeBridgeResult& BridgeResult)
{
	if (!BridgeResult.bSuccess)
	{
		const FString FailureReason = BridgeResult.ErrorMessage.IsEmpty() ? TEXT("unknown bridge failure") : BridgeResult.ErrorMessage;
		SetStatus(FString::Printf(TEXT("Generate failed: %s"), *FailureReason));
		return;
	}

	FPrototypeShapeDsl ParsedDsl;
	FString ParseError;
	if (!PrototypeMeshBuilder::ParseShapeDslJson(BridgeResult.RawDslJson, ParsedDsl, ParseError))
	{
		SetStatus(FString::Printf(TEXT("DSL parse failed: %s"), *ParseError));
		return;
	}

	FGeneratedMeshBuffers GeneratedBuffers;
	FString BuildError;
	if (!PrototypeMeshBuilder::BuildMeshBuffers(ParsedDsl, GeneratedBuffers, BuildError))
	{
		SetStatus(FString::Printf(TEXT("Mesh build failed: %s"), *BuildError));
		return;
	}

	UE::Geometry::FDynamicMesh3 GeneratedDynamicMesh(UE::Geometry::EMeshComponents::None);
	if (!PrototypeMeshBuilder::BuildDynamicMesh(GeneratedBuffers, GeneratedDynamicMesh, BuildError))
	{
		SetStatus(FString::Printf(TEXT("Preview mesh failed: %s"), *BuildError));
		return;
	}

	FString PreviewError;
	if (!SpawnPreviewActor(CompletedJob, ParsedDsl, GeneratedBuffers, GeneratedDynamicMesh, BridgeResult, PreviewError))
	{
		SetStatus(FString::Printf(TEXT("Preview spawn failed: %s"), *PreviewError));
		return;
	}

	const int32 RemainingJobs = PendingJobs.Num() + (ActiveJob.IsSet() ? 1 : 0);
	SetStatus(FString::Printf(
		TEXT("Generated %s with %d primitive(s), %d triangle(s). %d job(s) remain queued/running."),
		*ParsedDsl.MeshName,
		ParsedDsl.Primitives.Num(),
		GeneratedBuffers.GetTriangleCount(),
		RemainingJobs));
}

void FPrototypeMeshBuilderController::RebuildJobDisplayItems()
{
	JobDisplayItems.Reset();

	const double Now = FPlatformTime::Seconds();
	if (ActiveJob.IsSet())
	{
		JobDisplayItems.Add(MakeShared<FString>(BuildJobSummary(TEXT("Running"), ActiveJob->Request, Now - ActiveJob->StartedAtSeconds)));
	}

	for (const FQueuedGenerateRequest& QueuedJob : PendingJobs)
	{
		JobDisplayItems.Add(MakeShared<FString>(BuildJobSummary(TEXT("Queued"), QueuedJob.Request, Now - QueuedJob.EnqueuedAtSeconds)));
	}
}

void FPrototypeMeshBuilderController::SetStatus(const FString& InStatus)
{
	StatusMessage = InStatus;
}

FString FPrototypeMeshBuilderController::FormatElapsed(double ElapsedSeconds) const
{
	const int32 TotalSeconds = FMath::Max(0, FMath::FloorToInt(ElapsedSeconds));
	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;
	return FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
}

FString FPrototypeMeshBuilderController::BuildJobSummary(const FString& Prefix, const FPrototypeMeshRequest& Request, double ElapsedSeconds) const
{
	const FString Title = Request.AssetName.IsEmpty() ? Request.Prompt.Left(32) : Request.AssetName;
	return FString::Printf(TEXT("%s  %s  [%s]  %s"), *Prefix, *FormatElapsed(ElapsedSeconds), *Request.ReasoningEffort, *Title);
}

FVector FPrototypeMeshBuilderController::GetNextPreviewLocation() const
{
	return FVector(0.0, PreviewRecords.Num() * PreviewOffsetCm, 0.0);
}

void FPrototypeMeshBuilderController::DestroyAllPreviewActors()
{
	for (const FPreviewRecord& PreviewRecord : PreviewRecords)
	{
		if (PreviewRecord.PreviewActor.IsValid())
		{
			if (UWorld* World = PreviewRecord.PreviewActor->GetWorld())
			{
				World->DestroyActor(PreviewRecord.PreviewActor.Get());
			}
		}
	}
}

AActor* FPrototypeMeshBuilderController::GetSingleSelectedActor(FString& OutError) const
{
	OutError = TEXT("Select one preview actor in the level to save.");

	if (!GEditor || !GEditor->GetSelectedActors())
	{
		OutError = TEXT("Editor selection is not available.");
		return nullptr;
	}

	USelection* SelectedActors = GEditor->GetSelectedActors();
	if (SelectedActors->Num() == 0)
	{
		OutError = TEXT("Select one preview actor in the level to save.");
		return nullptr;
	}

	if (SelectedActors->Num() > 1)
	{
		OutError = TEXT("Select exactly one actor before saving.");
		return nullptr;
	}

	for (FSelectionIterator It(*SelectedActors); It; ++It)
	{
		if (AActor* Actor = Cast<AActor>(*It))
		{
			OutError.Empty();
			return Actor;
		}
	}

	OutError = TEXT("Selected object is not an actor.");
	return nullptr;
}

UDynamicMeshComponent* FPrototypeMeshBuilderController::GetSavableDynamicMeshComponent(AActor* Actor) const
{
	return Actor ? Actor->FindComponentByClass<UDynamicMeshComponent>() : nullptr;
}

FPrototypeMeshBuilderController::FPreviewRecord* FPrototypeMeshBuilderController::FindPreviewRecord(const AActor* Actor)
{
	for (FPreviewRecord& PreviewRecord : PreviewRecords)
	{
		if (PreviewRecord.PreviewActor.Get() == Actor)
		{
			return &PreviewRecord;
		}
	}

	return nullptr;
}

const FPrototypeMeshBuilderController::FPreviewRecord* FPrototypeMeshBuilderController::FindPreviewRecord(const AActor* Actor) const
{
	for (const FPreviewRecord& PreviewRecord : PreviewRecords)
	{
		if (PreviewRecord.PreviewActor.Get() == Actor)
		{
			return &PreviewRecord;
		}
	}

	return nullptr;
}

bool FPrototypeMeshBuilderController::SpawnPreviewActor(const FActiveGenerateJob& CompletedJob, const FPrototypeShapeDsl& Dsl, const FGeneratedMeshBuffers& Buffers, const UE::Geometry::FDynamicMesh3& DynamicMesh, const FPrototypeBridgeResult& BridgeResult, FString& OutError)
{
	UWorld* EditorWorld = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!EditorWorld)
	{
		OutError = TEXT("Editor world is not available.");
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Name = MakeUniqueObjectName(EditorWorld, ADynamicMeshActor::StaticClass(), TEXT("PrototypeMeshPreview"));
	SpawnParameters.ObjectFlags = RF_Transient | RF_DuplicateTransient | RF_TextExportTransient;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.OverrideLevel = EditorWorld->GetCurrentLevel();

	ADynamicMeshActor* SpawnedActor = EditorWorld->SpawnActor<ADynamicMeshActor>(SpawnParameters);
	if (!SpawnedActor)
	{
		OutError = TEXT("Failed to spawn preview actor.");
		return false;
	}

	const FString PreviewLabel = FString::Printf(TEXT("PrototypeMeshPreview_%s"), *PrototypeMeshBuilder::SanitizeAssetName(Dsl.MeshName));
	SpawnedActor->SetActorLabel(PreviewLabel);
	SpawnedActor->SetActorEnableCollision(false);
	SpawnedActor->SetActorLocation(GetNextPreviewLocation());

	UDynamicMeshComponent* DynamicMeshComponent = SpawnedActor->GetDynamicMeshComponent();
	if (!DynamicMeshComponent)
	{
		EditorWorld->DestroyActor(SpawnedActor);
		OutError = TEXT("Preview actor does not contain a DynamicMesh component.");
		return false;
	}

	DynamicMeshComponent->SetMesh(UE::Geometry::FDynamicMesh3(DynamicMesh));
	if (GEngine)
	{
		UMaterialInterface* PreviewMaterial = GEngine->VertexColorViewModeMaterial_ColorOnly ? static_cast<UMaterialInterface*>(GEngine->VertexColorViewModeMaterial_ColorOnly) : static_cast<UMaterialInterface*>(GEngine->VertexColorMaterial);
		if (PreviewMaterial)
		{
			DynamicMeshComponent->SetMaterial(0, PreviewMaterial);
		}
	}

	FPreviewRecord& PreviewRecord = PreviewRecords.AddDefaulted_GetRef();
	PreviewRecord.PreviewActor = SpawnedActor;
	PreviewRecord.JobId = CompletedJob.Handle.Id;
	PreviewRecord.Dsl = Dsl;
	PreviewRecord.Prompt = CompletedJob.Request.Prompt;
	PreviewRecord.ReasoningEffort = CompletedJob.Request.ReasoningEffort;
	PreviewRecord.RequestedAssetName = CompletedJob.Request.AssetName;
	PreviewRecord.RequestedContentPath = CompletedJob.Request.ContentPath;
	PreviewRecord.Diagnostics = BridgeResult.Diagnostics;
	PreviewRecord.EnqueuedAtSeconds = CompletedJob.EnqueuedAtSeconds;
	PreviewRecord.StartedAtSeconds = CompletedJob.StartedAtSeconds;
	PreviewRecord.CompletedAtSeconds = FPlatformTime::Seconds();
	PreviewRecord.TriangleCount = Buffers.GetTriangleCount();
	PreviewRecord.PrimitiveCount = Dsl.Primitives.Num();
	PreviewRecord.PreviewOffsetIndex = PreviewRecords.Num() - 1;

	return true;
}

bool FPrototypeMeshBuilderController::BuildSelectedActorMetadataJson(const AActor& SelectedActor, const FPreviewRecord* PreviewRecord, const FString& SavedMeshPath, const FString& SavedMaterialPath, FString& OutJson) const
{
	TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
	RootObject->SetStringField(TEXT("version"), TEXT("1.0"));
	RootObject->SetStringField(TEXT("tool"), TEXT("PrototypeMeshBuilder"));
	RootObject->SetStringField(TEXT("saved_mesh_path"), SavedMeshPath);
	RootObject->SetStringField(TEXT("saved_material_path"), SavedMaterialPath);
	RootObject->SetStringField(TEXT("saved_utc"), FDateTime::UtcNow().ToIso8601());
	RootObject->SetStringField(TEXT("source_actor_label"), SelectedActor.GetActorLabel());
	RootObject->SetStringField(TEXT("source_actor_path"), SelectedActor.GetPathName());
	RootObject->SetStringField(TEXT("source_actor_class"), SelectedActor.GetClass()->GetPathName());

	if (PreviewRecord)
	{
		RootObject->SetStringField(TEXT("job_id"), PreviewRecord->JobId.ToString(EGuidFormats::DigitsWithHyphensLower));
		RootObject->SetStringField(TEXT("prompt"), PreviewRecord->Prompt);
		RootObject->SetStringField(TEXT("reasoning_effort"), PreviewRecord->ReasoningEffort);
		RootObject->SetStringField(TEXT("request_asset_name"), PreviewRecord->RequestedAssetName);
		RootObject->SetStringField(TEXT("request_content_path"), PreviewRecord->RequestedContentPath);
		RootObject->SetStringField(TEXT("mesh_name"), PreviewRecord->Dsl.MeshName);
		RootObject->SetNumberField(TEXT("primitive_count"), PreviewRecord->PrimitiveCount);
		RootObject->SetNumberField(TEXT("triangle_count"), PreviewRecord->TriangleCount);
		RootObject->SetNumberField(TEXT("preview_offset_cm"), PreviewRecord->PreviewOffsetIndex * PreviewOffsetCm);
		RootObject->SetNumberField(TEXT("queue_wait_seconds"), FMath::Max(0.0, PreviewRecord->StartedAtSeconds - PreviewRecord->EnqueuedAtSeconds));
		RootObject->SetNumberField(TEXT("generation_seconds"), FMath::Max(0.0, PreviewRecord->CompletedAtSeconds - PreviewRecord->StartedAtSeconds));
		RootObject->SetStringField(TEXT("dsl_json"), PreviewRecord->Dsl.RawJson);
		RootObject->SetStringField(TEXT("bridge_diagnostics"), PreviewRecord->Diagnostics);
	}
	else
	{
		RootObject->SetStringField(TEXT("source_type"), TEXT("selected_dynamic_mesh_actor"));
	}

	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
	return FJsonSerializer::Serialize(RootObject, Writer);
}
