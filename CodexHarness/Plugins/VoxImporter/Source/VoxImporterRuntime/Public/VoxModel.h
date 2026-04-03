#pragma once

#include "Containers/Array.h"
#include "CoreMinimal.h"

struct VOXIMPORTERRUNTIME_API FVoxVoxel
{
	int32 X = 0;
	int32 Y = 0;
	int32 Z = 0;
	uint8 ColorIndex = 0;
};

struct VOXIMPORTERRUNTIME_API FVoxModelData
{
	FIntVector Size = FIntVector::ZeroValue;
	TArray<FVoxVoxel> Voxels;
	TArray<FColor> Palette;

	void Reset()
	{
		Size = FIntVector::ZeroValue;
		Voxels.Reset();
		Palette.Reset();
	}

	const FColor& GetColor(uint8 InColorIndex) const
	{
		static const FColor TransparentColor(0, 0, 0, 0);

		if (Palette.IsValidIndex(InColorIndex))
		{
			return Palette[InColorIndex];
		}

		return TransparentColor;
	}
};
