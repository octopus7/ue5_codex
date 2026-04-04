#include "Commandlets/CMWImportVoxCommandlet.h"

#include "Assets/CMWEditorAssetUtils.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Misc/Parse.h"
#include "Systems/Voxel/CMWVoxelDataAsset.h"
#include "Voxel/CMWVoxParser.h"
#include "Voxel/CMWVoxelStaticMeshBuilder.h"

UCMWImportVoxCommandlet::UCMWImportVoxCommandlet()
{
	IsClient = false;
	IsEditor = true;
	LogToConsole = true;
}

int32 UCMWImportVoxCommandlet::Main(const FString& Params)
{
	FString SourceFilePath;
	FString DataAssetPath = TEXT("/Game/Voxel/Data/DA_ImportedVoxel");
	FString MeshAssetPath = TEXT("/Game/Voxel/Generated/SM_ImportedVoxel");
	FString MaterialPath = TEXT("/Game/Voxel/Materials/M_VoxelVertexColor");

	FParse::Value(*Params, TEXT("Source="), SourceFilePath);
	FParse::Value(*Params, TEXT("DataAsset="), DataAssetPath);
	FParse::Value(*Params, TEXT("MeshAsset="), MeshAssetPath);
	FParse::Value(*Params, TEXT("Material="), MaterialPath);

	if (SourceFilePath.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Missing required parameter: Source=<absolute_or_relative_vox_path>"));
		return 1;
	}

	FCMWVoxModelData ParsedModelData;
	FString ParseError;
	if (!FCMWVoxParser::ParseFile(SourceFilePath, ParsedModelData, ParseError))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse VOX file '%s': %s"), *SourceFilePath, *ParseError);
		return 1;
	}

	UCMWVoxelDataAsset* VoxelDataAsset = CMWEditorAssetUtils::LoadOrCreateAsset<UCMWVoxelDataAsset>(DataAssetPath, [](UPackage* Package, const FName& AssetName)
	{
		return NewObject<UCMWVoxelDataAsset>(Package, UCMWVoxelDataAsset::StaticClass(), AssetName, RF_Public | RF_Standalone);
	});

	if (!VoxelDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create voxel data asset '%s'."), *DataAssetPath);
		return 1;
	}

	VoxelDataAsset->Modify();
	VoxelDataAsset->Dimensions = ParsedModelData.Dimensions;
	VoxelDataAsset->ResizeToDimensions();
	VoxelDataAsset->SourceFilePath = SourceFilePath;
	VoxelDataAsset->Voxels = ParsedModelData.Voxels;
	VoxelDataAsset->MarkPackageDirty();

	FString SaveError;
	if (!CMWEditorAssetUtils::SaveAsset(VoxelDataAsset, &SaveError))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save voxel data asset '%s': %s"), *VoxelDataAsset->GetPathName(), *SaveError);
		return 1;
	}

	if (!MeshAssetPath.IsEmpty())
	{
		UMaterialInterface* SharedMaterial = CMWEditorAssetUtils::LoadAsset<UMaterialInterface>(MaterialPath);
		if (!SharedMaterial)
		{
			UE_LOG(LogTemp, Error, TEXT("Shared voxel material '%s' could not be loaded. Run CMWBootstrapContent first."), *MaterialPath);
			return 1;
		}

		UStaticMesh* StaticMesh = CMWEditorAssetUtils::LoadOrCreateAsset<UStaticMesh>(MeshAssetPath, [](UPackage* Package, const FName& AssetName)
		{
			return NewObject<UStaticMesh>(Package, UStaticMesh::StaticClass(), AssetName, RF_Public | RF_Standalone);
		});

		if (!StaticMesh)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create static mesh asset '%s'."), *MeshAssetPath);
			return 1;
		}

		FString MeshBuildError;
		if (!FCMWVoxelStaticMeshBuilder::BuildStaticMesh(StaticMesh, *VoxelDataAsset, SharedMaterial, MeshBuildError))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to build static mesh '%s': %s"), *StaticMesh->GetPathName(), *MeshBuildError);
			return 1;
		}

		if (!CMWEditorAssetUtils::SaveAsset(StaticMesh, &SaveError))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save static mesh asset '%s': %s"), *StaticMesh->GetPathName(), *SaveError);
			return 1;
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Imported VOX file '%s' into '%s' and '%s' successfully."), *SourceFilePath, *DataAssetPath, *MeshAssetPath);
	return 0;
}
