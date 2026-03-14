// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenTopDownCharacter.h"

#include "CodexInvenGameInstance.h"
#include "CodexInvenProjectile.h"
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

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const TSubclassOf<ACodexInvenProjectile> ProjectileClass = GetConfiguredProjectileClass();
	if (ProjectileClass == nullptr)
	{
		return;
	}

	const FVector SpawnDirection = GetActorForwardVector();
	const FVector SpawnLocation = GetActorLocation() + (SpawnDirection * ProjectileSpawnForwardDistance);
	const FRotator SpawnRotation = SpawnDirection.Rotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (ACodexInvenProjectile* SpawnedProjectile = World->SpawnActor<ACodexInvenProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters))
	{
		OnProjectileSpawned(SpawnedProjectile, SpawnLocation, SpawnDirection);
	}
}

FVector ACodexInvenTopDownCharacter::GetPlanarDirectionTo(const FVector& InWorldLocation) const
{
	FVector PlanarDirection = InWorldLocation - GetActorLocation();
	PlanarDirection.Z = 0.0f;
	PlanarDirection.Normalize();
	return PlanarDirection;
}

TSubclassOf<ACodexInvenProjectile> ACodexInvenTopDownCharacter::GetConfiguredProjectileClass() const
{
	const UCodexInvenGameInstance* CodexInvenGameInstance = GetGameInstance<UCodexInvenGameInstance>();
	if (CodexInvenGameInstance == nullptr)
	{
		return ACodexInvenProjectile::StaticClass();
	}

	if (TSubclassOf<ACodexInvenProjectile> ProjectileClass = CodexInvenGameInstance->GetProjectileClass())
	{
		return ProjectileClass;
	}

	return ACodexInvenProjectile::StaticClass();
}
