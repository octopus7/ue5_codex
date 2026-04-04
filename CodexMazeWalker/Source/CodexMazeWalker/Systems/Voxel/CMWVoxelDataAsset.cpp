#include "Systems/Voxel/CMWVoxelDataAsset.h"

UCMWVoxelDataAsset::UCMWVoxelDataAsset()
{
	ResizeToDimensions();
}

void UCMWVoxelDataAsset::PostLoad()
{
	Super::PostLoad();
	ResizeToDimensions();
}

#if WITH_EDITOR
void UCMWVoxelDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ResizeToDimensions();
}
#endif

void UCMWVoxelDataAsset::ResizeToDimensions()
{
	Dimensions.X = FMath::Clamp(Dimensions.X, 1, 32);
	Dimensions.Y = FMath::Clamp(Dimensions.Y, 1, 32);
	Dimensions.Z = FMath::Clamp(Dimensions.Z, 1, 32);

	const int32 RequiredCount = Dimensions.X * Dimensions.Y * Dimensions.Z;
	if (Voxels.Num() != RequiredCount)
	{
		Voxels.SetNumZeroed(RequiredCount);
	}
}

bool UCMWVoxelDataAsset::IsFilled(int32 X, int32 Y, int32 Z) const
{
	const int32 Index = ToIndex(X, Y, Z);
	return Voxels.IsValidIndex(Index) && Voxels[Index].A > 0;
}

FColor UCMWVoxelDataAsset::GetVoxelColor(int32 X, int32 Y, int32 Z) const
{
	const int32 Index = ToIndex(X, Y, Z);
	return Voxels.IsValidIndex(Index) ? Voxels[Index] : FColor::Transparent;
}

int32 UCMWVoxelDataAsset::ToIndex(int32 X, int32 Y, int32 Z) const
{
	if (X < 0 || Y < 0 || Z < 0 || X >= Dimensions.X || Y >= Dimensions.Y || Z >= Dimensions.Z)
	{
		return INDEX_NONE;
	}

	return X + Dimensions.X * (Y + Dimensions.Y * Z);
}
