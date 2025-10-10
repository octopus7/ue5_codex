#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PSVEnemyCharacter.generated.h"

class UPSVHealthComponent;
class APSVExperienceGem;
class APSVGoldCoin;

UCLASS()
class PINKSURVIVOR_API APSVEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APSVEnemyCharacter();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    void UpdateMovement(float DeltaSeconds);
    void TryAttack();

    void StartAttackLoop();
    void StopAttackLoop();
    void PerformAttack();

    UFUNCTION()
    void HandleDeath();

    void SpawnExperienceGem();
    void TrySpawnGoldCoin();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UPSVHealthComponent> HealthComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health", meta=(ClampMin="1.0"))
    float MaxHealth = 40.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0.0"))
    float MoveSpeed = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin="0.0"))
    float AttackRange = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin="0.1"))
    float AttackInterval = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin="0.0"))
    float AttackDamage = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin="0.0"))
    float AttackRadiusTolerance = 50.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Experience")
    TArray<TSubclassOf<APSVExperienceGem>> ExperienceGemClasses;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Experience", meta=(ClampMin="0.0"))
    float GemSpawnOffsetZ = 30.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Currency")
    TSubclassOf<APSVGoldCoin> GoldCoinClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Currency", meta=(ClampMin="0.0", ClampMax="1.0"))
    float GoldCoinDropChance = 0.05f;

    bool bIsDead = false;
    bool bIsAttacking = false;

    TWeakObjectPtr<APawn> TargetPawn;

    FTimerHandle AttackTimerHandle;
};
