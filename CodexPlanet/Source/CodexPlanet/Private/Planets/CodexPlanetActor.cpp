// Copyright Epic Games, Inc. All Rights Reserved.

#include "Planets/CodexPlanetActor.h"

#include "Components/SceneComponent.h"
#include "Planets/NoisyPlanetMeshComponent.h"

ACodexPlanetActor::ACodexPlanetActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(SceneRoot);

	GeneratedPlanetMesh = CreateDefaultSubobject<UNoisyPlanetMeshComponent>(TEXT("GeneratedPlanetMesh"));
	GeneratedPlanetMesh->SetupAttachment(VisualRoot);

	SetActiveSurfaceComponent(GeneratedPlanetMesh);
}

void ACodexPlanetActor::BeginPlay()
{
	Super::BeginPlay();

	RebuildPlanetVisual();
}

void ACodexPlanetActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (bRebuildOnConstruction)
	{
		RebuildPlanetVisual();
	}
}

UPrimitiveComponent* ACodexPlanetActor::GetPlacementSurfaceComponent() const
{
	return ActiveSurfaceComponent;
}

bool ACodexPlanetActor::ResolvePlacementSurfaceHit(const FHitResult& Hit, FCodexPlacementSurfaceHit& OutSurfaceHit) const
{
	if (Hit.GetActor() != this || Hit.Component.Get() != ActiveSurfaceComponent)
	{
		return false;
	}

	OutSurfaceHit.WorldPosition = Hit.ImpactPoint;
	OutSurfaceHit.WorldNormal = Hit.ImpactNormal.GetSafeNormal();
	OutSurfaceHit.SurfaceActor = const_cast<ACodexPlanetActor*>(this);
	OutSurfaceHit.SurfaceComponent = ActiveSurfaceComponent;
	return true;
}

void ACodexPlanetActor::ApplyTrackballRotation(const FQuat& DeltaRotation)
{
	AddActorWorldRotation(DeltaRotation, false, nullptr, ETeleportType::None);
}

FVector ACodexPlanetActor::GetPlanetCenter() const
{
	return GetActorLocation();
}

float ACodexPlanetActor::GetPlanetRadius() const
{
	return GeneratedPlanetMesh ? GeneratedPlanetMesh->GetMaxSurfaceRadius() : 0.0f;
}

void ACodexPlanetActor::RebuildPlanetVisual()
{
	if (GeneratedPlanetMesh)
	{
		GeneratedPlanetMesh->RebuildPlanetMesh();
		SetActiveSurfaceComponent(GeneratedPlanetMesh);
	}
}

void ACodexPlanetActor::SetActiveSurfaceComponent(UPrimitiveComponent* InSurfaceComponent)
{
	ActiveSurfaceComponent = InSurfaceComponent;
}
