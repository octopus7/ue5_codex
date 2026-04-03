#include "CodexHarnessHealthComponent.h"

#include "GameFramework/Actor.h"

UCodexHarnessHealthComponent::UCodexHarnessHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCodexHarnessHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	ResetHealthState();

	if (AActor* const OwnerActor = GetOwner())
	{
		OwnerActor->OnTakeAnyDamage.AddDynamic(this, &ThisClass::HandleOwnerTakeAnyDamage);
	}
}

float UCodexHarnessHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float UCodexHarnessHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UCodexHarnessHealthComponent::GetHealthFraction() const
{
	return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}

bool UCodexHarnessHealthComponent::IsAlive() const
{
	return !bIsDead && CurrentHealth > 0.0f;
}

bool UCodexHarnessHealthComponent::IsDead() const
{
	return !IsAlive();
}

void UCodexHarnessHealthComponent::SetMaxHealth(const float NewMaxHealth)
{
	MaxHealth = FMath::Max(NewMaxHealth, 1.0f);
}

FOnCodexHarnessHealthChanged& UCodexHarnessHealthComponent::OnHealthChanged()
{
	return HealthChangedEvent;
}

FOnCodexHarnessDeath& UCodexHarnessHealthComponent::OnDeath()
{
	return DeathEvent;
}

void UCodexHarnessHealthComponent::HandleOwnerTakeAnyDamage(
	AActor* DamagedActor,
	const float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser)
{
	static_cast<void>(DamagedActor);
	static_cast<void>(DamageType);
	static_cast<void>(InstigatedBy);
	static_cast<void>(DamageCauser);

	ApplyDamageInternal(Damage);
}

void UCodexHarnessHealthComponent::ResetHealthState()
{
	CurrentHealth = MaxHealth;
	bIsDead = false;
	HealthChangedEvent.Broadcast(CurrentHealth);
}

void UCodexHarnessHealthComponent::ApplyDamageInternal(const float Damage)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	HealthChangedEvent.Broadcast(CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		DeathEvent.Broadcast();
	}
}
