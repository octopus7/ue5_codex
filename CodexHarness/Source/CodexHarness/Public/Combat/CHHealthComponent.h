#pragma once

#include "Components/ActorComponent.h"

#include "CHHealthComponent.generated.h"

class UCHHealthComponent;

DECLARE_MULTICAST_DELEGATE_ThreeParams(FCHHealthChangedSignature, UCHHealthComponent*, float, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FCHDeathSignature, UCHHealthComponent*);

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class CODEXHARNESS_API UCHHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCHHealthComponent();

	virtual void BeginPlay() override;

	bool ApplyDamage(float DamageAmount, float& OutAppliedDamage);
	void ResetToMaxHealth();

	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE bool IsDead() const { return bIsDead; }

	FCHHealthChangedSignature OnHealthChanged;
	FCHDeathSignature OnDeath;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;
};
