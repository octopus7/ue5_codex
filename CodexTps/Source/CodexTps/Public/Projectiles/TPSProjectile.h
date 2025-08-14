// TPSProjectile.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class CODEXTPS_API ATPSProjectile : public AActor
{
    GENERATED_BODY()

public:
    ATPSProjectile();

    // Sets the initial velocity direction and magnitude
    void FireInDirection(const FVector& ShootDirection);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* VisualMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    UProjectileMovementComponent* ProjectileMovement;

    // Config
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float InitialSpeed = 2500.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float MaxSpeed = 3000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
    float LifeSeconds = 3.0f;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
               FVector NormalImpulse, const FHitResult& Hit);
};

