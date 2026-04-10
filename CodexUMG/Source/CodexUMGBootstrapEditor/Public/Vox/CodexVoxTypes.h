#pragma once

#include "CoreMinimal.h"

struct FVoxVoxel
{
	uint8 X = 0;
	uint8 Y = 0;
	uint8 Z = 0;
	uint8 ColorIndex = 0;
};

struct FVoxModel
{
	int32 SizeX = 0;
	int32 SizeY = 0;
	int32 SizeZ = 0;
	TArray<FVoxVoxel> Voxels;
	TArray<FColor> Palette;

	bool HasExpectedResolution() const
	{
		return SizeX == 32 && SizeY == 32 && SizeZ == 32;
	}
};

struct FVoxAssetManifestEntry
{
	FString Id;
	FString DisplayName;
	FString SourceVoxFile;
	FString TargetPackagePath;
	FString TargetAssetName;
	FString Category;
	bool bRepeatable = false;
	FString PivotRule;
	FString CollisionType;
	FString Notes;
};

struct FVoxAssetBuildSettings
{
	FString ManifestPath;
	bool bVerbose = false;
	bool bOverwriteExisting = true;
};

struct FVoxAssetBuildResult
{
	int32 ProcessedCount = 0;
	int32 BuiltCount = 0;
	TArray<FString> GeneratedAssetPaths;
};
