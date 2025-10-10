#include "Core/PSVChestRouletteSubsystem.h"

#include "Characters/PSVPlayerCharacter.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/PSVHUD.h"

UPSVChestRouletteSubsystem::UPSVChestRouletteSubsystem()
{
    CandidateTexts = {TEXT("A"), TEXT("B"), TEXT("C"), TEXT("D"), TEXT("E")};
    RouletteInterval = 0.1f;
    DefaultRouletteDuration = 2.0f;
}

bool UPSVChestRouletteSubsystem::StartRoulette(APSVPlayerCharacter* Player, float InDuration)
{
    if (bIsRouletteActive || !Player)
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }

    if (CandidateTexts.Num() == 0)
    {
        CandidateTexts = {TEXT("A"), TEXT("B"), TEXT("C"), TEXT("D"), TEXT("E")};
    }

    ActivePlayer = Player;
    bIsRouletteActive = true;
    bAwaitingConfirm = false;

    const float ActiveDuration = InDuration > 0.f ? InDuration : DefaultRouletteDuration;
    ActiveRouletteDuration = ActiveDuration;
    RouletteElapsedTime = 0.f;
    RouletteIntervalAccumulator = 0.f;

    UGameplayStatics::SetGamePaused(World, true);

    // Advance once immediately to seed the display
    AdvanceRoulette();

    StartRouletteTicker();

    return true;
}

void UPSVChestRouletteSubsystem::StartRouletteTicker()
{
    StopRouletteTicker();

    RouletteTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateUObject(this, &UPSVChestRouletteSubsystem::TickRoulette));
}

void UPSVChestRouletteSubsystem::StopRouletteTicker()
{
    if (RouletteTickerHandle.IsValid())
    {
        FTSTicker::GetCoreTicker().RemoveTicker(RouletteTickerHandle);
        RouletteTickerHandle = FTSTicker::FDelegateHandle();
    }
}

bool UPSVChestRouletteSubsystem::TickRoulette(float DeltaTime)
{
    if (!bIsRouletteActive)
    {
        return false;
    }

    RouletteElapsedTime += DeltaTime;
    RouletteIntervalAccumulator += DeltaTime;

    if (RouletteInterval <= KINDA_SMALL_NUMBER)
    {
        AdvanceRoulette();
    }
    else
    {
        while (RouletteIntervalAccumulator >= RouletteInterval)
        {
            RouletteIntervalAccumulator -= RouletteInterval;
            AdvanceRoulette();
        }
    }

    if (RouletteElapsedTime >= ActiveRouletteDuration)
    {
        FinalizeResult();
        return false;
    }

    return true;
}

void UPSVChestRouletteSubsystem::AdvanceRoulette()
{
    if (!bIsRouletteActive || CandidateTexts.Num() == 0)
    {
        return;
    }

    const int32 RandomIndex = FMath::RandRange(0, CandidateTexts.Num() - 1);
    CurrentDisplay = CandidateTexts[RandomIndex];

    UpdateHUDDisplay(CurrentDisplay, FString());
}

void UPSVChestRouletteSubsystem::FinalizeResult()
{
    if (!bIsRouletteActive)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        ClearState();
        return;
    }

    StopRouletteTicker();

    bAwaitingConfirm = true;

    static const FString ConfirmPromptText = TEXT("Press X / OK to confirm");
    UpdateHUDDisplay(CurrentDisplay, ConfirmPromptText);
}

void UPSVChestRouletteSubsystem::HandleConfirm(APSVPlayerCharacter* Player)
{
    if (!bIsRouletteActive || !bAwaitingConfirm)
    {
        return;
    }

    if (!ActivePlayer.IsValid() || ActivePlayer.Get() != Player)
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        UGameplayStatics::SetGamePaused(World, false);
    }

    UpdateHUDDisplay(FString(), FString());
    ClearState();
}

APSVHUD* UPSVChestRouletteSubsystem::ResolveHUD(APSVPlayerCharacter* Player) const
{
    if (!Player)
    {
        return nullptr;
    }

    if (APlayerController* PlayerController = Cast<APlayerController>(Player->GetController()))
    {
        return PlayerController->GetHUD<APSVHUD>();
    }

    return nullptr;
}

void UPSVChestRouletteSubsystem::UpdateHUDDisplay(const FString& DisplayText, const FString& PromptText)
{
    APSVHUD* HUD = ActivePlayer.IsValid() ? ResolveHUD(ActivePlayer.Get()) : nullptr;

    if (!HUD)
    {
        return;
    }

    if (DisplayText.IsEmpty() && PromptText.IsEmpty())
    {
        HUD->ClearRouletteMessage();
    }
    else
    {
        HUD->ShowRouletteMessage(DisplayText, PromptText);
    }
}

void UPSVChestRouletteSubsystem::ClearState()
{
    bIsRouletteActive = false;
    bAwaitingConfirm = false;
    ActivePlayer.Reset();
    StopRouletteTicker();

    ActiveRouletteDuration = 0.f;
    RouletteElapsedTime = 0.f;
    RouletteIntervalAccumulator = 0.f;
}
