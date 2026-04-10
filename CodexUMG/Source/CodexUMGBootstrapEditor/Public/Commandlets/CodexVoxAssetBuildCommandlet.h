#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "CodexVoxAssetBuildCommandlet.generated.h"

UCLASS()
class CODEXUMGBOOTSTRAPEDITOR_API UCodexVoxAssetBuildCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexVoxAssetBuildCommandlet(const FObjectInitializer& ObjectInitializer);

	virtual int32 Main(const FString& Params) override;
};
