#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CodexHarnessEffectsConfigDataAsset.generated.h"

class UNiagaraSystem;

UCLASS(BlueprintType)
class CODEXHARNESS_API UCodexHarnessEffectsConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Damage")
	TObjectPtr<UNiagaraSystem> PlayerHitReactionSystem = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Damage")
	FVector PlayerHitReactionLocationOffset = FVector(0.0f, 0.0f, 40.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Damage")
	FVector PlayerHitReactionScale = FVector(0.35f, 0.35f, 0.35f);

	UFUNCTION(BlueprintPure, Category = "Effects|Damage")
	bool HasRequiredAssets() const;
};
