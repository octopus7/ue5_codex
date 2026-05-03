// Copyright Epic Games, Inc. All Rights Reserved.

#include "TigerShopCameraRig.h"

#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"

ATigerShopCameraRig::ATigerShopCameraRig()
{
	PrimaryActorTick.bCanEverTick = false;

	RigRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RigRoot"));
	SetRootComponent(RigRoot);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RigRoot);
	SpringArmComponent->TargetArmLength = 1300.0f;
	SpringArmComponent->SetRelativeRotation(FRotator(-36.0f, 138.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->FieldOfView = 45.0f;
}

void ATigerShopCameraRig::AddPanInput(const FVector2D& Delta)
{
	SetPanOffset(PanOffset + Delta);
}

void ATigerShopCameraRig::SetPanOffset(const FVector2D& NewPanOffset)
{
	PanOffset.X = FMath::Clamp(NewPanOffset.X, MinPanBounds.X, MaxPanBounds.X);
	PanOffset.Y = FMath::Clamp(NewPanOffset.Y, MinPanBounds.Y, MaxPanBounds.Y);
	SetActorLocation(FVector(PanOffset.X, PanOffset.Y, 0.0f));
}
