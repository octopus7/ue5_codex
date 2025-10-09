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
