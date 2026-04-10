#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexBasicMapFloorBuildCommandlet.generated.h"

UCLASS()
class CODEXUMGBOOTSTRAPEDITOR_API UCodexBasicMapFloorBuildCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexBasicMapFloorBuildCommandlet();

	virtual int32 Main(const FString& Params) override;
};
