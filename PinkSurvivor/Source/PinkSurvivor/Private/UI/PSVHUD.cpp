#include "UI/PSVHUD.h"

#include "Blueprint/UserWidget.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
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

void APSVHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!bIsRouletteVisible || !Canvas)
    {
        return;
    }

    const FVector2D ScreenSize(Canvas->SizeX, Canvas->SizeY);
    const FVector2D DisplayAnchor(ScreenSize.X * 0.5f, ScreenSize.Y * 0.35f);

    UFont* DisplayFont = GEngine ? GEngine->GetLargeFont() : nullptr;
    const float DisplayScale = 2.5f;
    float DisplayWidth = 0.f;
    float DisplayHeight = 0.f;
    GetTextSize(RouletteDisplayText, DisplayWidth, DisplayHeight, DisplayFont, DisplayScale);

    const float DisplayX = DisplayAnchor.X - (DisplayWidth * 0.5f);
    const float DisplayY = DisplayAnchor.Y - (DisplayHeight * 0.5f);
    DrawText(RouletteDisplayText, FLinearColor::Yellow, DisplayX, DisplayY, DisplayFont, DisplayScale, false);

    if (!RoulettePromptText.IsEmpty())
    {
        UFont* PromptFont = GEngine ? GEngine->GetMediumFont() : nullptr;
        const float PromptScale = 1.2f;
        float PromptWidth = 0.f;
        float PromptHeight = 0.f;
        GetTextSize(RoulettePromptText, PromptWidth, PromptHeight, PromptFont, PromptScale);

        const float PromptX = DisplayAnchor.X - (PromptWidth * 0.5f);
        const float PromptY = DisplayY + DisplayHeight + 20.f;
        DrawText(RoulettePromptText, FLinearColor::White, PromptX, PromptY, PromptFont, PromptScale, false);
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

void APSVHUD::ShowRouletteMessage(const FString& Text, const FString& PromptText)
{
    RouletteDisplayText = Text;
    RoulettePromptText = PromptText;
    bIsRouletteVisible = true;
}

void APSVHUD::ClearRouletteMessage()
{
    bIsRouletteVisible = false;
    RouletteDisplayText.Reset();
    RoulettePromptText.Reset();
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
