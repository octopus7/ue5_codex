#pragma once

#include "GameFramework/Character.h"

#include "CHPlayerCharacter.generated.h"

class UCameraComponent;
class UCHHealthComponent;
class USpringArmComponent;

USTRUCT(BlueprintType)
struct FCHFireResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bShotExecuted = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bBlockedByCooldown = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHit = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector ShotStart = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector AimWorldPoint = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector TraceEnd = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	FVector ImpactPoint = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	FName HitActorName = NAME_None;
};

UCLASS()
class CODEXHARNESS_API ACHPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACHPlayerCharacter();
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Move(const FVector2D& MovementInput, float DeltaSeconds);
	void AimAt(const FVector& WorldPoint);
	bool TryFire(FCHFireResult& OutFireResult);

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCHHealthComponent* GetHealthComponent() const { return HealthComponent; }
	FORCEINLINE UCameraComponent* GetViewCamera() const { return ViewCamera; }
	FORCEINLINE const FVector& GetLastAimWorldPoint() const { return LastAimWorldPoint; }
	FORCEINLINE const FCHFireResult& GetLastFireResult() const { return LastFireResult; }
	FORCEINLINE bool HasAimWorldPoint() const { return bHasAimWorldPoint; }
	FORCEINLINE bool IsDead() const { return bIsDead; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UCHHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MoveSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float FireRange = 2500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float FireCooldownSeconds = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float FireDamage = 34.0f;

	void HandleHealthDepleted(UCHHealthComponent* InHealthComponent);

	FVector LastAimWorldPoint = FVector::ZeroVector;
	FCHFireResult LastFireResult;
	bool bHasAimWorldPoint = false;
	bool bIsDead = false;
	float LastFireWorldTime = -1000.0f;
};
