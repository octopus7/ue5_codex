#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PSVExperienceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPSVOnExperienceChangedSignature, int32, CurrentExperience, int32, CurrentLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPSVOnLevelUpSignature, int32, NewLevel, int32, TotalExperience);

/**
 * Component that stores the player's run-based experience and level state.
 * Blueprint authors can tune level requirements through the exposed thresholds array.
 */
UCLASS(ClassGroup=(Progression), meta=(BlueprintSpawnableComponent))
class PINKSURVIVOR_API UPSVExperienceComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPSVExperienceComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Experience")
    void GainExperience(int32 Amount);

    UFUNCTION(BlueprintCallable, Category="Experience")
    void ResetProgression(bool bBroadcastChange = true);

    UFUNCTION(BlueprintCallable, Category="Experience")
    int32 GetCurrentLevel() const { return CurrentLevel; }

    UFUNCTION(BlueprintCallable, Category="Experience")
    int32 GetCurrentExperience() const { return CurrentExperience; }

    UFUNCTION(BlueprintCallable, Category="Experience")
    int32 GetExperienceForLevel(int32 Level) const;

    UFUNCTION(BlueprintCallable, Category="Experience")
    int32 GetNextLevelThreshold() const;

    UFUNCTION(BlueprintCallable, Category="Experience")
    int32 GetExperienceToNextLevel() const;

    UPROPERTY(BlueprintAssignable, Category="Experience")
    FPSVOnExperienceChangedSignature OnExperienceChanged;

    UPROPERTY(BlueprintAssignable, Category="Experience")
    FPSVOnLevelUpSignature OnLevelUp;

protected:
    bool EvaluateLevelUps();
    void BroadcastExperienceChanged();

    /** 누적 경험치 기준으로 각 레벨 달성 요구치 (인덱스 0 -> 레벨 2 필요 XP). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Experience")
    TArray<int32> ExperienceThresholds;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Experience")
    int32 CurrentLevel;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Experience")
    int32 CurrentExperience;
};
