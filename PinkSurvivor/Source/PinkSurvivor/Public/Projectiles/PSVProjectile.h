#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PSVProjectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class PINKSURVIVOR_API APSVProjectile : public AActor
{
    GENERATED_BODY()

public:
    APSVProjectile();

    UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UStaticMeshComponent> ProjectileMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile", meta=(ClampMin="0.0"))
    float Damage = 10.f;
};
