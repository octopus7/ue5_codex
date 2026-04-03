#pragma once

#include "GameFramework/Character.h"

#include "CHEnemyCharacter.generated.h"

class UCHHealthComponent;

UCLASS()
class CODEXHARNESS_API ACHEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACHEnemyCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE UCHHealthComponent* GetHealthComponent() const { return HealthComponent; }
	FORCEINLINE bool IsDead() const { return bIsDead; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UCHHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float ChaseMoveSpeed = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 140.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackCooldownSeconds = 0.75f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackDamage = 20.0f;

private:
	void UpdateChaseAndAttack(float DeltaTime);
	void HandleHealthDepleted(UCHHealthComponent* InHealthComponent);

	bool bEnemySmokeEnabled = false;
	bool bEnemySmokeChaseLogged = false;
	bool bEnemySmokeFinished = false;
	bool bIsDead = false;
	float LastAttackWorldTime = -1000.0f;
};
