// Fill out your copyright notice in the Description page of Project Settings.


#include "CodexInvenBasicMapGameMode.h"

#include "CodexInvenPickupActor.h"
#include "CodexInvenPickupSpawner.h"
#include "CodexInvenTopDownCharacter.h"
#include "CodexInvenTopDownPlayerController.h"
#include "CodexInvenPickupData.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ACodexInvenBasicMapGameMode::ACodexInvenBasicMapGameMode()
{
	DefaultPawnClass = ACodexInvenTopDownCharacter::StaticClass();
	PlayerControllerClass = ACodexInvenTopDownPlayerController::StaticClass();
	PickupSpawnerClass = ACodexInvenPickupSpawner::StaticClass();
	LevelStartPickupActorClass = ACodexInvenPickupActor::StaticClass();
}

void ACodexInvenBasicMapGameMode::StartPlay()
{
	Super::StartPlay();

	TrySpawnPickupSpawner();
	TrySpawnLevelStartKeyPickup();
}

void ACodexInvenBasicMapGameMode::TrySpawnPickupSpawner()
{
	if (!HasAuthority() || PickupSpawnerClass == nullptr)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<ACodexInvenPickupSpawner> It(World); It; ++It)
		{
			if (PickupSpawnerClass == nullptr || It->IsA(PickupSpawnerClass))
			{
				RuntimePickupSpawner = *It;
				return;
			}
		}

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		RuntimePickupSpawner = World->SpawnActor<ACodexInvenPickupSpawner>(PickupSpawnerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
	}
}

void ACodexInvenBasicMapGameMode::TrySpawnLevelStartKeyPickup()
{
	if (!HasAuthority() || LevelStartPickupActorClass == nullptr || RuntimeLevelStartKeyPickup != nullptr)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	APawn* const PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn == nullptr)
	{
		return;
	}

	const FVector SpawnLocation = ResolveLevelStartKeySpawnLocation(*PlayerPawn);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = PlayerPawn;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (ACodexInvenPickupActor* SpawnedPickup = World->SpawnActor<ACodexInvenPickupActor>(
		LevelStartPickupActorClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParameters))
	{
		SpawnedPickup->InitializeFromPickupType(ECodexInvenPickupType::Key);
		RuntimeLevelStartKeyPickup = SpawnedPickup;
	}
}

FVector ACodexInvenBasicMapGameMode::ResolveLevelStartKeySpawnLocation(const APawn& InPlayerPawn) const
{
	FVector SpawnDirection = InPlayerPawn.GetActorForwardVector();
	SpawnDirection.Z = 0.0f;
	if (!SpawnDirection.Normalize())
	{
		SpawnDirection = FVector::ForwardVector;
	}

	const FVector PlayerLocation = InPlayerPawn.GetActorLocation();
	FVector SpawnLocation = PlayerLocation + (SpawnDirection * LevelStartKeyDistance);
	SpawnLocation.Z = PlayerLocation.Z + LevelStartSpawnHeightOffset;

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return SpawnLocation;
	}

	const FVector TraceStart = FVector(SpawnLocation.X, SpawnLocation.Y, PlayerLocation.Z + LevelStartGroundTraceStartOffset);
	const FVector TraceEnd = FVector(SpawnLocation.X, SpawnLocation.Y, PlayerLocation.Z - LevelStartGroundTraceEndOffset);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CodexInvenLevelStartKeyTrace), false);
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(&InPlayerPawn);

	FHitResult HitResult;
	if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		SpawnLocation = HitResult.ImpactPoint;
		SpawnLocation.Z += LevelStartSpawnHeightOffset;
	}

	return SpawnLocation;
}

