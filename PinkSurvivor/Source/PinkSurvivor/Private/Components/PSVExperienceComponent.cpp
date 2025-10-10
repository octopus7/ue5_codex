#include "Components/PSVExperienceComponent.h"

UPSVExperienceComponent::UPSVExperienceComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    ExperienceThresholds = { 10, 25, 45, 70 };
    CurrentLevel = 1;
    CurrentExperience = 0;
}

void UPSVExperienceComponent::BeginPlay()
{
    Super::BeginPlay();

    ExperienceThresholds.Sort();
    BroadcastExperienceChanged();
}

void UPSVExperienceComponent::GainExperience(int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    CurrentExperience = FMath::Max(0, CurrentExperience + Amount);

    const bool bLeveledUp = EvaluateLevelUps();

    if (!bLeveledUp)
    {
        BroadcastExperienceChanged();
    }
}

void UPSVExperienceComponent::ResetProgression(bool bBroadcastChange)
{
    CurrentLevel = 1;
    CurrentExperience = 0;

    if (bBroadcastChange)
    {
        BroadcastExperienceChanged();
    }
}

int32 UPSVExperienceComponent::GetExperienceForLevel(int32 Level) const
{
    if (Level <= 1)
    {
        return 0;
    }

    const int32 Index = Level - 2;
    if (ExperienceThresholds.IsValidIndex(Index))
    {
        return ExperienceThresholds[Index];
    }

    return ExperienceThresholds.Num() > 0 ? ExperienceThresholds.Last() : 0;
}

int32 UPSVExperienceComponent::GetNextLevelThreshold() const
{
    const int32 Index = CurrentLevel - 1;
    if (ExperienceThresholds.IsValidIndex(Index))
    {
        return ExperienceThresholds[Index];
    }
    return INDEX_NONE;
}

int32 UPSVExperienceComponent::GetExperienceToNextLevel() const
{
    const int32 Threshold = GetNextLevelThreshold();
    if (Threshold == INDEX_NONE)
    {
        return 0;
    }

    return FMath::Max(0, Threshold - CurrentExperience);
}

bool UPSVExperienceComponent::EvaluateLevelUps()
{
    bool bHasLevelled = false;

    while (true)
    {
        const int32 Threshold = GetNextLevelThreshold();
        if (Threshold == INDEX_NONE || CurrentExperience < Threshold)
        {
            break;
        }

        ++CurrentLevel;
        bHasLevelled = true;
        OnLevelUp.Broadcast(CurrentLevel, CurrentExperience);
    }

    if (bHasLevelled)
    {
        BroadcastExperienceChanged();
    }

    return bHasLevelled;
}

void UPSVExperienceComponent::BroadcastExperienceChanged()
{
    OnExperienceChanged.Broadcast(CurrentExperience, CurrentLevel);
}
