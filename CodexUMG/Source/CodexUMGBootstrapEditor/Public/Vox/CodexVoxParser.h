#pragma once

#include "CoreMinimal.h"
#include "Vox/CodexVoxTypes.h"

class FCodexVoxParser
{
public:
	static bool LoadModelFromFile(const FString& FilePath, CodexVox::FParsedModel& OutModel, FString& OutError);
};
