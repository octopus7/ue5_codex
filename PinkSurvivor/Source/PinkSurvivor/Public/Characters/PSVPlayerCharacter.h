#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PSVPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UPSVAutoFireComponent;
class UPSVHealthComponent;
class UPSVExperienceComponent;
class APSVHUD;

UCLASS()
class PINKSURVIVOR_API APSVPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APSVPlayerCharacter();

    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable, Category="Progression")
    void HandlePersistentGoldChanged(int32 NewTotalGold);

protected:
    void Move(const FInputActionValue& Value);
    void InitializeHealth();
    void InitializeExperience();

    UFUNCTION()
    void HandleHealthChanged(float CurrentHealth, float MaxHealth);

    UFUNCTION()
    void HandleDeath();

    UFUNCTION()
    void HandleExperienceChanged(int32 CurrentExperience, int32 CurrentLevel);

    UFUNCTION()
    void HandleLevelUp(int32 NewLevel, int32 TotalExperience);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputMappingContext> DefaultInputMapping;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
    TObjectPtr<UPSVAutoFireComponent> AutoFireComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
    TObjectPtr<UPSVHealthComponent> HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Progression")
    TObjectPtr<UPSVExperienceComponent> ExperienceComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement")
    bool bAlignMovementToCameraYaw = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health", meta=(ClampMin="1.0"))
    float MaxHealth = 100.f;

    bool bIsDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="0.0"))
    float CameraBoomLength = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
    FRotator CameraRelativeRotation = FRotator(-55.0f, 0.0f, 0.0f);
};
