#include "UI/PSVHUD.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PSVPlayerHUDWidget.h"

APSVHUD::APSVHUD()
{
}

void APSVHUD::BeginPlay()
{
    Super::BeginPlay();

    if (PlayerHUDWidgetClass)
    {
        PlayerHUDWidget = CreateWidget<UPSVPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
        if (PlayerHUDWidget)
        {
            PlayerHUDWidget->AddToViewport();

            if (bHasCachedHealth)
            {
                PlayerHUDWidget->OnHealthChanged(CachedHealth, CachedMaxHealth);
            }

            if (bHasCachedExperience)
            {
                PlayerHUDWidget->OnExperienceChanged(CachedExperience, CachedLevel, CachedExperienceToNextLevel);
            }

            if (bHasCachedPersistentGold)
            {
                PlayerHUDWidget->OnPersistentGoldChanged(CachedPersistentGold);
            }
        }
    }
}

void APSVHUD::HandlePlayerHealthChanged(float CurrentHealth, float MaxHealth)
{
    CachedHealth = CurrentHealth;
    CachedMaxHealth = MaxHealth;
    bHasCachedHealth = true;

    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->OnHealthChanged(CurrentHealth, MaxHealth);
    }
}

void APSVHUD::HandlePlayerDeath()
{
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->OnPlayerDied();
    }

    if (!GameOverWidget && GameOverWidgetClass)
    {
        GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
    }

    if (GameOverWidget && !GameOverWidget->IsInViewport())
    {
        GameOverWidget->AddToViewport();
    }

    if (APlayerController* PC = GetOwningPlayerController())
    {
        PC->SetPause(true);
        PC->bShowMouseCursor = true;
    }
}

void APSVHUD::HandlePlayerExperienceChanged(int32 CurrentExperience, int32 CurrentLevel, int32 ExperienceToNextLevel)
{
    CachedExperience = CurrentExperience;
    CachedLevel = CurrentLevel;
    CachedExperienceToNextLevel = ExperienceToNextLevel;
    bHasCachedExperience = true;

    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->OnExperienceChanged(CurrentExperience, CurrentLevel, ExperienceToNextLevel);
    }
}

void APSVHUD::HandlePlayerLevelUp(int32 NewLevel, int32 TotalExperience)
{
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->OnLevelUp(NewLevel, TotalExperience);
    }
}

void APSVHUD::HandlePlayerGoldChanged(int32 TotalGold)
{
    CachedPersistentGold = TotalGold;
    bHasCachedPersistentGold = true;

    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->OnPersistentGoldChanged(TotalGold);
    }
}
