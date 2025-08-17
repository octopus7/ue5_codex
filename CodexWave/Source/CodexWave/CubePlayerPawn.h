#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "CubePlayerPawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class UInputAction;
class UInputMappingContext;
class AWaveProjectile;

UCLASS()
class CODEXWAVE_API ACubePlayerPawn : public APawn
{
    GENERATED_BODY()

public:
    ACubePlayerPawn();

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void BeginPlay() override;

protected:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* Camera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraArmLength = 1400.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    UFloatingPawnMovement* FloatingMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bRotateToMovement = !true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RotationInterpSpeed = 10.f;

    // Projectile
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    TSubclassOf<AWaveProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
    float ProjectileSpawnOffset = 60.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Preview")
    float TrajectoryPreviewTime = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Preview")
    float TrajectorySegmentLength = 120.f;

    // Enhanced Input (created at runtime; keep transient so not saved)
    UPROPERTY(Transient)
    UInputMappingContext* DefaultMappingContext = nullptr;

    UPROPERTY(Transient)
    UInputAction* MoveForwardAction = nullptr;

    UPROPERTY(Transient)
    UInputAction* MoveRightAction = nullptr;

    UPROPERTY(Transient)
    UInputAction* FireAction = nullptr;

protected:
    // Input callbacks
    void MoveForward(const FInputActionValue& Value);
    void MoveRight(const FInputActionValue& Value);
    void Fire(const FInputActionValue& Value);

    virtual UPawnMovementComponent* GetMovementComponent() const override { return (UPawnMovementComponent*)FloatingMovement; }

    void EnsureInputAssets();
    bool bMappingApplied = false;
};
