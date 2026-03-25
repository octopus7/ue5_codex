#pragma once

#include "CoreMinimal.h"

class UMaterialInterface;
class UStaticMesh;
class UVoxImportedAssetUserData;
class FFeedbackContext;
struct FMeshDescription;

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
	bool BuildStaticMeshAsset(UStaticMesh* StaticMesh, const FMeshDescription& MeshDescription, const FVoxMeshAssetBuildParams& BuildParams, FFeedbackContext* Warn);
}
