// TPSProjectile.cpp

#include "Projectiles/TPSProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

ATPSProjectile::ATPSProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    // Collision as root
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->InitSphereRadius(8.f);
    CollisionSphere->SetCollisionProfileName(TEXT("Projectile"));
    CollisionSphere->SetNotifyRigidBodyCollision(true);
    RootComponent = CollisionSphere;

    // Visual mesh (optional, small sphere)
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(RootComponent);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetRelativeScale3D(FVector(0.15f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
    if (SphereMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(SphereMesh.Object);
    }

    // Movement
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionSphere;
    ProjectileMovement->InitialSpeed = InitialSpeed;
    ProjectileMovement->MaxSpeed = MaxSpeed;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.0f; // straight flight by default

    // Hit delegate
    CollisionSphere->OnComponentHit.AddDynamic(this, &ATPSProjectile::OnHit);

    // Auto destroy after some time
    InitialLifeSpan = LifeSeconds;
}

void ATPSProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void ATPSProjectile::FireInDirection(const FVector& ShootDirection)
{
    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity = ShootDirection.GetSafeNormal() * ProjectileMovement->InitialSpeed;
    }
}

void ATPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           FVector NormalImpulse, const FHitResult& Hit)
{
    // Simple behavior: destroy on any valid hit that isn't self
    if (OtherActor && OtherActor != this)
    {
        Destroy();
    }
}

