#pragma once

#include "CoreMinimal.h"
#include "VoxModel.h"

struct FMeshDescription;

class VOXIMPORTERRUNTIME_API FVoxMeshBuilder
{
public:
	static constexpr float DefaultVoxelSize = 1.0f;

	static bool BuildMeshDescription(const FVoxModelData& Model, FMeshDescription& OutMeshDescription, FString& OutError, float VoxelSize = DefaultVoxelSize);
};
