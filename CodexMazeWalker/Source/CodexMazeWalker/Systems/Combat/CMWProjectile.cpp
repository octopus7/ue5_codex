#include "Systems/Combat/CMWProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ACMWProjectile::ACMWProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(12.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	RootComponent = CollisionComponent;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	InitialLifeSpan = LifeSeconds;
}

void ACMWProjectile::InitializeProjectile(AActor* InDamageCauser, float InDamage)
{
	SetOwner(InDamageCauser);
	Damage = InDamage;
}

void ACMWProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
	InitialLifeSpan = LifeSeconds;

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleOverlap);
}

void ACMWProjectile::HandleOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), GetOwner(), nullptr);
	Destroy();
}
