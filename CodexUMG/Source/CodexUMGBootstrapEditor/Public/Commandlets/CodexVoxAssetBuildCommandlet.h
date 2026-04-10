#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexVoxAssetBuildCommandlet.generated.h"

UCLASS()
class CODEXUMGBOOTSTRAPEDITOR_API UCodexVoxAssetBuildCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexVoxAssetBuildCommandlet();

	virtual int32 Main(const FString& Params) override;
};
