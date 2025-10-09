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
};
