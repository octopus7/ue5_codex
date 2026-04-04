// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/CodexPlanetViewPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACodexPlanetViewPawn::ACodexPlanetViewPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(SceneRoot);
	CameraBoom->TargetArmLength = 1600.0f;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->SetRelativeRotation(FRotator(-22.0f, 0.0f, 0.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	AutoPossessPlayer = EAutoReceiveInput::Disabled;
}

void ACodexPlanetViewPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateViewAnchor();
}

void ACodexPlanetViewPawn::SetFocusActor(AActor* InFocusActor)
{
	FocusActor = InFocusActor;
	UpdateViewAnchor();
}

FVector ACodexPlanetViewPawn::GetFocusPoint() const
{
	return FocusActor.IsValid() ? FocusActor->GetActorLocation() + FocusOffset : FVector::ZeroVector;
}

void ACodexPlanetViewPawn::UpdateViewAnchor()
{
	SetActorLocation(GetFocusPoint());
}
