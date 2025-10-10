#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PSVPlayerHUDWidget.generated.h"

UCLASS()
class PINKSURVIVOR_API UPSVPlayerHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category="HUD")
    void OnHealthChanged(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, Category="HUD")
    void OnPlayerDied();

    UFUNCTION(BlueprintImplementableEvent, Category="HUD")
    void OnExperienceChanged(int32 CurrentExperience, int32 CurrentLevel, int32 ExperienceToNextLevel);

    UFUNCTION(BlueprintImplementableEvent, Category="HUD")
    void OnLevelUp(int32 NewLevel, int32 TotalExperience);

    UFUNCTION(BlueprintImplementableEvent, Category="HUD")
    void OnPersistentGoldChanged(int32 TotalGold);
};
