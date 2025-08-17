#include "WaveProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyConeCharacter.h"
#include "DrawDebugHelpers.h"

AWaveProjectile::AWaveProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(12.f);
    Collision->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    Collision->SetSimulatePhysics(false);
    Collision->SetGenerateOverlapEvents(true);
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    Collision->BodyInstance.bUseCCD = true; // reduce tunneling at high speed
    SetRootComponent(Collision);

    Collision->OnComponentBeginOverlap.AddDynamic(this, &AWaveProjectile::OnCollisionOverlap);
    Collision->SetNotifyRigidBodyCollision(true);
    Collision->OnComponentHit.AddDynamic(this, &AWaveProjectile::HandleComponentHit);
    OnActorHit.AddDynamic(this, &AWaveProjectile::HandleActorHit);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetRelativeScale3D(FVector(0.25f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (SphereMesh.Succeeded())
    {
        Mesh->SetStaticMesh(SphereMesh.Object);
    }

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 1500.f;
    ProjectileMovement->MaxSpeed = 1500.f;
    ProjectileMovement->ProjectileGravityScale = 0.f; // no gravity
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->bInitialVelocityInLocalSpace = false;
    ProjectileMovement->SetUpdatedComponent(Collision);

    SetLifeSpan(5.f);
}

void AWaveProjectile::BeginPlay()
{
    Super::BeginPlay();

    // Prevent immediate collision with owner/instigator
    if (AActor* Ow = GetOwner())
    {
        Collision->IgnoreActorWhenMoving(Ow, true);
    }
    if (APawn* Inst = GetInstigator())
    {
        Collision->IgnoreActorWhenMoving(Inst, true);
    }
}

void AWaveProjectile::InitVelocity(const FVector& Direction)
{
    if (ProjectileMovement)
    {
        FVector Dir = Direction;
        Dir.Z = 0.f; // keep horizontal
        Dir.Normalize();
        ProjectileMovement->Velocity = Dir * ProjectileMovement->InitialSpeed;
    }
}

void AWaveProjectile::HandleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }
    if (OtherActor->IsA(AEnemyConeCharacter::StaticClass()))
    {
        AController* InstigatorController = GetInstigatorController();
        UGameplayStatics::ApplyDamage(OtherActor, 1.f, InstigatorController, this, nullptr);
        if (UWorld* World = GetWorld())
        {
            DrawDebugPoint(World, Hit.ImpactPoint, 12.f, FColor::Red, false, 2.0f);
        }
        Destroy();
    }
}

float AWaveProjectile::GetInitialSpeed() const
{
    return ProjectileMovement ? ProjectileMovement->InitialSpeed : 0.f;
}

void AWaveProjectile::OnCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                         const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }

    // Only damage our enemy cone for now (임시 스코프)
    if (OtherActor->IsA(AEnemyConeCharacter::StaticClass()))
    {
        AController* InstigatorController = GetInstigatorController();
        UGameplayStatics::ApplyDamage(OtherActor, 1.f, InstigatorController, this, nullptr);
        Destroy();
    }
}

void AWaveProjectile::HandleActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
    {
        return;
    }
    if (OtherActor->IsA(AEnemyConeCharacter::StaticClass()))
    {
        AController* InstigatorController = GetInstigatorController();
        UGameplayStatics::ApplyDamage(OtherActor, 1.f, InstigatorController, this, nullptr);
        Destroy();
    }
}
