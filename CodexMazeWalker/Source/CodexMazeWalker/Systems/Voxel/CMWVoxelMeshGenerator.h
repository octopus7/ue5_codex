#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"

class UCMWVoxelDataAsset;

struct CODEXMAZEWALKER_API FCMWVoxelMeshBuffers
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FLinearColor> Colors;
	TArray<FProcMeshTangent> Tangents;

	void Reset();
	bool HasGeometry() const;
};

class CODEXMAZEWALKER_API FCMWVoxelMeshGenerator
{
public:
	static bool BuildMeshBuffers(const UCMWVoxelDataAsset* VoxelDataAsset, FCMWVoxelMeshBuffers& OutMeshBuffers);
};
