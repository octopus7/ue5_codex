// Copyright Epic Games, Inc. All Rights Reserved.

#include "Props/CodexPlaceablePropActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

ACodexPlaceablePropActor::ACodexPlaceablePropActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PropMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropMeshComponent"));
	SetRootComponent(PropMeshComponent);

	PropMeshComponent->SetMobility(EComponentMobility::Movable);
	PropMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PropMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	PropMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	PropMeshComponent->SetGenerateOverlapEvents(false);
}

void ACodexPlaceablePropActor::ConfigurePropMesh(UStaticMesh* InStaticMesh)
{
	PropMeshComponent->SetStaticMesh(InStaticMesh);
}

UPrimitiveComponent* ACodexPlaceablePropActor::GetPlacementSurfaceComponent() const
{
	return PropMeshComponent;
}

bool ACodexPlaceablePropActor::ResolvePlacementSurfaceHit(const FHitResult& Hit, FCodexPlacementSurfaceHit& OutSurfaceHit) const
{
	if (Hit.GetActor() != this || Hit.Component.Get() != PropMeshComponent)
	{
		return false;
	}

	OutSurfaceHit.WorldPosition = Hit.ImpactPoint;
	OutSurfaceHit.WorldNormal = Hit.ImpactNormal.GetSafeNormal();
	OutSurfaceHit.SurfaceActor = const_cast<ACodexPlaceablePropActor*>(this);
	OutSurfaceHit.SurfaceComponent = PropMeshComponent;
	return true;
}
