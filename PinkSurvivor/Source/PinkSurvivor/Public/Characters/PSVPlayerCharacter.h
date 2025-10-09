#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PSVPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class PINKSURVIVOR_API APSVPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APSVPlayerCharacter();

    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    void Move(const FInputActionValue& Value);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UCameraComponent> FollowCamera;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputMappingContext> DefaultInputMapping;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera", meta=(ClampMin="0.0"))
    float CameraBoomLength = 800.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera")
    FRotator CameraRelativeRotation = FRotator(-55.0f, 0.0f, 0.0f);
};
