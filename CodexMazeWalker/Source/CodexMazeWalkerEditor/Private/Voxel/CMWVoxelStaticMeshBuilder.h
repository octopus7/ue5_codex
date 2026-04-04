#pragma once

#include "CoreMinimal.h"

class UCMWVoxelDataAsset;
class UMaterialInterface;
class UStaticMesh;

class FCMWVoxelStaticMeshBuilder
{
public:
	static bool BuildStaticMesh(UStaticMesh* StaticMesh, const UCMWVoxelDataAsset& VoxelDataAsset, UMaterialInterface* SharedMaterial, FString& OutError);
};
