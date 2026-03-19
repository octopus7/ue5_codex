#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CodexInvenMediumGameInstance.generated.h"

class UCodexInvenMediumInputConfigDataAsset;

UCLASS(BlueprintType)
class CODEXINVENMEDIUM_API UCodexInvenMediumGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UCodexInvenMediumInputConfigDataAsset> DefaultInputConfig = nullptr;
};
