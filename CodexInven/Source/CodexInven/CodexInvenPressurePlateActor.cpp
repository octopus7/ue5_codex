// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenPressurePlateActor.h"

#include "CodexInvenDoorActor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"

namespace
{
	const FVector DefaultTriggerBoxExtent(80.0f, 80.0f, 100.0f);
	const FVector DefaultTriggerBoxOffset(0.0f, 0.0f, 100.0f);
}

ACodexInvenPressurePlateActor::ACodexInvenPressurePlateActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
	SetCanBeDamaged(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	TriggerBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBoxComponent"));
	TriggerBoxComponent->SetupAttachment(RootComponent);
	TriggerBoxComponent->InitBoxExtent(DefaultTriggerBoxExtent);
	TriggerBoxComponent->SetRelativeLocation(DefaultTriggerBoxOffset);
	TriggerBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBoxComponent->SetGenerateOverlapEvents(true);

	PlateMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMeshComponent"));
	PlateMeshComponent->SetupAttachment(RootComponent);
	PlateMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleTriggerBoxBeginOverlap);
	TriggerBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::HandleTriggerBoxEndOverlap);
}

void ACodexInvenPressurePlateActor::BeginPlay()
{
	Super::BeginPlay();

	if (PlateMeshComponent != nullptr)
	{
		RaisedPlateRelativeLocation = PlateMeshComponent->GetRelativeLocation();
	}

	if (TriggerBoxComponent != nullptr)
	{
		TArray<AActor*> InitialOverlaps;
		TriggerBoxComponent->GetOverlappingActors(InitialOverlaps, APawn::StaticClass());

		for (AActor* OverlappingActor : InitialOverlaps)
		{
			APawn* OverlappingPawn = Cast<APawn>(OverlappingActor);
			if (OverlappingPawn != nullptr && OverlappingPawn->IsPlayerControlled())
			{
				OverlappingPlayerPawns.Add(OverlappingPawn);
			}
		}
	}

	RefreshPressedState();
	ApplyPlateVisualState();
	UpdateTickEnabledState();
}

void ACodexInvenPressurePlateActor::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TargetAlpha = bShouldBePressed ? 1.0f : 0.0f;
	if (!FMath::IsNearlyEqual(PlatePressAlpha, TargetAlpha))
	{
		if (PlateMoveDuration <= KINDA_SMALL_NUMBER)
		{
			PlatePressAlpha = TargetAlpha;
		}
		else
		{
			const float InterpSpeed = 1.0f / PlateMoveDuration;
			PlatePressAlpha = FMath::FInterpConstantTo(PlatePressAlpha, TargetAlpha, DeltaTime, InterpSpeed);
		}

		ApplyPlateVisualState();
	}

	if (bShouldBePressed)
	{
		DrawConnectedDoorDebugLines();
	}

	UpdateTickEnabledState();
}

void ACodexInvenPressurePlateActor::ApplyPlateVisualState()
{
	if (PlateMeshComponent == nullptr)
	{
		return;
	}

	PlateMeshComponent->SetRelativeLocation(RaisedPlateRelativeLocation + FVector(0.0f, 0.0f, -PlateLowerDistance * PlatePressAlpha));
}

void ACodexInvenPressurePlateActor::DrawConnectedDoorDebugLines() const
{
	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const FVector StartLocation = (PlateMeshComponent != nullptr ? PlateMeshComponent->GetComponentLocation() : GetActorLocation()) + FVector(0.0f, 0.0f, 8.0f);
	for (const ACodexInvenDoorActor* ConnectedDoor : ConnectedDoors)
	{
		if (ConnectedDoor == nullptr)
		{
			continue;
		}

		const FVector EndLocation = ConnectedDoor->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
		DrawDebugLine(World, StartLocation, EndLocation, FColor::Green, false, 0.0f, 0, 3.0f);
	}
}

void ACodexInvenPressurePlateActor::RefreshPressedState()
{
	for (auto It = OverlappingPlayerPawns.CreateIterator(); It; ++It)
	{
		if (!It->IsValid())
		{
			It.RemoveCurrent();
		}
	}

	const bool bNewShouldBePressed = OverlappingPlayerPawns.Num() > 0;
	if (bShouldBePressed == bNewShouldBePressed)
	{
		return;
	}

	bShouldBePressed = bNewShouldBePressed;
	UpdateConnectedDoors();

	if (PlateMoveDuration <= KINDA_SMALL_NUMBER)
	{
		PlatePressAlpha = bShouldBePressed ? 1.0f : 0.0f;
		ApplyPlateVisualState();
	}

	UpdateTickEnabledState();
}

void ACodexInvenPressurePlateActor::UpdateTickEnabledState()
{
	const float TargetAlpha = bShouldBePressed ? 1.0f : 0.0f;
	const bool bIsAnimating = !FMath::IsNearlyEqual(PlatePressAlpha, TargetAlpha);
	SetActorTickEnabled(bShouldBePressed || bIsAnimating);
}

void ACodexInvenPressurePlateActor::UpdateConnectedDoors() const
{
	for (ACodexInvenDoorActor* ConnectedDoor : ConnectedDoors)
	{
		if (ConnectedDoor == nullptr)
		{
			continue;
		}

		if (bShouldBePressed)
		{
			switch (PressedDoorAction)
			{
			case ECodexInvenPressurePlatePressedDoorAction::Open:
				ConnectedDoor->OpenDoor();
				break;

			case ECodexInvenPressurePlatePressedDoorAction::Close:
				ConnectedDoor->CloseDoor();
				break;
			}
		}
		else
		{
			switch (ReleasedDoorAction)
			{
			case ECodexInvenPressurePlateReleasedDoorAction::Close:
				ConnectedDoor->CloseDoor();
				break;

			case ECodexInvenPressurePlateReleasedDoorAction::None:
				break;

			case ECodexInvenPressurePlateReleasedDoorAction::DelayedClose:
				ConnectedDoor->RequestDelayedClose(ReleasedDoorDelaySeconds);
				break;
			}
		}
	}
}

void ACodexInvenPressurePlateActor::HandleTriggerBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	const int32 OtherBodyIndex,
	const bool bFromSweep,
	const FHitResult& SweepResult)
{
	static_cast<void>(OverlappedComponent);
	static_cast<void>(OtherComp);
	static_cast<void>(OtherBodyIndex);
	static_cast<void>(bFromSweep);
	static_cast<void>(SweepResult);

	APawn* OverlappingPawn = Cast<APawn>(OtherActor);
	if (OverlappingPawn == nullptr || !OverlappingPawn->IsPlayerControlled())
	{
		return;
	}

	OverlappingPlayerPawns.Add(OverlappingPawn);
	RefreshPressedState();
}

void ACodexInvenPressurePlateActor::HandleTriggerBoxEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	const int32 OtherBodyIndex)
{
	static_cast<void>(OverlappedComponent);
	static_cast<void>(OtherComp);
	static_cast<void>(OtherBodyIndex);

	APawn* OverlappingPawn = Cast<APawn>(OtherActor);
	if (OverlappingPawn == nullptr || !OverlappingPawn->IsPlayerControlled())
	{
		return;
	}

	OverlappingPlayerPawns.Remove(OverlappingPawn);
	RefreshPressedState();
}
