#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Input/CodexHarnessInputConfigDataAsset.h"
#include "CodexHarnessGameInstance.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class CODEXHARNESS_API UCodexHarnessGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UCodexHarnessInputConfigDataAsset> DefaultInputConfig = nullptr;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UCodexHarnessInputConfigDataAsset* GetInputConfig() const;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UInputMappingContext* GetInputMappingContext() const;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UInputAction* GetInputAction(ECodexHarnessConfiguredInputAction InAction) const;
};
