#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Effects/CodexHarnessEffectsConfigDataAsset.h"
#include "Input/CodexHarnessInputConfigDataAsset.h"
#include "CodexHarnessGameInstance.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraShakeBase;
class UNiagaraSystem;

UCLASS()
class CODEXHARNESS_API UCodexHarnessGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UCodexHarnessInputConfigDataAsset> DefaultInputConfig = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UCodexHarnessEffectsConfigDataAsset> DefaultEffectsConfig = nullptr;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UCodexHarnessInputConfigDataAsset* GetInputConfig() const;

	UFUNCTION(BlueprintPure, Category = "Effects")
	const UCodexHarnessEffectsConfigDataAsset* GetEffectsConfig() const;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UInputMappingContext* GetInputMappingContext() const;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UInputAction* GetInputAction(ECodexHarnessConfiguredInputAction InAction) const;

	UFUNCTION(BlueprintPure, Category = "Effects")
	const UNiagaraSystem* GetPlayerHitReactionSystem() const;

	UFUNCTION(BlueprintPure, Category = "Effects")
	TSubclassOf<UCameraShakeBase> GetPlayerHitCameraShakeClass() const;

	UFUNCTION(BlueprintPure, Category = "Effects")
	float GetPlayerHitCameraShakeScale() const;
};
