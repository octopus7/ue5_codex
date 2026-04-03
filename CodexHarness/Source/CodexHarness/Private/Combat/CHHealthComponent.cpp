#include "Combat/CHHealthComponent.h"

UCHHealthComponent::UCHHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCHHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	ResetToMaxHealth();
}

bool UCHHealthComponent::ApplyDamage(const float DamageAmount, float& OutAppliedDamage)
{
	OutAppliedDamage = 0.0f;

	if (bIsDead || DamageAmount <= 0.0f)
	{
		return false;
	}

	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	OutAppliedDamage = OldHealth - CurrentHealth;
	if (OutAppliedDamage <= 0.0f)
	{
		return false;
	}

	OnHealthChanged.Broadcast(this, OldHealth, CurrentHealth);
	if (CurrentHealth <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast(this);
	}

	return true;
}

void UCHHealthComponent::ResetToMaxHealth()
{
	bIsDead = false;
	CurrentHealth = MaxHealth;
}
