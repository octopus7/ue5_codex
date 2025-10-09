#include "Projectiles/PSVProjectile.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

void APSVProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (ProjectileMesh)
    {
        ProjectileMesh->OnComponentHit.AddDynamic(this, &APSVProjectile::HandleProjectileHit);
    }
}

void APSVProjectile::HandleProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }

    if (OtherComp && OtherComp->IsSimulatingPhysics())
    {
        OtherComp->AddImpulseAtLocation(GetVelocity().GetSafeNormal() * ImpactImpulse, Hit.ImpactPoint);
    }

    if (Damage > 0.f)
    {
        UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);
    }

    Destroy();
}
