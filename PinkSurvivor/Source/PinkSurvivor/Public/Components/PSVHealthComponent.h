#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PSVHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPSVOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPSVOnDeathSignature);

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class PINKSURVIVOR_API UPSVHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPSVHealthComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="Health")
    void InitializeHealth(float NewMaxHealth, bool bResetCurrent = true);

    UFUNCTION(BlueprintCallable, Category="Health")
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintCallable, Category="Health")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintCallable, Category="Health")
    float GetHealthPercent() const;

    UFUNCTION(BlueprintCallable, Category="Health")
    bool IsDead() const { return CurrentHealth <= 0.f; }

    float ApplyDamage(float DamageAmount);
    float Heal(float HealAmount);

    UPROPERTY(BlueprintAssignable, Category="Health")
    FPSVOnHealthChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category="Health")
    FPSVOnDeathSignature OnDeath;

protected:
    void ClampHealth();

    UFUNCTION()
    void HandleOwnerTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health", meta=(ClampMin="1.0"))
    float MaxHealth = 100.f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Health")
    float CurrentHealth = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health")
    bool bAutoRegisterForDamage = false;
};
