#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CMWProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class CODEXMAZEWALKER_API ACMWProjectile : public AActor
{
	GENERATED_BODY()

public:
	ACMWProjectile();

	void InitializeProjectile(AActor* InDamageCauser, float InDamage);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (ClampMin = "0.0"))
	float InitialSpeed = 1800.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (ClampMin = "0.0"))
	float Damage = 15.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (ClampMin = "0.0"))
	float LifeSeconds = 5.0f;
};
