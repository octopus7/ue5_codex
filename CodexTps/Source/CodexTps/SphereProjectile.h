#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SphereProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class CODEXTPS_API ASphereProjectile : public AActor
{
    GENERATED_BODY()

public:
    ASphereProjectile();

    UFUNCTION(BlueprintCallable, Category="Projectile")
    void FireInDirection(const FVector& ShootDirection);

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
               FVector NormalImpulse, const FHitResult& Hit);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    USphereComponent* CollisionComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UStaticMeshComponent* MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
    float InitialSpeed = 3000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
    float MaxSpeed = 3000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
    float LifeSeconds = 5.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
    bool bShouldBounce = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
    float Bounciness = 0.6f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
    float GravityScale = 0.0f;
};

