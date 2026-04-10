#pragma once

#include "CoreMinimal.h"
#include "Vox/CodexVoxTypes.h"

class FCodexVoxAssetGenerator
{
public:
	static bool Run(const FVoxAssetBuildSettings& Settings, FVoxAssetBuildResult& OutResult, FString& OutError);
};
