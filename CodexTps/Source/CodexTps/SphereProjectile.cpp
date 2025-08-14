#include "SphereProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

ASphereProjectile::ASphereProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    CollisionComp->InitSphereRadius(12.f);
    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComp->SetNotifyRigidBodyCollision(true);
    CollisionComp->SetGenerateOverlapEvents(false);
    RootComponent = CollisionComp;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootComponent);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Visual: engine sphere mesh, tinted
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMesh.Succeeded())
    {
        MeshComp->SetStaticMesh(SphereMesh.Object);
    }

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = InitialSpeed;
    ProjectileMovement->MaxSpeed = MaxSpeed;
    ProjectileMovement->ProjectileGravityScale = GravityScale;
    ProjectileMovement->bShouldBounce = bShouldBounce;
    ProjectileMovement->Bounciness = Bounciness;
    ProjectileMovement->Friction = 0.2f;

    // Bind hit
    CollisionComp->OnComponentHit.AddDynamic(this, &ASphereProjectile::OnHit);
}

void ASphereProjectile::BeginPlay()
{
    Super::BeginPlay();

    SetLifeSpan(LifeSeconds);
}

void ASphereProjectile::FireInDirection(const FVector& ShootDirection)
{
    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity = ShootDirection.GetSafeNormal() * InitialSpeed;
    }
}

void ASphereProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this && OtherComp && OtherComp->IsSimulatingPhysics())
    {
        OtherComp->AddImpulseAtLocation(ProjectileMovement ? ProjectileMovement->Velocity * 5.f : FVector::ZeroVector, Hit.ImpactPoint);
    }

    if (!bShouldBounce)
    {
        Destroy();
    }
}
