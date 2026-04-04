#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CMWVoxelDataAsset.generated.h"

UCLASS(BlueprintType)
class CODEXMAZEWALKER_API UCMWVoxelDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UCMWVoxelDataAsset();

	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void ResizeToDimensions();

	UFUNCTION(BlueprintPure, Category = "Voxel")
	bool IsFilled(int32 X, int32 Y, int32 Z) const;

	UFUNCTION(BlueprintPure, Category = "Voxel")
	FColor GetVoxelColor(int32 X, int32 Y, int32 Z) const;

	int32 ToIndex(int32 X, int32 Y, int32 Z) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Voxel")
	FIntVector Dimensions = FIntVector(32, 32, 32);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Voxel", meta = (ClampMin = "1.0"))
	float VoxelSize = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Voxel")
	FString SourceFilePath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Voxel")
	TArray<FColor> Voxels;
};
