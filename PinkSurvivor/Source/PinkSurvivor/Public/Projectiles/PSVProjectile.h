#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PSVProjectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class UPrimitiveComponent;

UCLASS()
class PINKSURVIVOR_API APSVProjectile : public AActor
{
    GENERATED_BODY()

public:
    APSVProjectile();

    UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void HandleProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UStaticMeshComponent> ProjectileMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile", meta=(ClampMin="0.0"))
    float Damage = 10.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile", meta=(ClampMin="0.0"))
    float ImpactImpulse = 1000.f;
};
