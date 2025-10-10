#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PSVChestRouletteSubsystem.generated.h"

class APSVPlayerCharacter;
class APSVHUD;

/**
 * Minimal roulette presentation subsystem triggered by upgrade chests.
 * Pauses the game, cycles through candidate texts, and waits for the player's confirm input.
 */
UCLASS()
class PINKSURVIVOR_API UPSVChestRouletteSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UPSVChestRouletteSubsystem();

    UFUNCTION(BlueprintCallable, Category="Roulette")
    bool StartRoulette(APSVPlayerCharacter* Player, float InDuration);

    void AdvanceRoulette();
    void FinalizeResult();
    void HandleConfirm(APSVPlayerCharacter* Player);

    UFUNCTION(BlueprintPure, Category="Roulette")
    bool IsRouletteActive() const { return bIsRouletteActive; }

protected:
    APSVHUD* ResolveHUD(APSVPlayerCharacter* Player) const;
    void UpdateHUDDisplay(const FString& DisplayText, const FString& PromptText);
    void ClearState();

    UPROPERTY(EditDefaultsOnly, Category="Roulette")
    TArray<FString> CandidateTexts;

    UPROPERTY(EditDefaultsOnly, Category="Roulette")
    float RouletteInterval;

    UPROPERTY(EditDefaultsOnly, Category="Roulette")
    float DefaultRouletteDuration;

    FTimerHandle RouletteTimerHandle;
    FTimerHandle FinalizeTimerHandle;

    FString CurrentDisplay;

    TWeakObjectPtr<APSVPlayerCharacter> ActivePlayer;

    bool bIsRouletteActive = false;
    bool bAwaitingConfirm = false;
};
