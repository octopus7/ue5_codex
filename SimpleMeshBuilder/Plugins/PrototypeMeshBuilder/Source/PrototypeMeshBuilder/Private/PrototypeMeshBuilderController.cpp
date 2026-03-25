#include "PrototypeMeshBuilderController.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DynamicMeshActor.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "IAssetTools.h"
#include "IContentBrowserSingleton.h"
#include "Misc/PackageName.h"
#include "PrototypeCodexMeshBridge.h"
#include "PrototypeMeshBuilderBridge.h"
#include "PrototypeMeshGeneration.h"

FPrototypeMeshBuilderController::FPrototypeMeshBuilderController()
	: Bridge(MakeUnique<FPrototypeCodexMeshBridge>())
	, ContentPath(TEXT("/Game/Generated/PrototypeMeshes"))
	, ReasoningEffort(TEXT("medium"))
	, StatusMessage(TEXT("Enter a prompt and generate a prototype mesh."))
{
}

FPrototypeMeshBuilderController::~FPrototypeMeshBuilderController()
{
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

bool FPrototypeMeshBuilderController::CanSave() const
{
	return bHasGeneratedResult && CurrentDynamicMesh.IsValid();
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

	FPrototypeBridgeResult BridgeResult = Bridge->GenerateDsl(Request);
	if (!BridgeResult.bSuccess)
	{
		ResetGeneratedState();
		const FString FailureReason = BridgeResult.ErrorMessage.IsEmpty() ? TEXT("unknown bridge failure") : BridgeResult.ErrorMessage;
		SetStatus(FString::Printf(TEXT("Generate failed: %s"), *FailureReason));
		return;
	}

	FPrototypeShapeDsl ParsedDsl;
	FString ParseError;
	if (!PrototypeMeshBuilder::ParseShapeDslJson(BridgeResult.RawDslJson, ParsedDsl, ParseError))
	{
		ResetGeneratedState();
		SetStatus(FString::Printf(TEXT("DSL parse failed: %s"), *ParseError));
		return;
	}

	FGeneratedMeshBuffers GeneratedBuffers;
	FString BuildError;
	if (!PrototypeMeshBuilder::BuildMeshBuffers(ParsedDsl, GeneratedBuffers, BuildError))
	{
		ResetGeneratedState();
		SetStatus(FString::Printf(TEXT("Mesh build failed: %s"), *BuildError));
		return;
	}

	TUniquePtr<UE::Geometry::FDynamicMesh3> GeneratedDynamicMesh = MakeUnique<UE::Geometry::FDynamicMesh3>(UE::Geometry::EMeshComponents::None);
	if (!PrototypeMeshBuilder::BuildDynamicMesh(GeneratedBuffers, *GeneratedDynamicMesh, BuildError))
	{
		ResetGeneratedState();
		SetStatus(FString::Printf(TEXT("Preview mesh failed: %s"), *BuildError));
		return;
	}

	FString PreviewError;
	if (!EnsurePreviewActor(PreviewError))
	{
		ResetGeneratedState();
		SetStatus(FString::Printf(TEXT("Preview spawn failed: %s"), *PreviewError));
		return;
	}

	PreviewActor->GetDynamicMeshComponent()->SetMesh(UE::Geometry::FDynamicMesh3(*GeneratedDynamicMesh));

	CurrentDsl = MoveTemp(ParsedDsl);
	CurrentBuffers = MoveTemp(GeneratedBuffers);
	CurrentDynamicMesh = MoveTemp(GeneratedDynamicMesh);
	bHasGeneratedResult = true;

	if (AssetName.TrimStartAndEnd().IsEmpty())
	{
		AssetName = PrototypeMeshBuilder::SanitizeAssetName(CurrentDsl.MeshName);
	}

	ContentPath = Request.ContentPath;
	SetStatus(FString::Printf(TEXT("Generated %d primitive(s), %d triangle(s). Preview updated."), CurrentDsl.Primitives.Num(), CurrentBuffers.GetTriangleCount()));
}

void FPrototypeMeshBuilderController::Save()
{
	if (!CanSave())
	{
		SetStatus(TEXT("Nothing to save. Generate a mesh first."));
		return;
	}

	const FString NormalizedContentPath = PrototypeMeshBuilder::NormalizeContentPath(ContentPath);
	if (!FPackageName::IsValidLongPackageName(NormalizedContentPath))
	{
		SetStatus(TEXT("Content path must be a valid /Game package path."));
		return;
	}

	const FString SanitizedAssetName = PrototypeMeshBuilder::SanitizeAssetName(AssetName.IsEmpty() ? CurrentDsl.MeshName : AssetName);
	const FString BasePackagePath = NormalizedContentPath / SanitizedAssetName;

	FString FinalPackagePath = BasePackagePath;
	FString FinalAssetName = SanitizedAssetName;
	if (FPackageName::DoesPackageExist(BasePackagePath))
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
		AssetToolsModule.Get().CreateUniqueAssetName(BasePackagePath, TEXT("_001"), FinalPackagePath, FinalAssetName);
	}

	UStaticMesh* SavedMesh = nullptr;
	FString SaveError;
	if (!PrototypeMeshBuilder::BuildStaticMeshAsset(FinalPackagePath, FinalAssetName, *CurrentDynamicMesh, SavedMesh, SaveError))
	{
		SetStatus(FString::Printf(TEXT("Save failed: %s"), *SaveError));
		return;
	}

	TArray<UPackage*> PackagesToSave;
	PackagesToSave.Add(SavedMesh->GetOutermost());
	if (!UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true))
	{
		SetStatus(TEXT("StaticMesh asset was created but package save failed."));
		return;
	}

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	ContentBrowserModule.Get().SyncBrowserToAssets(TArray<UObject*>{SavedMesh});

	SetStatus(FString::Printf(TEXT("Saved StaticMesh to %s"), *FinalPackagePath));
}

void FPrototypeMeshBuilderController::Clear()
{
	ResetGeneratedState();
	SetStatus(TEXT("Cleared preview and generated mesh state."));
}

void FPrototypeMeshBuilderController::CleanupPreview()
{
	if (PreviewActor.IsValid())
	{
		if (UWorld* World = PreviewActor->GetWorld())
		{
			World->DestroyActor(PreviewActor.Get());
		}
	}

	PreviewActor.Reset();
}

void FPrototypeMeshBuilderController::ResetGeneratedState()
{
	bHasGeneratedResult = false;
	CurrentDsl = FPrototypeShapeDsl();
	CurrentBuffers.Reset();
	CurrentDynamicMesh.Reset();
	CleanupPreview();
}

void FPrototypeMeshBuilderController::SetStatus(const FString& InStatus)
{
	StatusMessage = InStatus;
}

bool FPrototypeMeshBuilderController::EnsurePreviewActor(FString& OutError)
{
	UWorld* EditorWorld = GetEditorWorld();
	if (!EditorWorld)
	{
		OutError = TEXT("Editor world is not available.");
		return false;
	}

	if (PreviewActor.IsValid())
	{
		return true;
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

	SpawnedActor->SetActorLabel(TEXT("PrototypeMeshPreview"));
	SpawnedActor->SetActorEnableCollision(false);
	PreviewActor = SpawnedActor;
	return true;
}

UWorld* FPrototypeMeshBuilderController::GetEditorWorld() const
{
	return GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
}
