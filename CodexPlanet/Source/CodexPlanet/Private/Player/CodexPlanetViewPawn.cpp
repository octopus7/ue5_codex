// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/CodexPlanetViewPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Scene.h"
#include "GameFramework/SpringArmComponent.h"

ACodexPlanetViewPawn::ACodexPlanetViewPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(SceneRoot);
	CameraBoom->TargetArmLength = 1250.0f;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->SetRelativeRotation(FRotator(-16.0f, 0.0f, 0.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 45.0f;
	Camera->PostProcessBlendWeight = 1.0f;
	Camera->PostProcessSettings.bOverride_AutoExposureMethod = true;
	Camera->PostProcessSettings.AutoExposureMethod = EAutoExposureMethod::AEM_Manual;
	Camera->PostProcessSettings.bOverride_AutoExposureApplyPhysicalCameraExposure = true;
	Camera->PostProcessSettings.AutoExposureApplyPhysicalCameraExposure = false;
	Camera->PostProcessSettings.bOverride_AutoExposureBias = true;
	Camera->PostProcessSettings.AutoExposureBias = 0.0f;
	Camera->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
	Camera->PostProcessSettings.AutoExposureMinBrightness = 1.0f;
	Camera->PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
	Camera->PostProcessSettings.AutoExposureMaxBrightness = 1.0f;

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
