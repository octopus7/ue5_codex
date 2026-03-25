#include "VoxStaticMeshUtilities.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorFramework/AssetImportData.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Materials/MaterialInterface.h"
#include "MeshDescription.h"
#include "Misc/FeedbackContext.h"
#include "Misc/Paths.h"
#include "PhysicsEngine/BodySetup.h"
#include "VoxImportedAssetUserData.h"

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

	bool HasVoxSourceImportPath(const UStaticMesh* StaticMesh, FString* OutFilename = nullptr)
	{
		if (!StaticMesh || !StaticMesh->GetAssetImportData())
		{
			return false;
		}

		const FString Filename = StaticMesh->GetAssetImportData()->GetFirstFilename();
		if (Filename.IsEmpty() || !FPaths::GetExtension(Filename).Equals(TEXT("vox"), ESearchCase::IgnoreCase))
		{
			return false;
		}

		if (OutFilename)
		{
			*OutFilename = Filename;
		}

		return true;
	}

	void UpdateVoxImportedAssetUserData(UStaticMesh* StaticMesh, const FVoxMeshAssetBuildParams& BuildParams)
	{
		if (!StaticMesh)
		{
			return;
		}

		StaticMesh->RemoveUserDataOfClass(UVoxImportedAssetUserData::StaticClass());

		UVoxImportedAssetUserData* UserData = NewObject<UVoxImportedAssetUserData>(StaticMesh, UVoxImportedAssetUserData::StaticClass(), NAME_None, RF_Transactional);
		UserData->SourceFilename = BuildParams.SourceFilename;
		UserData->bIsSmoothReconstruction = BuildParams.bIsSmoothReconstruction;
		UserData->ReconstructionResolutionScale = BuildParams.ReconstructionResolutionScale;
		UserData->GeneratedFromAssetPath = BuildParams.GeneratedFromAssetPath;
		StaticMesh->AddAssetUserData(UserData);
	}
}

UMaterialInterface* VoxStaticMeshUtilities::ResolveVoxelMaterial()
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

const UVoxImportedAssetUserData* VoxStaticMeshUtilities::GetVoxImportedAssetUserData(const UStaticMesh* StaticMesh)
{
	if (!StaticMesh)
	{
		return nullptr;
	}

	if (const TArray<UAssetUserData*>* AssetUserData = StaticMesh->GetAssetUserDataArray())
	{
		for (const UAssetUserData* UserData : *AssetUserData)
		{
			if (const UVoxImportedAssetUserData* VoxUserData = Cast<UVoxImportedAssetUserData>(UserData))
			{
				return VoxUserData;
			}
		}
	}

	return nullptr;
}

bool VoxStaticMeshUtilities::IsVoxImportedStaticMesh(const UStaticMesh* StaticMesh)
{
	return GetVoxImportedAssetUserData(StaticMesh) != nullptr || HasVoxSourceImportPath(StaticMesh);
}

bool VoxStaticMeshUtilities::IsPrimaryVoxSourceStaticMesh(const UStaticMesh* StaticMesh)
{
	if (const UVoxImportedAssetUserData* UserData = GetVoxImportedAssetUserData(StaticMesh))
	{
		return !UserData->bIsSmoothReconstruction;
	}

	return HasVoxSourceImportPath(StaticMesh);
}

FString VoxStaticMeshUtilities::GetVoxSourceFilename(const UStaticMesh* StaticMesh)
{
	if (const UVoxImportedAssetUserData* UserData = GetVoxImportedAssetUserData(StaticMesh))
	{
		if (!UserData->SourceFilename.IsEmpty())
		{
			return UserData->SourceFilename;
		}
	}

	FString SourceFilename;
	HasVoxSourceImportPath(StaticMesh, &SourceFilename);
	return SourceFilename;
}

bool VoxStaticMeshUtilities::BuildStaticMeshAsset(UStaticMesh* StaticMesh, const FMeshDescription& MeshDescription, const FVoxMeshAssetBuildParams& BuildParams, FFeedbackContext* Warn)
{
	if (!StaticMesh)
	{
		if (Warn)
		{
			Warn->Logf(ELogVerbosity::Error, TEXT("Static Mesh asset target was null."));
		}
		return false;
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

	UStaticMesh::FBuildMeshDescriptionsParams BuildMeshParams;
	BuildMeshParams.bCommitMeshDescription = true;
	BuildMeshParams.bFastBuild = false;
	BuildMeshParams.bBuildSimpleCollision = false;
	BuildMeshParams.bMarkPackageDirty = true;

	if (!StaticMesh->BuildFromMeshDescriptions(MeshDescriptions, BuildMeshParams))
	{
		if (Warn)
		{
			Warn->Logf(ELogVerbosity::Error, TEXT("Failed to build Static Mesh render data for %s."), *StaticMesh->GetName());
		}
		return false;
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

	if (!BuildParams.SourceFilename.IsEmpty())
	{
		StaticMesh->GetAssetImportData()->Update(BuildParams.SourceFilename);
	}

	UpdateVoxImportedAssetUserData(StaticMesh, BuildParams);

	StaticMesh->MarkPackageDirty();
	StaticMesh->PostEditChange();
	return true;
}
