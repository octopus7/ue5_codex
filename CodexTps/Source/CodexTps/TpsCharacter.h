#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "UObject/StrongObjectPtr.h"
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

    void Move2D(const FInputActionValue& Value);
    void Look2D(const FInputActionValue& Value);
    void JumpStarted(const FInputActionValue& Value);
    void JumpCompleted(const FInputActionValue& Value);

    void SetupEnhancedInput();

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

    UPROPERTY(EditAnywhere, Category = "Input")
    bool bInvertLookY = true;

    // Asset-based Enhanced Input references (set in editor/BP)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess="true"))
    UInputMappingContext* IMC_TPS = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess="true"))
    UInputAction* IA_Move = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess="true"))
    UInputAction* IA_Look = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess="true"))
    UInputAction* IA_Jump = nullptr;
};
