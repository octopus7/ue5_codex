#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class CODEXWAVE_API AWaveProjectile : public AActor
{
    GENERATED_BODY()

public:
    AWaveProjectile();

    void InitVelocity(const FVector& Direction);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    UProjectileMovementComponent* ProjectileMovement;
};

