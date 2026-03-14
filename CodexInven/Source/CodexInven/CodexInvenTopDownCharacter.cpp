// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenTopDownCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACodexInvenTopDownCharacter::ACodexInvenTopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 650.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 550.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = CameraBoomLength;
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetRelativeRotation(CameraBoomRotation);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bUsePawnControlRotation = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;
}

void ACodexInvenTopDownCharacter::MoveInTopDownPlane(const FVector2D& InMoveInput)
{
	if (InMoveInput.IsNearlyZero())
	{
		return;
	}

	AddMovementInput(FVector::ForwardVector, InMoveInput.Y);
	AddMovementInput(FVector::RightVector, InMoveInput.X);
}

void ACodexInvenTopDownCharacter::BeginGenericJump()
{
	Jump();
}

void ACodexInvenTopDownCharacter::EndGenericJump()
{
	StopJumping();
}

void ACodexInvenTopDownCharacter::AimAtWorldLocation(const FVector& InWorldLocation)
{
	const FVector PlanarDirection = GetPlanarDirectionTo(InWorldLocation);
	if (PlanarDirection.IsNearlyZero())
	{
		return;
	}

	SetActorRotation(PlanarDirection.Rotation());
}

void ACodexInvenTopDownCharacter::FireAtWorldLocation(const FVector& InWorldLocation)
{
	const FVector PlanarDirection = GetPlanarDirectionTo(InWorldLocation);
	if (PlanarDirection.IsNearlyZero())
	{
		return;
	}

	AimAtWorldLocation(InWorldLocation);

	const FVector TraceStart = GetActorLocation() + FVector(0.0f, 0.0f, BaseEyeHeight);
	const FVector TraceEnd = TraceStart + (PlanarDirection * FireTraceDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CodexInvenTopDownFire), false, this);
	const bool bHitBlockingObject = GetWorld() != nullptr
		&& GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	const FVector ImpactPoint = bHitBlockingObject ? HitResult.ImpactPoint : TraceEnd;
	OnFireTriggered(TraceStart, InWorldLocation, ImpactPoint, bHitBlockingObject);
}

FVector ACodexInvenTopDownCharacter::GetPlanarDirectionTo(const FVector& InWorldLocation) const
{
	FVector PlanarDirection = InWorldLocation - GetActorLocation();
	PlanarDirection.Z = 0.0f;
	PlanarDirection.Normalize();
	return PlanarDirection;
}
