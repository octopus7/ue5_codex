// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenProjectile.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

ACodexInvenProjectile::ACodexInvenProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(ProjectileRadius);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	RootComponent = CollisionComponent;

	ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	ProjectileMeshComponent->SetupAttachment(RootComponent);
	ProjectileMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		ProjectileMeshComponent->SetStaticMesh(SphereMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SphereMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial_Inst.BasicShapeMaterial_Inst"));
	if (SphereMaterial.Succeeded())
	{
		ProjectileMeshComponent->SetMaterial(0, SphereMaterial.Object);
	}

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
	ProjectileMovementComponent->InitialSpeed = ProjectileSpeed;
	ProjectileMovementComponent->MaxSpeed = ProjectileSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

	InitialLifeSpan = ProjectileLifeSeconds;

	CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::HandleProjectileHit);
}

void ACodexInvenProjectile::OnConstruction(const FTransform& InTransform)
{
	Super::OnConstruction(InTransform);

	CollisionComponent->SetSphereRadius(ProjectileRadius);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(ProjectileRadius / 50.0f));
	ProjectileMeshComponent->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(ProjectileColor.R, ProjectileColor.G, ProjectileColor.B));
	ProjectileMovementComponent->InitialSpeed = ProjectileSpeed;
	ProjectileMovementComponent->MaxSpeed = ProjectileSpeed;
	InitialLifeSpan = ProjectileLifeSeconds;
}

void ACodexInvenProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* InstigatorActor = GetInstigator())
	{
		CollisionComponent->IgnoreActorWhenMoving(InstigatorActor, true);
	}

	if (AActor* OwnerActor = GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(OwnerActor, true);
	}
}

void ACodexInvenProjectile::HandleProjectileHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (OtherActor == nullptr || OtherActor == this || OtherActor == GetOwner() || OtherActor == GetInstigator())
	{
		return;
	}

	Destroy();
}
