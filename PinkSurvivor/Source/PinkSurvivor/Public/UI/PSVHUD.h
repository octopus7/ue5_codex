#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PSVHUD.generated.h"

class UPSVPlayerHUDWidget;
class UUserWidget;

UCLASS()
class PINKSURVIVOR_API APSVHUD : public AHUD
{
    GENERATED_BODY()

public:
    APSVHUD();

    virtual void BeginPlay() override;

    void HandlePlayerHealthChanged(float CurrentHealth, float MaxHealth);
    void HandlePlayerDeath();
    void HandlePlayerExperienceChanged(int32 CurrentExperience, int32 CurrentLevel, int32 ExperienceToNextLevel);
    void HandlePlayerLevelUp(int32 NewLevel, int32 TotalExperience);
    void HandlePlayerGoldChanged(int32 TotalGold);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HUD")
    TSubclassOf<UPSVPlayerHUDWidget> PlayerHUDWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="HUD")
    TSubclassOf<UUserWidget> GameOverWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UPSVPlayerHUDWidget> PlayerHUDWidget;

    UPROPERTY()
    TObjectPtr<UUserWidget> GameOverWidget;

    float CachedHealth = 0.f;
    float CachedMaxHealth = 1.f;
    bool bHasCachedHealth = false;

    int32 CachedExperience = 0;
    int32 CachedLevel = 1;
    int32 CachedExperienceToNextLevel = 0;
    bool bHasCachedExperience = false;

    int32 CachedPersistentGold = 0;
    bool bHasCachedPersistentGold = false;
};
