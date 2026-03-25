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
#include "HAL/FileManager.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "Materials/MaterialInterface.h"
#include "Misc/DateTime.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "PrototypeCodexMeshBridge.h"
#include "PrototypeMeshBuilderBridge.h"
#include "PrototypeMeshGeneration.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Containers/Ticker.h"

namespace
{
	constexpr double PreviewOffsetCm = 200.0;
	constexpr int32 MaxEmbeddedSourcePayloadChars = 65536;
	const TCHAR* SharedVertexColorMaterialAssetName = TEXT("M_PrototypeVertexColorLit");

	FString BuildModeLabel(const FPrototypeMeshRequest& Request)
	{
		if (Request.GenerationMode == EPrototypeGenerationMode::Voxel)
		{
			return FString::Printf(TEXT("voxel %d^3"), Request.VoxelResolution);
		}

		return TEXT("primitive");
	}
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

void FPrototypeMeshBuilderController::SetGenerationMode(EPrototypeGenerationMode InGenerationMode)
{
	GenerationMode = InGenerationMode;
}

void FPrototypeMeshBuilderController::SetVoxelResolution(int32 InVoxelResolution)
{
	VoxelResolution = PrototypeIsSupportedVoxelResolution(InVoxelResolution) ? InVoxelResolution : 32;
}

void FPrototypeMeshBuilderController::SetUseSharedMaterial(bool bInUseSharedMaterial)
{
	bUseSharedMaterial = bInUseSharedMaterial;
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

EPrototypeGenerationMode FPrototypeMeshBuilderController::GetGenerationMode() const
{
	return GenerationMode;
}

int32 FPrototypeMeshBuilderController::GetVoxelResolution() const
{
	return VoxelResolution;
}

bool FPrototypeMeshBuilderController::GetUseSharedMaterial() const
{
	return bUseSharedMaterial;
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
		const FString ModeLabel = PreviewRecord->GenerationMode == EPrototypeGenerationMode::Voxel
			? FString::Printf(TEXT("voxel %d x %d x %d"), PreviewRecord->VoxelResolution.X, PreviewRecord->VoxelResolution.Y, PreviewRecord->VoxelResolution.Z)
			: FString::Printf(TEXT("primitive %d part(s)"), PreviewRecord->PrimitiveCount);
		return FText::FromString(FString::Printf(
			TEXT("Selected Actor: %s (%s, %s, %d triangle(s))"),
			*SelectedActor->GetActorLabel(),
			bSavable ? TEXT("saveable preview") : TEXT("preview only"),
			*ModeLabel,
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

bool FPrototypeMeshBuilderController::CanDeleteSelectedPreview() const
{
	FString Error;
	AActor* SelectedActor = GetSingleSelectedActor(Error);
	return SelectedActor && FindPreviewRecord(SelectedActor) != nullptr;
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
	Request.GenerationMode = GenerationMode;
	Request.MaxPrimitiveCount = 32;
	Request.VoxelResolution = VoxelResolution;

	FQueuedGenerateRequest& QueuedJob = PendingJobs.AddDefaulted_GetRef();
	QueuedJob.Request = MoveTemp(Request);
	QueuedJob.EnqueuedAtSeconds = FPlatformTime::Seconds();
	const FString QueuedModeLabel = BuildModeLabel(QueuedJob.Request);

	TryStartNextQueuedJob();
	RebuildJobDisplayItems();

	const int32 QueuedCount = PendingJobs.Num() + (ActiveJob.IsSet() ? 1 : 0);
	if (QueuedCount > 0)
	{
		SetStatus(FString::Printf(TEXT("Queued %s generation job. Active + queued jobs: %d."), *QueuedModeLabel, QueuedCount));
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
		? (PreviewRecord->RequestedAssetName.IsEmpty() ? PreviewRecord->MeshName : PreviewRecord->RequestedAssetName)
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

	FString FinalMaterialPackagePath;
	FString FinalMaterialAssetName;
	UMaterialInterface* SavedMaterial = nullptr;
	bool bCreatedMaterialAsset = false;
	const bool bUsingSharedMaterial = bUseSharedMaterial;

	if (bUsingSharedMaterial)
	{
		FinalMaterialAssetName = SharedVertexColorMaterialAssetName;
		FinalMaterialPackagePath = NormalizedContentPath / FinalMaterialAssetName;

		if (FPackageName::DoesPackageExist(FinalMaterialPackagePath))
		{
			const FString MaterialObjectPath = FinalMaterialPackagePath + TEXT(".") + FinalMaterialAssetName;
			SavedMaterial = LoadObject<UMaterialInterface>(nullptr, *MaterialObjectPath);
			if (!SavedMaterial)
			{
				SetStatus(FString::Printf(TEXT("Shared material exists but could not be loaded: %s"), *MaterialObjectPath));
				return;
			}
		}
		else if (!PrototypeMeshBuilder::CreateVertexColorLitMaterialAsset(FinalMaterialPackagePath, FinalMaterialAssetName, SavedMaterial, SaveError))
		{
			SetStatus(FString::Printf(TEXT("Shared material creation failed: %s"), *SaveError));
			return;
		}
		else
		{
			bCreatedMaterialAsset = true;
		}
	}
	else
	{
		const FString BaseMaterialAssetName = FString::Printf(TEXT("M_%s_VertexColorLit"), *FinalMeshAssetName);
		const FString BaseMaterialPackagePath = NormalizedContentPath / BaseMaterialAssetName;
		FinalMaterialPackagePath = BaseMaterialPackagePath;
		FinalMaterialAssetName = BaseMaterialAssetName;
		if (FPackageName::DoesPackageExist(BaseMaterialPackagePath))
		{
			FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
			AssetToolsModule.Get().CreateUniqueAssetName(BaseMaterialPackagePath, TEXT("_001"), FinalMaterialPackagePath, FinalMaterialAssetName);
		}

		if (!PrototypeMeshBuilder::CreateVertexColorLitMaterialAsset(FinalMaterialPackagePath, FinalMaterialAssetName, SavedMaterial, SaveError))
		{
			SetStatus(FString::Printf(TEXT("Material save failed: %s"), *SaveError));
			return;
		}

		bCreatedMaterialAsset = true;
	}

	if (!PrototypeMeshBuilder::ApplyStaticMeshMaterial(SavedMesh, SavedMaterial, SaveError))
	{
		SetStatus(FString::Printf(TEXT("Material assignment failed: %s"), *SaveError));
		return;
	}

	FString MeshMetadataJson;
	if (!BuildSelectedActorMetadataJson(*SelectedActor, PreviewRecord, FinalMeshPackagePath, FinalMaterialPackagePath, bUsingSharedMaterial, MeshMetadataJson))
	{
		SetStatus(TEXT("Failed to build mesh metadata."));
		return;
	}

	FString MaterialMetadataJson;
	if (!BuildMaterialMetadataJson(FinalMaterialPackagePath, FinalMeshPackagePath, bUsingSharedMaterial, MaterialMetadataJson))
	{
		SetStatus(TEXT("Failed to build material metadata."));
		return;
	}

	if (!PrototypeMeshBuilder::WriteAssetMetadata(SavedMesh, MeshMetadataJson, SaveError)
		|| !PrototypeMeshBuilder::WriteAssetMetadata(SavedMaterial, MaterialMetadataJson, SaveError))
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

	const FString MaterialModeText = bUsingSharedMaterial
		? (bCreatedMaterialAsset ? TEXT("created shared material") : TEXT("reused shared material"))
		: TEXT("created dedicated material");
	SetStatus(FString::Printf(TEXT("Saved StaticMesh to %s and %s at %s"), *FinalMeshPackagePath, *MaterialModeText, *FinalMaterialPackagePath));
}

void FPrototypeMeshBuilderController::DeleteSelectedPreview()
{
	FString SelectionError;
	AActor* SelectedActor = GetSingleSelectedActor(SelectionError);
	if (!SelectedActor)
	{
		SetStatus(SelectionError);
		return;
	}

	int32 PreviewIndex = INDEX_NONE;
	for (int32 Index = 0; Index < PreviewRecords.Num(); ++Index)
	{
		if (PreviewRecords[Index].PreviewActor.Get() == SelectedActor)
		{
			PreviewIndex = Index;
			break;
		}
	}

	if (PreviewIndex == INDEX_NONE)
	{
		SetStatus(TEXT("Selected actor is not a preview generated by this tool."));
		return;
	}

	const FString DeletedActorLabel = SelectedActor->GetActorLabel();
	if (UWorld* World = SelectedActor->GetWorld())
	{
		World->DestroyActor(SelectedActor);
	}

	PreviewRecords.RemoveAt(PreviewIndex);
	RefreshPreviewActorOffsets();
	SetStatus(FString::Printf(TEXT("Deleted preview actor: %s"), *DeletedActorLabel));
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
	FString DebugDirectory;
	FString DebugExportError;

	if (!BridgeResult.bSuccess)
	{
		const bool bDebugExported = ExportDebugArtifacts(CompletedJob.Request, BridgeResult, nullptr, nullptr, DebugDirectory, DebugExportError);
		const FString FailureReason = BridgeResult.ErrorMessage.IsEmpty() ? TEXT("unknown bridge failure") : BridgeResult.ErrorMessage;
		SetStatus(bDebugExported
			? FString::Printf(TEXT("Generate failed: %s Debug files: %s"), *FailureReason, *DebugDirectory)
			: FString::Printf(TEXT("Generate failed: %s Debug export failed: %s"), *FailureReason, *DebugExportError));
		return;
	}

	FPrototypeMeshPayload ParsedPayload;
	FString ParseError;
	if (!PrototypeMeshBuilder::ParseMeshPayloadJson(BridgeResult.RawPayloadJson, ParsedPayload, ParseError))
	{
		SetStatus(FString::Printf(TEXT("Payload parse failed: %s"), *ParseError));
		return;
	}

	if (ParsedPayload.GenerationMode != CompletedJob.Request.GenerationMode)
	{
		SetStatus(FString::Printf(
			TEXT("Payload mode mismatch: requested %s but bridge returned %s."),
			*PrototypeGenerationModeToString(CompletedJob.Request.GenerationMode),
			*PrototypeGenerationModeToString(ParsedPayload.GenerationMode)));
		return;
	}

	FGeneratedPreviewData PreviewData;
	PreviewData.GenerationMode = ParsedPayload.GenerationMode;
	PreviewData.MeshName = ParsedPayload.MeshName;
	PreviewData.SourcePayloadJson = BridgeResult.RawPayloadJson;

	FGeneratedMeshBuffers GeneratedBuffers;
	FString BuildError;
	if (ParsedPayload.GenerationMode == EPrototypeGenerationMode::Voxel)
	{
		PreviewData.VoxelResolution = ParsedPayload.VoxelGrid.Resolution;
		if (!PrototypeMeshBuilder::CountOccupiedVoxels(ParsedPayload.VoxelGrid, PreviewData.OccupiedVoxelCount, BuildError))
		{
			SetStatus(FString::Printf(TEXT("Voxel grid validation failed: %s"), *BuildError));
			return;
		}
	}
	else
	{
		PreviewData.PrimitiveCount = ParsedPayload.PrimitiveShape.Primitives.Num();
	}

	if (!PrototypeMeshBuilder::BuildMeshBuffers(ParsedPayload, GeneratedBuffers, BuildError))
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
	if (!SpawnPreviewActor(CompletedJob, PreviewData, GeneratedBuffers, GeneratedDynamicMesh, BridgeResult, PreviewError))
	{
		SetStatus(FString::Printf(TEXT("Preview spawn failed: %s"), *PreviewError));
		return;
	}

	const bool bDebugExported = ExportDebugArtifacts(CompletedJob.Request, BridgeResult, &PreviewData, &GeneratedBuffers, DebugDirectory, DebugExportError);

	const int32 RemainingJobs = PendingJobs.Num() + (ActiveJob.IsSet() ? 1 : 0);
	if (PreviewData.GenerationMode == EPrototypeGenerationMode::Voxel)
	{
		const FString Status = FString::Printf(
			TEXT("Generated %s with %d^3 voxels (%d filled), %d triangle(s). %d job(s) remain queued/running."),
			*PreviewData.MeshName,
			PreviewData.VoxelResolution.X,
			PreviewData.OccupiedVoxelCount,
			GeneratedBuffers.GetTriangleCount(),
			RemainingJobs);
		SetStatus(bDebugExported
			? FString::Printf(TEXT("%s Debug files: %s"), *Status, *DebugDirectory)
			: FString::Printf(TEXT("%s Debug export failed: %s"), *Status, *DebugExportError));
		return;
	}

	const FString Status = FString::Printf(
		TEXT("Generated %s with %d primitive(s), %d triangle(s). %d job(s) remain queued/running."),
		*PreviewData.MeshName,
		PreviewData.PrimitiveCount,
		GeneratedBuffers.GetTriangleCount(),
		RemainingJobs);
	SetStatus(bDebugExported
		? FString::Printf(TEXT("%s Debug files: %s"), *Status, *DebugDirectory)
		: FString::Printf(TEXT("%s Debug export failed: %s"), *Status, *DebugExportError));
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
	return FString::Printf(TEXT("%s  %s  [%s|%s]  %s"), *Prefix, *FormatElapsed(ElapsedSeconds), *Request.ReasoningEffort, *BuildModeLabel(Request), *Title);
}

bool FPrototypeMeshBuilderController::ExportDebugArtifacts(const FPrototypeMeshRequest& Request, const FPrototypeBridgeResult& BridgeResult, const FGeneratedPreviewData* PreviewData, const FGeneratedMeshBuffers* Buffers, FString& OutDirectory, FString& OutError) const
{
	OutDirectory.Empty();
	OutError.Empty();

	const FString SafeAssetName = PrototypeMeshBuilder::SanitizeAssetName(Request.AssetName.IsEmpty() ? TEXT("PrototypeMesh") : Request.AssetName);
	const FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
	const FString ShortJobId = FGuid::NewGuid().ToString(EGuidFormats::Digits).Left(8);
	OutDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("PrototypeMeshBuilder"), TEXT("Debug"), Timestamp + TEXT("_") + SafeAssetName + TEXT("_") + ShortJobId);

	if (!IFileManager::Get().DirectoryExists(*OutDirectory) && !IFileManager::Get().MakeDirectory(*OutDirectory, true))
	{
		OutError = FString::Printf(TEXT("Failed to create debug directory: %s"), *OutDirectory);
		return false;
	}

	TSharedRef<FJsonObject> RequestJson = MakeShared<FJsonObject>();
	RequestJson->SetStringField(TEXT("prompt"), Request.Prompt);
	RequestJson->SetStringField(TEXT("asset_name"), Request.AssetName);
	RequestJson->SetStringField(TEXT("content_path"), Request.ContentPath);
	RequestJson->SetStringField(TEXT("locale"), Request.Locale);
	RequestJson->SetStringField(TEXT("reasoning_effort"), Request.ReasoningEffort);
	RequestJson->SetStringField(TEXT("generation_mode"), PrototypeGenerationModeToString(Request.GenerationMode));
	RequestJson->SetNumberField(TEXT("max_primitive_count"), Request.MaxPrimitiveCount);
	RequestJson->SetNumberField(TEXT("voxel_resolution"), Request.VoxelResolution);

	FString RequestPayload;
	{
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestPayload);
		FJsonSerializer::Serialize(RequestJson, Writer);
	}

	if (!PrototypeMeshBuilder::WriteTextFileUtf8(FPaths::Combine(OutDirectory, TEXT("request.json")), RequestPayload, OutError))
	{
		return false;
	}

	if (!BridgeResult.RawLastMessage.IsEmpty()
		&& !PrototypeMeshBuilder::WriteTextFileUtf8(FPaths::Combine(OutDirectory, TEXT("codex_last_message.txt")), BridgeResult.RawLastMessage, OutError))
	{
		return false;
	}

	if (!BridgeResult.Diagnostics.IsEmpty()
		&& !PrototypeMeshBuilder::WriteTextFileUtf8(FPaths::Combine(OutDirectory, TEXT("codex_cli_output.txt")), BridgeResult.Diagnostics, OutError))
	{
		return false;
	}

	if (!BridgeResult.RawPayloadJson.IsEmpty()
		&& !PrototypeMeshBuilder::WriteTextFileUtf8(FPaths::Combine(OutDirectory, TEXT("mesh_payload.json")), BridgeResult.RawPayloadJson, OutError))
	{
		return false;
	}

	if (Buffers && Buffers->IsValid())
	{
		if (!PrototypeMeshBuilder::WriteMeshBuffersObjFile(FPaths::Combine(OutDirectory, TEXT("mesh.obj")), *Buffers, OutError))
		{
			return false;
		}
	}

	if (PreviewData)
	{
		TSharedRef<FJsonObject> SummaryJson = MakeShared<FJsonObject>();
		SummaryJson->SetStringField(TEXT("mesh_name"), PreviewData->MeshName);
		SummaryJson->SetStringField(TEXT("generation_mode"), PrototypeGenerationModeToString(PreviewData->GenerationMode));
		SummaryJson->SetNumberField(TEXT("primitive_count"), PreviewData->PrimitiveCount);
		SummaryJson->SetNumberField(TEXT("voxel_resolution_x"), PreviewData->VoxelResolution.X);
		SummaryJson->SetNumberField(TEXT("voxel_resolution_y"), PreviewData->VoxelResolution.Y);
		SummaryJson->SetNumberField(TEXT("voxel_resolution_z"), PreviewData->VoxelResolution.Z);
		SummaryJson->SetNumberField(TEXT("occupied_voxel_count"), PreviewData->OccupiedVoxelCount);
		SummaryJson->SetBoolField(TEXT("bridge_success"), BridgeResult.bSuccess);

		FString SummaryPayload;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&SummaryPayload);
		FJsonSerializer::Serialize(SummaryJson, Writer);
		if (!PrototypeMeshBuilder::WriteTextFileUtf8(FPaths::Combine(OutDirectory, TEXT("summary.json")), SummaryPayload, OutError))
		{
			return false;
		}
	}

	return true;
}

FVector FPrototypeMeshBuilderController::GetNextPreviewLocation() const
{
	return GetPreviewLocationForIndex(PreviewRecords.Num());
}

FVector FPrototypeMeshBuilderController::GetPreviewLocationForIndex(int32 PreviewIndex) const
{
	return FVector(0.0, PreviewIndex * PreviewOffsetCm, 0.0);
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

void FPrototypeMeshBuilderController::RefreshPreviewActorOffsets()
{
	for (int32 Index = 0; Index < PreviewRecords.Num(); ++Index)
	{
		FPreviewRecord& PreviewRecord = PreviewRecords[Index];
		PreviewRecord.PreviewOffsetIndex = Index;
		if (PreviewRecord.PreviewActor.IsValid())
		{
			PreviewRecord.PreviewActor->SetActorLocation(GetPreviewLocationForIndex(Index));
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

bool FPrototypeMeshBuilderController::SpawnPreviewActor(const FActiveGenerateJob& CompletedJob, const FGeneratedPreviewData& PreviewData, const FGeneratedMeshBuffers& Buffers, const UE::Geometry::FDynamicMesh3& DynamicMesh, const FPrototypeBridgeResult& BridgeResult, FString& OutError)
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

	const FString PreviewLabel = FString::Printf(TEXT("PrototypeMeshPreview_%s"), *PrototypeMeshBuilder::SanitizeAssetName(PreviewData.MeshName));
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
	PreviewRecord.GenerationMode = PreviewData.GenerationMode;
	PreviewRecord.MeshName = PreviewData.MeshName;
	PreviewRecord.SourcePayloadJson = PreviewData.SourcePayloadJson;
	PreviewRecord.Prompt = CompletedJob.Request.Prompt;
	PreviewRecord.ReasoningEffort = CompletedJob.Request.ReasoningEffort;
	PreviewRecord.RequestedAssetName = CompletedJob.Request.AssetName;
	PreviewRecord.RequestedContentPath = CompletedJob.Request.ContentPath;
	PreviewRecord.Diagnostics = BridgeResult.Diagnostics;
	PreviewRecord.EnqueuedAtSeconds = CompletedJob.EnqueuedAtSeconds;
	PreviewRecord.StartedAtSeconds = CompletedJob.StartedAtSeconds;
	PreviewRecord.CompletedAtSeconds = FPlatformTime::Seconds();
	PreviewRecord.TriangleCount = Buffers.GetTriangleCount();
	PreviewRecord.PrimitiveCount = PreviewData.PrimitiveCount;
	PreviewRecord.VoxelResolution = PreviewData.VoxelResolution;
	PreviewRecord.OccupiedVoxelCount = PreviewData.OccupiedVoxelCount;
	PreviewRecord.PreviewOffsetIndex = PreviewRecords.Num() - 1;

	return true;
}

bool FPrototypeMeshBuilderController::BuildSelectedActorMetadataJson(const AActor& SelectedActor, const FPreviewRecord* PreviewRecord, const FString& SavedMeshPath, const FString& SavedMaterialPath, bool bUsingSharedMaterial, FString& OutJson) const
{
	TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
	RootObject->SetStringField(TEXT("version"), TEXT("1.0"));
	RootObject->SetStringField(TEXT("tool"), TEXT("PrototypeMeshBuilder"));
	RootObject->SetStringField(TEXT("saved_mesh_path"), SavedMeshPath);
	RootObject->SetStringField(TEXT("saved_material_path"), SavedMaterialPath);
	RootObject->SetStringField(TEXT("saved_utc"), FDateTime::UtcNow().ToIso8601());
	RootObject->SetBoolField(TEXT("use_shared_material"), bUsingSharedMaterial);
	RootObject->SetStringField(TEXT("material_scope"), bUsingSharedMaterial ? TEXT("shared") : TEXT("per_mesh"));
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
		RootObject->SetStringField(TEXT("generation_mode"), PrototypeGenerationModeToString(PreviewRecord->GenerationMode));
		RootObject->SetStringField(TEXT("mesh_name"), PreviewRecord->MeshName);
		RootObject->SetNumberField(TEXT("primitive_count"), PreviewRecord->PrimitiveCount);
		RootObject->SetNumberField(TEXT("voxel_resolution_x"), PreviewRecord->VoxelResolution.X);
		RootObject->SetNumberField(TEXT("voxel_resolution_y"), PreviewRecord->VoxelResolution.Y);
		RootObject->SetNumberField(TEXT("voxel_resolution_z"), PreviewRecord->VoxelResolution.Z);
		RootObject->SetNumberField(TEXT("occupied_voxel_count"), PreviewRecord->OccupiedVoxelCount);
		RootObject->SetNumberField(TEXT("triangle_count"), PreviewRecord->TriangleCount);
		RootObject->SetNumberField(TEXT("preview_offset_cm"), PreviewRecord->PreviewOffsetIndex * PreviewOffsetCm);
		RootObject->SetNumberField(TEXT("queue_wait_seconds"), FMath::Max(0.0, PreviewRecord->StartedAtSeconds - PreviewRecord->EnqueuedAtSeconds));
		RootObject->SetNumberField(TEXT("generation_seconds"), FMath::Max(0.0, PreviewRecord->CompletedAtSeconds - PreviewRecord->StartedAtSeconds));
		RootObject->SetNumberField(TEXT("source_payload_char_count"), PreviewRecord->SourcePayloadJson.Len());
		if (!PreviewRecord->SourcePayloadJson.IsEmpty() && PreviewRecord->SourcePayloadJson.Len() <= MaxEmbeddedSourcePayloadChars)
		{
			RootObject->SetStringField(TEXT("source_payload_json"), PreviewRecord->SourcePayloadJson);
		}
		else
		{
			RootObject->SetBoolField(TEXT("source_payload_json_omitted"), true);
		}
		RootObject->SetStringField(TEXT("bridge_diagnostics"), PreviewRecord->Diagnostics);
	}
	else
	{
		RootObject->SetStringField(TEXT("source_type"), TEXT("selected_dynamic_mesh_actor"));
	}

	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
	return FJsonSerializer::Serialize(RootObject, Writer);
}

bool FPrototypeMeshBuilderController::BuildMaterialMetadataJson(const FString& SavedMaterialPath, const FString& SavedMeshPath, bool bUsingSharedMaterial, FString& OutJson) const
{
	TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
	RootObject->SetStringField(TEXT("version"), TEXT("1.0"));
	RootObject->SetStringField(TEXT("tool"), TEXT("PrototypeMeshBuilder"));
	RootObject->SetStringField(TEXT("asset_type"), TEXT("material"));
	RootObject->SetStringField(TEXT("material_path"), SavedMaterialPath);
	RootObject->SetStringField(TEXT("material_scope"), bUsingSharedMaterial ? TEXT("shared") : TEXT("per_mesh"));
	RootObject->SetBoolField(TEXT("use_shared_material"), bUsingSharedMaterial);
	RootObject->SetStringField(TEXT("shading_model"), TEXT("DefaultLit"));
	RootObject->SetStringField(TEXT("vertex_color_source"), TEXT("mesh_vertex_color"));

	if (!bUsingSharedMaterial)
	{
		RootObject->SetStringField(TEXT("linked_mesh_path"), SavedMeshPath);
	}

	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJson);
	return FJsonSerializer::Serialize(RootObject, Writer);
}
