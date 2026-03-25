// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenDoorActor.h"

#include "CodexInvenDoorCountdownWidget.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"

namespace
{
	const FVector DefaultDoorPivotOffset(0.0f, -50.0f, 0.0f);
	const FVector DefaultDoorMeshOffset(0.0f, 50.0f, 0.0f);
	const FVector DefaultDoorBlockerExtent(12.0f, 50.0f, 100.0f);
	const FVector DefaultDoorCountdownWidgetOffset(0.0f, 0.0f, 240.0f);
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

	DoorCountdownWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DoorCountdownWidgetComponent"));
	DoorCountdownWidgetComponent->SetupAttachment(RootComponent);
	DoorCountdownWidgetComponent->SetRelativeLocation(DefaultDoorCountdownWidgetOffset);
	DoorCountdownWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	DoorCountdownWidgetComponent->SetDrawAtDesiredSize(true);
	DoorCountdownWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DoorCountdownWidgetComponent->SetWidgetClass(UCodexInvenDoorCountdownWidget::StaticClass());
	DoorCountdownWidgetComponent->SetHiddenInGame(true);
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
	SetDelayedCloseCountdownVisible(false);
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

	DelayedCloseEndTimeSeconds = World->GetTimeSeconds() + InDelaySeconds;
	LastCountdownDisplaySeconds = INDEX_NONE;
	World->GetTimerManager().ClearTimer(DelayedCloseTimerHandle);
	World->GetTimerManager().SetTimer(
		DelayedCloseTimerHandle,
		this,
		&ThisClass::HandleDelayedCloseExpired,
		InDelaySeconds,
		false);

	RefreshDelayedCloseCountdown();
	World->GetTimerManager().ClearTimer(DelayedCloseCountdownTimerHandle);
	World->GetTimerManager().SetTimer(
		DelayedCloseCountdownTimerHandle,
		this,
		&ThisClass::HandleDelayedCloseCountdownTick,
		1.0f,
		true,
		1.0f);
}

void ACodexInvenDoorActor::ApplyDoorVisualState()
{
	if (DoorPivotComponent == nullptr)
	{
		return;
	}

	DoorPivotComponent->SetRelativeRotation(ClosedDoorRelativeRotation + FRotator(0.0f, OpenYawOffset * DoorOpenAlpha, 0.0f));
}

UCodexInvenDoorCountdownWidget* ACodexInvenDoorActor::GetDoorCountdownWidget() const
{
	return DoorCountdownWidgetComponent != nullptr
		? Cast<UCodexInvenDoorCountdownWidget>(DoorCountdownWidgetComponent->GetUserWidgetObject())
		: nullptr;
}

void ACodexInvenDoorActor::RefreshDelayedCloseCountdown()
{
	UWorld* const World = GetWorld();
	if (World == nullptr || DelayedCloseEndTimeSeconds < 0.0)
	{
		SetDelayedCloseCountdownVisible(false);
		return;
	}

	const int32 RemainingSeconds = FMath::Max(0, FMath::CeilToInt(DelayedCloseEndTimeSeconds - World->GetTimeSeconds()));
	if (RemainingSeconds == LastCountdownDisplaySeconds)
	{
		return;
	}

	LastCountdownDisplaySeconds = RemainingSeconds;
	if (UCodexInvenDoorCountdownWidget* const CountdownWidget = GetDoorCountdownWidget())
	{
		CountdownWidget->SetRemainingSeconds(RemainingSeconds);
	}

	SetDelayedCloseCountdownVisible(true);
}

void ACodexInvenDoorActor::SetDelayedCloseCountdownVisible(const bool bInVisible) const
{
	if (DoorCountdownWidgetComponent != nullptr)
	{
		DoorCountdownWidgetComponent->SetHiddenInGame(!bInVisible);
	}

	if (UCodexInvenDoorCountdownWidget* const CountdownWidget = GetDoorCountdownWidget())
	{
		CountdownWidget->SetCountdownVisible(bInVisible);
	}
}

void ACodexInvenDoorActor::HandleDelayedCloseCountdownTick()
{
	RefreshDelayedCloseCountdown();
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
		World->GetTimerManager().ClearTimer(DelayedCloseCountdownTimerHandle);
	}

	DelayedCloseEndTimeSeconds = -1.0;
	LastCountdownDisplaySeconds = INDEX_NONE;
	SetDelayedCloseCountdownVisible(false);
}

void ACodexInvenDoorActor::HandleDelayedCloseExpired()
{
	CloseDoor();
}
