#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CodexHarnessHealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCodexHarnessHealthChanged, float);
DECLARE_MULTICAST_DELEGATE(FOnCodexHarnessDeath);

UCLASS(ClassGroup = (CodexHarness), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class CODEXHARNESS_API UCodexHarnessHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCodexHarnessHealthComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthFraction() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;

	void SetMaxHealth(float NewMaxHealth);

	FOnCodexHarnessHealthChanged& OnHealthChanged();
	FOnCodexHarnessDeath& OnDeath();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

private:
	UFUNCTION()
	void HandleOwnerTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void ResetHealthState();
	void ApplyDamageInternal(float Damage);

	FOnCodexHarnessHealthChanged HealthChangedEvent;
	FOnCodexHarnessDeath DeathEvent;
};
