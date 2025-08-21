#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AEnemyConeCharacter;
class AWaveProjectile;

UENUM()
enum class EEnemyState : uint8
{
    Idle,
    Patrol,
    Chase
};

UCLASS()
class CODEXWAVE_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController();

    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

    EEnemyState GetState() const { return State; }
    UFUNCTION(BlueprintPure, Category="AI")
    float GetIdleTimeRemaining() const;

protected:
    // Sensing/Combat params
    UPROPERTY(EditAnywhere, Category="Sensing")
    float SightRadius = 1500.f;

    UPROPERTY(EditAnywhere, Category="Sensing")
    float LoseSightRadius = 2000.f;

    UPROPERTY(EditAnywhere, Category="Sensing")
    float FOVDegrees = 90.f;

    UPROPERTY(EditAnywhere, Category="Sensing")
    bool bUseFOV = true;

    UPROPERTY(EditAnywhere, Category="Sensing")
    bool bUseLineOfSight = true;

    UPROPERTY(EditAnywhere, Category="Sensing")
    float SightHeightOffset = 50.f;

    UPROPERTY(EditAnywhere, Category="Combat")
    float AttackRange = 150.f;

    UPROPERTY(EditAnywhere, Category="Combat")
    float AttackCooldown = 1.0f;

    UPROPERTY(EditAnywhere, Category="Combat")
    float ProjectileSpawnOffset = 60.f;

    UPROPERTY(EditDefaultsOnly, Category="Combat")
    TSubclassOf<AWaveProjectile> AttackProjectileClass;

    UPROPERTY(EditAnywhere, Category="Patrol")
    float PatrolRadius = 1000.f;

    UPROPERTY(EditAnywhere, Category="Patrol")
    float IdleTimeMin = 1.5f;

    UPROPERTY(EditAnywhere, Category="Patrol")
    float IdleTimeMax = 3.0f;

    UPROPERTY(EditAnywhere, Category="Debug")
    bool bDrawDebug = false;

    UPROPERTY(EditAnywhere, Category="Debug")
    float ThinkInterval = 0.2f;

private:
    TWeakObjectPtr<AEnemyConeCharacter> CachedEnemy;
    TWeakObjectPtr<APawn> CachedPlayer;

    EEnemyState State = EEnemyState::Idle;
    bool bInitialChasePending = true;

    FTimerHandle ThinkTimerHandle;
    FTimerHandle IdleTimerHandle;
    float LastAttackTime = -10000.f;

    void Think();
    void StartIdle(float Duration);
    void StartPatrol();
    void StartChase();
    void TryAttack();

    bool CanSeePlayer(bool bStrict = true) const;
    bool IsPlayerInAttackRange() const;

    void DrawFOVDebug(float Radius, const FColor& Color) const;

    void AcquirePlayer();

};
