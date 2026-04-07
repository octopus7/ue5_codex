#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CMWTopDownCharacter.generated.h"

class UCameraComponent;
class UCMWCombatComponent;
class UStaticMeshComponent;
class USpringArmComponent;
struct FInputActionValue;

UCLASS()
class CODEXMAZEWALKER_API ACMWTopDownCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACMWTopDownCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void SetAimWorldLocation(const FVector& NewAimWorldLocation);

protected:
	virtual void BeginPlay() override;

	void EnsureSpawnedAboveWalkableSurface();
	void HandleMove(const FInputActionValue& InputActionValue);
	void HandleAttack();
	void HandleToggleAttackMode();
	void FaceAimLocation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> TopDownCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UCMWCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> PlayerVisualMesh;

	UPROPERTY(Transient)
	FVector AimWorldLocation = FVector::ZeroVector;

	UPROPERTY(Transient)
	bool bHasAimWorldLocation = false;
};
