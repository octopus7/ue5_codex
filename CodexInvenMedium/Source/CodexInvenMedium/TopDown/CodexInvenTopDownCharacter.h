#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CodexInvenTopDownCharacter.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class USpringArmComponent;
struct FInputActionValue;

UCLASS()
class CODEXINVENMEDIUM_API ACodexInvenTopDownCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACodexInvenTopDownCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJumpingAction();
	void FirePressed();
	void RotateTowardCursor();
	void RotateTowardWorldDirection(const FVector& WorldDirection);

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> CubeVisual = nullptr;

	FVector2D LastLookInput = FVector2D::ZeroVector;
};
