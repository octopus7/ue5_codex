#pragma once

#include "CoreMinimal.h"
#include "VoxModel.h"

struct FMeshDescription;

struct VOXIMPORTERRUNTIME_API FVoxSmoothBuildSettings
{
	static constexpr float DefaultDesiredResolutionScale = 2.0f;
	static constexpr int32 DefaultMaxGridDimension = 128;
	static constexpr int32 DefaultPaddingVoxels = 1;
	static constexpr float DefaultIsoValue = 0.5f;
	static constexpr float DefaultDensityBlurBlend = 0.15f;

	float DesiredResolutionScale = DefaultDesiredResolutionScale;
	int32 MaxGridDimension = DefaultMaxGridDimension;
	int32 PaddingVoxels = DefaultPaddingVoxels;
	float IsoValue = DefaultIsoValue;
	float DensityBlurBlend = DefaultDensityBlurBlend;
};

struct VOXIMPORTERRUNTIME_API FVoxSmoothBuildMetadata
{
	float ResolutionScaleUsed = 1.0f;
	FIntVector GridResolution = FIntVector::ZeroValue;
};

class VOXIMPORTERRUNTIME_API FVoxSmoothMeshBuilder
{
public:
	static bool BuildSmoothMeshDescription(
		const FVoxModelData& Model,
		FMeshDescription& OutMeshDescription,
		FString& OutError,
		float VoxelSize = 1.0f,
		const FVoxSmoothBuildSettings& Settings = FVoxSmoothBuildSettings(),
		FVoxSmoothBuildMetadata* OutMetadata = nullptr);
};
