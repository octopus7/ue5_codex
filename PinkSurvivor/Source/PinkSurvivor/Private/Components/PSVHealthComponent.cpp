#include "Components/PSVHealthComponent.h"

#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UPSVHealthComponent::UPSVHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPSVHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;

    if (bAutoRegisterForDamage)
    {
        if (AActor* OwnerActor = GetOwner())
        {
            OwnerActor->OnTakeAnyDamage.AddDynamic(this, &UPSVHealthComponent::HandleOwnerTakeAnyDamage);
        }
    }
}

void UPSVHealthComponent::InitializeHealth(float NewMaxHealth, bool bResetCurrent)
{
    MaxHealth = FMath::Max(1.f, NewMaxHealth);
    if (bResetCurrent || CurrentHealth > MaxHealth)
    {
        CurrentHealth = MaxHealth;
    }

    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

float UPSVHealthComponent::GetHealthPercent() const
{
    return MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f;
}

float UPSVHealthComponent::ApplyDamage(float DamageAmount)
{
    if (DamageAmount <= 0.f || IsDead())
    {
        return 0.f;
    }

    const float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.f, MaxHealth);

    if (!FMath::IsNearlyEqual(CurrentHealth, PreviousHealth))
    {
        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

        if (IsDead())
        {
            OnDeath.Broadcast();
        }
    }

    return PreviousHealth - CurrentHealth;
}

float UPSVHealthComponent::Heal(float HealAmount)
{
    if (HealAmount <= 0.f || IsDead())
    {
        return 0.f;
    }

    const float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, MaxHealth);

    if (!FMath::IsNearlyEqual(CurrentHealth, PreviousHealth))
    {
        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    }

    return CurrentHealth - PreviousHealth;
}

void UPSVHealthComponent::ClampHealth()
{
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);
}

void UPSVHealthComponent::HandleOwnerTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    ApplyDamage(Damage);
}
