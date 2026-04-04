#pragma once

#include "CoreMinimal.h"

struct FCMWVoxModelData
{
	FIntVector Dimensions = FIntVector::ZeroValue;
	TArray<FColor> Voxels;
};

class FCMWVoxParser
{
public:
	static bool ParseFile(const FString& SourceFilePath, FCMWVoxModelData& OutModelData, FString& OutError);
};
