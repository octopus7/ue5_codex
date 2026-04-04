// Copyright Epic Games, Inc. All Rights Reserved.

#include "Props/CodexPlacementPreviewActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

ACodexPlacementPreviewActor::ACodexPlacementPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PreviewMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PreviewMeshComponent"));
	SetRootComponent(PreviewMeshComponent);

	PreviewMeshComponent->SetMobility(EComponentMobility::Movable);
	PreviewMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewMeshComponent->SetGenerateOverlapEvents(false);
	PreviewMeshComponent->SetCastShadow(false);
	PreviewMeshComponent->SetRenderCustomDepth(true);
}

void ACodexPlacementPreviewActor::ConfigurePreviewMesh(UStaticMesh* InStaticMesh)
{
	PreviewMeshComponent->SetStaticMesh(InStaticMesh);
}
