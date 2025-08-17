#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AEnemyConeCharacter;

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

protected:
    // Sensing/Combat params
    UPROPERTY(EditAnywhere, Category="Sensing")
    float SightRadius = 1500.f;

    UPROPERTY(EditAnywhere, Category="Sensing")
    float LoseSightRadius = 2000.f;

    UPROPERTY(EditAnywhere, Category="Sensing")
    float FOVDegrees = 90.f;

    UPROPERTY(EditAnywhere, Category="Combat")
    float AttackRange = 150.f;

    UPROPERTY(EditAnywhere, Category="Patrol")
    float PatrolRadius = 1000.f;

    UPROPERTY(EditAnywhere, Category="Patrol")
    float IdleTimeMin = 1.5f;

    UPROPERTY(EditAnywhere, Category="Patrol")
    float IdleTimeMax = 3.0f;

    UPROPERTY(EditAnywhere, Category="Debug")
    bool bDrawDebug = false;

private:
    TWeakObjectPtr<AEnemyConeCharacter> CachedEnemy;
    TWeakObjectPtr<APawn> CachedPlayer;

    EEnemyState State = EEnemyState::Idle;

    FTimerHandle ThinkTimerHandle;
    FTimerHandle IdleTimerHandle;

    void Think();
    void StartIdle(float Duration);
    void StartPatrol();
    void StartChase();

    bool CanSeePlayer(bool bStrict = true) const;
    bool IsPlayerInAttackRange() const;

};
