#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexInteractionAssetBuildCommandlet.generated.h"

UCLASS()
class CODEXUMGBOOTSTRAPEDITOR_API UCodexInteractionAssetBuildCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexInteractionAssetBuildCommandlet();

	virtual int32 Main(const FString& Params) override;
};
