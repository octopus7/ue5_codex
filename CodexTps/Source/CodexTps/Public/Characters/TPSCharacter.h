// TPSCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "TPSCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
class UStaticMeshComponent;

UCLASS()
class CODEXTPS_API ATPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ATPSCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Movement input handlers
    void Input_Move(const FInputActionValue& Value);
    void Input_Look(const FInputActionValue& Value);
    void Input_JumpStarted(const FInputActionValue& Value);
    void Input_JumpCompleted(const FInputActionValue& Value);

protected:
    // Camera boom (keeps camera behind the character)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* CameraBoom;

    // Follow camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FollowCamera;

    // Simple placeholder mesh (cube) you can replace later
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
    UStaticMeshComponent* PlaceholderMesh;

    // Enhanced Input
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Mapping")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
    UInputAction* IA_Move;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
    UInputAction* IA_Look;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Actions")
    UInputAction* IA_Jump;

    // Look sensitivity multipliers (optional tuning)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Tuning")
    float LookYawScale = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Tuning")
    float LookPitchScale = 1.0f;
};

