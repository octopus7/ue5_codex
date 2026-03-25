#pragma once

#include "CoreMinimal.h"

class UMaterialInterface;
class UStaticMesh;
class UVoxImportedAssetUserData;
class FFeedbackContext;
struct FMeshDescription;
struct FVoxModelData;

struct FVoxMeshAssetBuildParams
{
	FString SourceFilename;
	bool bIsSmoothReconstruction = false;
	float ReconstructionResolutionScale = 0.0f;
	FString GeneratedFromAssetPath;
};

namespace VoxStaticMeshUtilities
{
	UMaterialInterface* ResolveVoxelMaterial();
	UMaterialInterface* ResolveVoxelBakedMaterial();
	const UVoxImportedAssetUserData* GetVoxImportedAssetUserData(const UStaticMesh* StaticMesh);
	bool IsVoxImportedStaticMesh(const UStaticMesh* StaticMesh);
	bool IsPrimaryVoxSourceStaticMesh(const UStaticMesh* StaticMesh);
	FString GetVoxSourceFilename(const UStaticMesh* StaticMesh);
	FVoxMeshAssetBuildParams MakeBuildParamsFromStaticMesh(const UStaticMesh* StaticMesh);
	bool PrepareVertexColorTextureBakeMeshDescription(const FMeshDescription& SourceMeshDescription, FMeshDescription& OutMeshDescription, TArray<FVector2D>& OutGeneratedUVs, FString& OutError);
	bool SimplifyMeshDescription(const FMeshDescription& SourceMeshDescription, float TargetPercentTriangles, FMeshDescription& OutMeshDescription, FString& OutError);
	bool ReapplyVoxelVertexColors(const FVoxModelData& Model, FMeshDescription& MeshDescription, float VoxelSize, FString& OutError);
	bool SimplifyReconstructedMeshDescription(const FMeshDescription& SourceMeshDescription, const FVoxModelData& Model, float VoxelSize, float TargetPercentTriangles, FMeshDescription& OutMeshDescription, FString& OutError);
	bool BuildStaticMeshAsset(UStaticMesh* StaticMesh, const FMeshDescription& MeshDescription, const FVoxMeshAssetBuildParams& BuildParams, FFeedbackContext* Warn);
}
