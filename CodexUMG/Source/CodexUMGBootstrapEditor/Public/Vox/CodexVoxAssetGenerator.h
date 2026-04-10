#pragma once

#include "CoreMinimal.h"
#include "Vox/CodexVoxTypes.h"

class FCodexVoxAssetGenerator
{
public:
	static bool RunBuild(const CodexVox::FBuildOptions& Options, FString& OutError);
};
