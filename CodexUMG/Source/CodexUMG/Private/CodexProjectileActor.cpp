// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodexProjectileActor.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

ACodexProjectileActor::ACodexProjectileActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	SetRootComponent(ProjectileMesh);
	ProjectileMesh->SetMobility(EComponentMobility::Movable);
	ProjectileMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	ProjectileMesh->SetGenerateOverlapEvents(true);
	ProjectileMesh->SetCanEverAffectNavigation(false);
	ProjectileMesh->SetRelativeScale3D(FVector(0.2f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultProjectileMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (DefaultProjectileMesh.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(DefaultProjectileMesh.Object);
	}

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 1600.0f;
	ProjectileMovement->MaxSpeed = 1600.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	InitialLifeSpan = 5.0f;
}
