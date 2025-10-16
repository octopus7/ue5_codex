#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "StaticMeshCharacter.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAmmoHealthWidget;
class USpringArmComponent;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, NewAmmo, int32, MaxAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHealth, float, MaxHealth);

/**
 * Pawn representing a player avatar driven via Enhanced Input.
 * Holds a configurable static mesh, tracks ammo/health, and exposes delegates for UI binding.
 */
UCLASS()
class CODEXUISIMPLE_API AStaticMeshCharacter : public APawn
{
    GENERATED_BODY()

public:
    AStaticMeshCharacter();

    // APawn interface
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_Controller() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** Current ammo count available for firing. */
    UFUNCTION(BlueprintPure, Category = "Status")
    int32 GetCurrentAmmo() const { return CurrentAmmo; }

    /** Max ammo cap. */
    UFUNCTION(BlueprintPure, Category = "Status")
    int32 GetMaxAmmo() const { return MaxAmmo; }

    /** Current health value. */
    UFUNCTION(BlueprintPure, Category = "Status")
    float GetCurrentHealth() const { return CurrentHealth; }

    /** Maximum health cap. */
    UFUNCTION(BlueprintPure, Category = "Status")
    float GetMaxHealth() const { return MaxHealth; }

    /** Allows designers to replenish ammo (e.g. pickups). */
    UFUNCTION(BlueprintCallable, Category = "Status")
    void AddAmmo(int32 AmmoAmount);

    /** Allows designers to heal the character (e.g. pickups). */
    UFUNCTION(BlueprintCallable, Category = "Status")
    void AddHealth(float HealthAmount);

    /** Broadcast when ammo changes so UI/widgets can react. */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAmmoChanged OnAmmoChanged;

    /** Broadcast when health changes so UI/widgets can react. */
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnHealthChanged OnHealthChanged;

protected:
    /** Capsule used for collision and as the root component. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCapsuleComponent* CapsuleComponent;

    /** Static mesh that artists/designers can swap out per-instance. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CharacterMesh;

    /** Spring arm used to position the player camera. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* CameraBoom;

    /** Player-facing camera attached to the spring arm. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* FollowCamera;

    /** Default mapping context pushed to the local player at begin-play. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    /** Input action bound to firing. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* FireAction;

    /** Input action bound to breathing. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* BreatheAction;

    /** Widget blueprint class instantiated to display player status info. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UAmmoHealthWidget> StatusWidgetClass;

    /** Cost in ammo units each time the fire input is triggered. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (ClampMin = "0"))
    int32 AmmoCostPerFire;

    /** Cost in health units each time the breathe input is triggered. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (ClampMin = "0.0"))
    float HealthCostPerBreath;

    /** Maximum ammo available; can be tuned per-instance. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status", meta = (ClampMin = "0"))
    int32 MaxAmmo;

    /** Maximum health available; can be tuned per-instance. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status", meta = (ClampMin = "0.0"))
    float MaxHealth;

private:
    void HandleFireInput(const FInputActionValue& Value);
    void HandleBreatheInput(const FInputActionValue& Value);

    void ConsumeAmmo(int32 Amount);
    void ConsumeHealth(float Amount);

    void BroadcastAmmoChanged();
    void BroadcastHealthChanged();
    void CreateStatusWidget();

    /** Tracked ammo value updated when firing/reloading. */
    UPROPERTY(VisibleInstanceOnly, Category = "Status", meta = (ClampMin = "0"))
    int32 CurrentAmmo;

    /** Tracked health value updated when breathing or healing. */
    UPROPERTY(VisibleInstanceOnly, Category = "Status", meta = (ClampMin = "0.0"))
    float CurrentHealth;

    /** HUD widget that mirrors ammo/health stats. */
    UPROPERTY(Transient)
    UAmmoHealthWidget* StatusWidget;

    /** Tracks whether we've already warned about a missing widget class to avoid log spam. */
    bool bHasWarnedMissingWidgetClass;
};
