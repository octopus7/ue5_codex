#include "VoxStaticMeshFactory.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "EditorFramework/AssetImportData.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "MeshDescription.h"
#include "Misc/FeedbackContext.h"
#include "Misc/Paths.h"
#include "PhysicsEngine/BodySetup.h"
#include "Subsystems/ImportSubsystem.h"
#include "VoxMeshBuilder.h"
#include "VoxParser.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(VoxStaticMeshFactory)

namespace
{
	const TCHAR* GVoxelMaterialPackagePath = TEXT("/Game/VoxImporter/Materials/M_VoxVertexColor");
	const TCHAR* GVoxelMaterialObjectPath = TEXT("/Game/VoxImporter/Materials/M_VoxVertexColor.M_VoxVertexColor");
	const TCHAR* GVoxelMaterialAssetName = TEXT("M_VoxVertexColor");

	UMaterialInterface* CreateVoxelProjectMaterial()
	{
		UPackage* Package = CreatePackage(GVoxelMaterialPackagePath);
		if (!Package)
		{
			return nullptr;
		}

		UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();
		UMaterial* Material = Cast<UMaterial>(
			MaterialFactory->FactoryCreateNew(
				UMaterial::StaticClass(),
				Package,
				*FString(GVoxelMaterialAssetName),
				RF_Standalone | RF_Public | RF_Transactional,
				nullptr,
				GWarn));

		if (!Material)
		{
			return nullptr;
		}

		Material->PreEditChange(nullptr);

		UMaterialEditorOnlyData* MaterialEditorOnly = Material->GetEditorOnlyData();

		UMaterialExpressionVertexColor* VertexColor = NewObject<UMaterialExpressionVertexColor>(Material);
		VertexColor->MaterialExpressionEditorX = -400;
		VertexColor->MaterialExpressionEditorY = -220;
		Material->GetExpressionCollection().AddExpression(VertexColor);
		MaterialEditorOnly->BaseColor.Connect(0, VertexColor);

		auto AddConstantExpression = [Material](float Value, int32 X, int32 Y)
		{
			UMaterialExpressionConstant* Constant = NewObject<UMaterialExpressionConstant>(Material);
			Constant->R = Value;
			Constant->MaterialExpressionEditorX = X;
			Constant->MaterialExpressionEditorY = Y;
			Material->GetExpressionCollection().AddExpression(Constant);
			return Constant;
		};

		MaterialEditorOnly->Roughness.Connect(0, AddConstantExpression(1.0f, -400, 0));
		MaterialEditorOnly->Specular.Connect(0, AddConstantExpression(0.0f, -400, 120));
		MaterialEditorOnly->Metallic.Connect(0, AddConstantExpression(0.0f, -400, 240));

		Material->PostEditChange();
		Material->MarkPackageDirty();
		Package->SetDirtyFlag(true);
		FAssetRegistryModule::AssetCreated(Material);
		Material->ForceRecompileForRendering();

		return Material;
	}

	UMaterialInterface* ResolveVoxelMaterial()
	{
		if (UMaterialInterface* ExistingMaterial = LoadObject<UMaterialInterface>(nullptr, GVoxelMaterialObjectPath))
		{
			return ExistingMaterial;
		}

		if (UMaterialInterface* CreatedMaterial = CreateVoxelProjectMaterial())
		{
			return CreatedMaterial;
		}

		if (GEngine && GEngine->VertexColorViewModeMaterial_ColorOnly)
		{
			return GEngine->VertexColorViewModeMaterial_ColorOnly;
		}

		return LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/EngineDebugMaterials/VertexColorViewMode_ColorOnly.VertexColorViewMode_ColorOnly"));
	}
}

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
		if (const UAssetImportData* AssetImportData = StaticMesh->GetAssetImportData())
		{
			AssetImportData->ExtractFilenames(OutFilenames);
			return OutFilenames.Num() > 0;
		}
	}

	return false;
}

void UVoxStaticMeshFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	UStaticMesh* StaticMesh = Cast<UStaticMesh>(Obj);
	if (StaticMesh && ensure(NewReimportPaths.Num() == 1) && StaticMesh->GetAssetImportData())
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

	UAssetImportData* AssetImportData = StaticMesh->GetAssetImportData();
	if (!AssetImportData)
	{
		return EReimportResult::Failed;
	}

	const FString Filename = AssetImportData->GetFirstFilename();
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

	StaticMesh->SetFlags(RF_Transactional);
	StaticMesh->Modify();
	StaticMesh->SetImportVersion(EImportStaticMeshVersion::LastVersion);
	StaticMesh->SetNumSourceModels(1);

	FStaticMeshSourceModel& SourceModel = StaticMesh->GetSourceModel(0);
	SourceModel.BuildSettings.bRecomputeNormals = false;
	SourceModel.BuildSettings.bRecomputeTangents = false;
	SourceModel.BuildSettings.bGenerateLightmapUVs = false;
	SourceModel.BuildSettings.bUseFullPrecisionUVs = true;
	SourceModel.BuildSettings.bUseHighPrecisionTangentBasis = false;

	const FName MaterialSlotName(TEXT("VoxelMaterial"));
	StaticMesh->GetStaticMaterials().Reset();
	StaticMesh->GetStaticMaterials().Add(FStaticMaterial(ResolveVoxelMaterial(), MaterialSlotName, MaterialSlotName));

	TArray<const FMeshDescription*> MeshDescriptions;
	MeshDescriptions.Add(&MeshDescription);

	UStaticMesh::FBuildMeshDescriptionsParams BuildParams;
	BuildParams.bCommitMeshDescription = true;
	BuildParams.bFastBuild = false;
	BuildParams.bBuildSimpleCollision = false;
	BuildParams.bMarkPackageDirty = true;

	if (!StaticMesh->BuildFromMeshDescriptions(MeshDescriptions, BuildParams))
	{
		if (Warn)
		{
			Warn->Logf(ELogVerbosity::Error, TEXT("Failed to build Static Mesh render data for %s."), *Filename);
		}
		GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, nullptr);
		return nullptr;
	}

	StaticMesh->CreateBodySetup();
	if (UBodySetup* BodySetup = StaticMesh->GetBodySetup())
	{
		BodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;
		BodySetup->InvalidatePhysicsData();
		BodySetup->CreatePhysicsMeshes();
	}

	if (!StaticMesh->GetAssetImportData())
	{
		StaticMesh->SetAssetImportData(NewObject<UAssetImportData>(StaticMesh, TEXT("AssetImportData")));
	}

	StaticMesh->GetAssetImportData()->Update(Filename);
	StaticMesh->MarkPackageDirty();
	StaticMesh->PostEditChange();

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, StaticMesh);
	return StaticMesh;
}
