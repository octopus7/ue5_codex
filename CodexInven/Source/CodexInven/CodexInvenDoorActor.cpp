// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenDoorActor.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

namespace
{
	const FVector DefaultDoorPivotOffset(0.0f, -50.0f, 0.0f);
	const FVector DefaultDoorMeshOffset(0.0f, 50.0f, 0.0f);
	const FVector DefaultDoorBlockerExtent(12.0f, 50.0f, 100.0f);
}

ACodexInvenDoorActor::ACodexInvenDoorActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetCanBeDamaged(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	DoorBlockerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorBlockerComponent"));
	DoorBlockerComponent->SetupAttachment(RootComponent);
	DoorBlockerComponent->InitBoxExtent(DefaultDoorBlockerExtent);
	DoorBlockerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DoorBlockerComponent->SetCollisionObjectType(ECC_WorldDynamic);
	DoorBlockerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	DoorBlockerComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	DoorBlockerComponent->SetGenerateOverlapEvents(false);
	DoorBlockerComponent->SetHiddenInGame(true);

	DoorPivotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivotComponent"));
	DoorPivotComponent->SetupAttachment(RootComponent);
	DoorPivotComponent->SetRelativeLocation(DefaultDoorPivotOffset);

	DoorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMeshComponent"));
	DoorMeshComponent->SetupAttachment(DoorPivotComponent);
	DoorMeshComponent->SetRelativeLocation(DefaultDoorMeshOffset);
	DoorMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACodexInvenDoorActor::BeginPlay()
{
	Super::BeginPlay();

	if (DoorPivotComponent != nullptr)
	{
		ClosedDoorRelativeRotation = DoorPivotComponent->GetRelativeRotation();
	}

	ApplyDoorVisualState();
	UpdateDoorBlockerCollision();
}

void ACodexInvenDoorActor::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TargetAlpha = bShouldBeOpen ? 1.0f : 0.0f;
	if (FMath::IsNearlyEqual(DoorOpenAlpha, TargetAlpha))
	{
		return;
	}

	if (DoorMoveDuration <= KINDA_SMALL_NUMBER)
	{
		DoorOpenAlpha = TargetAlpha;
	}
	else
	{
		const float InterpSpeed = 1.0f / DoorMoveDuration;
		DoorOpenAlpha = FMath::FInterpConstantTo(DoorOpenAlpha, TargetAlpha, DeltaTime, InterpSpeed);
	}

	ApplyDoorVisualState();
	UpdateDoorBlockerCollision();
}

void ACodexInvenDoorActor::OpenDoor()
{
	ClearPendingDelayedClose();
	SetDoorOpen(true);
}

void ACodexInvenDoorActor::CloseDoor()
{
	ClearPendingDelayedClose();
	SetDoorOpen(false);
}

void ACodexInvenDoorActor::SetDoorOpen(const bool bInShouldBeOpen)
{
	if (bShouldBeOpen == bInShouldBeOpen)
	{
		return;
	}

	bShouldBeOpen = bInShouldBeOpen;

	if (DoorMoveDuration <= KINDA_SMALL_NUMBER)
	{
		DoorOpenAlpha = bShouldBeOpen ? 1.0f : 0.0f;
		if (HasActorBegunPlay())
		{
			ApplyDoorVisualState();
		}
	}

	UpdateDoorBlockerCollision();
}

void ACodexInvenDoorActor::RequestDelayedClose(const float InDelaySeconds)
{
	if (InDelaySeconds <= KINDA_SMALL_NUMBER)
	{
		CloseDoor();
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		CloseDoor();
		return;
	}

	World->GetTimerManager().ClearTimer(DelayedCloseTimerHandle);
	World->GetTimerManager().SetTimer(
		DelayedCloseTimerHandle,
		this,
		&ThisClass::HandleDelayedCloseExpired,
		InDelaySeconds,
		false);
}

void ACodexInvenDoorActor::ApplyDoorVisualState()
{
	if (DoorPivotComponent == nullptr)
	{
		return;
	}

	DoorPivotComponent->SetRelativeRotation(ClosedDoorRelativeRotation + FRotator(0.0f, OpenYawOffset * DoorOpenAlpha, 0.0f));
}

void ACodexInvenDoorActor::UpdateDoorBlockerCollision() const
{
	if (DoorBlockerComponent == nullptr)
	{
		return;
	}

	const bool bShouldDisableCollision = bShouldBeOpen || DoorOpenAlpha > KINDA_SMALL_NUMBER;
	DoorBlockerComponent->SetCollisionEnabled(bShouldDisableCollision ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);
}

void ACodexInvenDoorActor::ClearPendingDelayedClose()
{
	if (UWorld* const World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DelayedCloseTimerHandle);
	}
}

void ACodexInvenDoorActor::HandleDelayedCloseExpired()
{
	CloseDoor();
}
