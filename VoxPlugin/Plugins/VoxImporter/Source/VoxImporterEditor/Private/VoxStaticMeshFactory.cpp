#include "VoxStaticMeshFactory.h"

#include "Editor.h"
#include "EditorFramework/AssetImportData.h"
#include "Engine/StaticMesh.h"
#include "MeshDescription.h"
#include "Misc/FeedbackContext.h"
#include "Misc/Paths.h"
#include "Subsystems/ImportSubsystem.h"
#include "VoxMeshBuilder.h"
#include "VoxParser.h"
#include "VoxStaticMeshUtilities.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VoxStaticMeshFactory)

UVoxStaticMeshFactory::UVoxStaticMeshFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UStaticMesh::StaticClass();
	bCreateNew = false;
	bEditorImport = true;
	bText = false;
	Formats.Add(TEXT("vox;MagicaVoxel voxel model"));
	ImportPriority = DefaultImportPriority;
}

bool UVoxStaticMeshFactory::FactoryCanImport(const FString& Filename)
{
	return FPaths::GetExtension(Filename).Equals(TEXT("vox"), ESearchCase::IgnoreCase);
}

UObject* UVoxStaticMeshFactory::FactoryCreateFile(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	const FString& Filename,
	const TCHAR* Parms,
	FFeedbackContext* Warn,
	bool& bOutOperationCanceled)
{
	return ImportFromFile(InClass, InParent, InName, Flags, Filename, Warn, bOutOperationCanceled, nullptr);
}

bool UVoxStaticMeshFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	if (const UStaticMesh* StaticMesh = Cast<UStaticMesh>(Obj))
	{
		if (VoxStaticMeshUtilities::IsPrimaryVoxSourceStaticMesh(StaticMesh))
		{
			const FString SourceFilename = VoxStaticMeshUtilities::GetVoxSourceFilename(StaticMesh);
			if (!SourceFilename.IsEmpty())
			{
				OutFilenames.Add(SourceFilename);
				return true;
			}
		}
	}

	return false;
}

void UVoxStaticMeshFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UStaticMesh* StaticMesh = Cast<UStaticMesh>(Obj);
	if (StaticMesh && VoxStaticMeshUtilities::IsPrimaryVoxSourceStaticMesh(StaticMesh) && ensure(NewReimportPaths.Num() == 1) && StaticMesh->GetAssetImportData())
	{
		StaticMesh->GetAssetImportData()->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

EReimportResult::Type UVoxStaticMeshFactory::Reimport(UObject* Obj)
{
	UStaticMesh* StaticMesh = Cast<UStaticMesh>(Obj);
	if (!StaticMesh)
	{
		return EReimportResult::Failed;
	}

	if (!VoxStaticMeshUtilities::IsPrimaryVoxSourceStaticMesh(StaticMesh))
	{
		return EReimportResult::Failed;
	}

	const FString Filename = VoxStaticMeshUtilities::GetVoxSourceFilename(StaticMesh);
	if (Filename.IsEmpty() || !FactoryCanImport(Filename) || !FPaths::FileExists(Filename))
	{
		return EReimportResult::Failed;
	}

	bool bOutOperationCanceled = false;
	if (!ImportFromFile(StaticMesh->GetClass(), StaticMesh->GetOuter(), StaticMesh->GetFName(), RF_Public | RF_Standalone, Filename, GWarn, bOutOperationCanceled, StaticMesh))
	{
		return bOutOperationCanceled ? EReimportResult::Cancelled : EReimportResult::Failed;
	}

	return EReimportResult::Succeeded;
}

UObject* UVoxStaticMeshFactory::ImportFromFile(
	UClass* InClass,
	UObject* InParent,
	FName InName,
	EObjectFlags Flags,
	const FString& Filename,
	FFeedbackContext* Warn,
	bool& bOutOperationCanceled,
	UStaticMesh* ExistingMesh)
{
	bOutOperationCanceled = false;
	AdditionalImportedObjects.Empty();

	const FString FileExtension = FPaths::GetExtension(Filename);
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, *FileExtension);

	FVoxModelData Model;
	FString ErrorMessage;
	if (!FVoxParser::ParseFile(Filename, Model, ErrorMessage))
	{
		if (Warn)
		{
			Warn->Logf(ELogVerbosity::Error, TEXT("%s"), *ErrorMessage);
		}
		GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, nullptr);
		return nullptr;
	}

	FMeshDescription MeshDescription;
	if (!FVoxMeshBuilder::BuildMeshDescription(Model, MeshDescription, ErrorMessage, FVoxMeshBuilder::DefaultVoxelSize))
	{
		if (Warn)
		{
			Warn->Logf(ELogVerbosity::Error, TEXT("%s"), *ErrorMessage);
		}
		GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, nullptr);
		return nullptr;
	}

	UStaticMesh* StaticMesh = ExistingMesh;
	if (!StaticMesh)
	{
		StaticMesh = NewObject<UStaticMesh>(InParent, InClass, InName, Flags);
	}

	if (!StaticMesh)
	{
		if (Warn)
		{
			Warn->Logf(ELogVerbosity::Error, TEXT("Failed to allocate Static Mesh asset for %s."), *Filename);
		}
		GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, nullptr);
		return nullptr;
	}

	const FVoxMeshAssetBuildParams BuildParams
	{
		Filename,
		false,
		0.0f,
		FString()
	};

	if (!VoxStaticMeshUtilities::BuildStaticMeshAsset(StaticMesh, MeshDescription, BuildParams, Warn))
	{
		GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, nullptr);
		return nullptr;
	}

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, StaticMesh);
	return StaticMesh;
}
