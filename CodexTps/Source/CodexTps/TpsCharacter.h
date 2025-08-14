#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TpsCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class UInputAction;
class UInputMappingContext;

UCLASS()
class CODEXTPS_API ATpsCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATpsCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void MoveForward(const FInputActionValue& Value);
    void MoveRight(const FInputActionValue& Value);
    void LookYaw(const FInputActionValue& Value);
    void LookPitch(const FInputActionValue& Value);
    void JumpStarted(const FInputActionValue& Value);
    void JumpCompleted(const FInputActionValue& Value);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
    UStaticMeshComponent* CubeMesh;

    UPROPERTY(EditAnywhere, Category = "Input")
    float LookSensitivityYaw = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Input")
    float LookSensitivityPitch = 1.0f;

    // Enhanced Input runtime-created assets
    UPROPERTY()
    UInputAction* IA_MoveForward;

    UPROPERTY()
    UInputAction* IA_MoveRight;

    UPROPERTY()
    UInputAction* IA_LookYaw;

    UPROPERTY()
    UInputAction* IA_LookPitch;

    UPROPERTY()
    UInputAction* IA_Jump;

    UPROPERTY()
    UInputMappingContext* IMC_TPS;
};

