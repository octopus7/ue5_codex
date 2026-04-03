#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CodexHarnessCharacter.generated.h"

class UCameraComponent;
class UCodexHarnessHealthComponent;
class UDamageType;
class UStaticMesh;
class UStaticMeshComponent;
class USpringArmComponent;
class AController;
struct FPropertyChangedEvent;

UCLASS()
class CODEXHARNESS_API ACodexHarnessCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACodexHarnessCharacter();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void RefreshVisualMeshDefaults();

	UFUNCTION(BlueprintPure, Category = "Camera")
	USpringArmComponent* GetCameraBoom() const;

	UFUNCTION(BlueprintPure, Category = "Camera")
	UCameraComponent* GetFollowCamera() const;

	UFUNCTION(BlueprintPure, Category = "Visual")
	UStaticMeshComponent* GetVisualMeshComponent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	UCodexHarnessHealthComponent* GetHealthComponent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const;

	void MoveInTopDownPlane(const FVector2D& MovementInput);
	void AimAtWorldLocation(const FVector& WorldLocation);
	void FireAtWorldLocation(const FVector& WorldLocation);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UStaticMesh> DefaultVisualMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float WeaponRange = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float WeaponDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Reaction", meta = (ClampMin = "0.0"))
	float DamageKnockbackStrength = 900.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Reaction", meta = (ClampMin = "0.0"))
	float DamageKnockbackUpwardVelocity = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FVector DefaultVisualMeshScale = FVector(8.0f, 8.0f, 8.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FVector DefaultVisualMeshOffset = FVector(0.0f, 0.0f, -48.0f);

private:
	void HandleDeath();
	void RotateTowardWorldDirection(const FVector& WorldDirection);

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> VisualMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCodexHarnessHealthComponent> HealthComponent = nullptr;
};
