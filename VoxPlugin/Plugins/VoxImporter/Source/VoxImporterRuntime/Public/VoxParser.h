#pragma once

#include "Containers/ArrayView.h"
#include "CoreMinimal.h"
#include "VoxModel.h"

class VOXIMPORTERRUNTIME_API FVoxParser
{
public:
	static bool ParseBytes(TConstArrayView<uint8> Bytes, FVoxModelData& OutModel, FString& OutError);
	static bool ParseFile(const FString& Filename, FVoxModelData& OutModel, FString& OutError);
};
