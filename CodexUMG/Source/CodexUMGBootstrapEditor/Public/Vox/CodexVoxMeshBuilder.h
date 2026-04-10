#pragma once

#include "CoreMinimal.h"
#include "Vox/CodexVoxTypes.h"

class FCodexVoxMeshBuilder
{
public:
	static bool BuildMeshDescription(const CodexVox::FMeshBuildInput& Input, CodexVox::FMeshBuildOutput& OutBuildOutput, FString& OutError);
};
