#include "Projectiles/PSVProjectile.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

APSVProjectile::APSVProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetCollisionProfileName(TEXT("Projectile"));
    ProjectileMesh->SetGenerateOverlapEvents(true);
    RootComponent = ProjectileMesh;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1600.f;
    ProjectileMovement->MaxSpeed = 1600.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->ProjectileGravityScale = 0.f;

    InitialLifeSpan = 3.0f;
}
