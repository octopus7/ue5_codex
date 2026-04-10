// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractableActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Interaction/CodexInteractionComponent.h"
#include "TimerManager.h"

ACodexInteractableActor::ACodexInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(SceneRoot);

	InteractionComponent = CreateDefaultSubobject<UCodexInteractionComponent>(TEXT("InteractionComponent"));
}

void ACodexInteractableActor::HandleInteractionRequested_Implementation(const FCodexInteractionRequest& Request)
{
	if (bPendingConsume)
	{
		return;
	}

	bPendingConsume = true;

	if (InteractionComponent)
	{
		InteractionComponent->SetInteractionEnabled(false);
	}

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	ReceiveInteractionRequested(Request);

	if (ConsumeDelaySeconds <= 0.0f)
	{
		ConsumeAndDestroy();
		return;
	}

	if (UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(
			TimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				ConsumeAndDestroy();
			}),
			ConsumeDelaySeconds,
			false);
	}
}

void ACodexInteractableActor::HandleInteractionEnded_Implementation(const FCodexInteractionRequest& Request)
{
	ReceiveInteractionEnded(Request);
}

void ACodexInteractableActor::ConsumeAndDestroy()
{
	Destroy();
}
