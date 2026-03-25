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
	const UVoxImportedAssetUserData* GetVoxImportedAssetUserData(const UStaticMesh* StaticMesh);
	bool IsVoxImportedStaticMesh(const UStaticMesh* StaticMesh);
	bool IsPrimaryVoxSourceStaticMesh(const UStaticMesh* StaticMesh);
	FString GetVoxSourceFilename(const UStaticMesh* StaticMesh);
	bool SimplifyMeshDescription(const FMeshDescription& SourceMeshDescription, float TargetPercentTriangles, FMeshDescription& OutMeshDescription, FString& OutError);
	bool ReapplyVoxelVertexColors(const FVoxModelData& Model, FMeshDescription& MeshDescription, float VoxelSize, FString& OutError);
	bool SimplifyReconstructedMeshDescription(const FMeshDescription& SourceMeshDescription, const FVoxModelData& Model, float VoxelSize, float TargetPercentTriangles, FMeshDescription& OutMeshDescription, FString& OutError);
	bool BuildStaticMeshAsset(UStaticMesh* StaticMesh, const FMeshDescription& MeshDescription, const FVoxMeshAssetBuildParams& BuildParams, FFeedbackContext* Warn);
}
