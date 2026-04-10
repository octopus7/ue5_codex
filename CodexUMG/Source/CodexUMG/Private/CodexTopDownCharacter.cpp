// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodexTopDownCharacter.h"

#include "Camera/CameraComponent.h"
#include "CodexGameInstance.h"
#include "CodexProjectileActor.h"
#include "CodexProjectileConfigDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACodexTopDownCharacter::ACodexTopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetAbsolute(false, true, false);
	CameraBoom->TargetArmLength = 1000.0f;
	CameraBoom->SetRelativeRotation(FRotator(-55.0f, 45.0f, 0.0f));
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bDoCollisionTest = false;

	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false;
}

void ACodexTopDownCharacter::ConsumeMoveInput_Implementation(FVector2D MoveAxis)
{
	if (MoveAxis.IsNearlyZero())
	{
		return;
	}

	const float CameraYaw = CameraBoom ? CameraBoom->GetComponentRotation().Yaw : 0.0f;
	const FRotator YawRotation(0.0f, CameraYaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MoveAxis.Y);
	AddMovementInput(RightDirection, MoveAxis.X);
}

void ACodexTopDownCharacter::ConsumeFireInput_Implementation()
{
	const UCodexGameInstance* CodexGameInstance = GetGameInstance<UCodexGameInstance>();
	const UCodexProjectileConfigDataAsset* ProjectileConfig = CodexGameInstance ? CodexGameInstance->GetPlayerProjectileConfig() : nullptr;
	const TSubclassOf<ACodexProjectileActor> ProjectileClass = ProjectileConfig ? ProjectileConfig->GetProjectileClass() : TSubclassOf<ACodexProjectileActor>();

	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile config is missing on %s."), *GetName());
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector ForwardDirection = GetActorForwardVector().GetSafeNormal();
	const FVector SpawnLocation = GetActorLocation() + (ForwardDirection * ProjectileSpawnDistance) + FVector(0.0f, 0.0f, ProjectileSpawnHeight);
	const FRotator SpawnRotation = ForwardDirection.Rotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	World->SpawnActor<ACodexProjectileActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters);
}
