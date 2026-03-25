// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenPickupSpawner.h"

#include "CodexInvenPickupActor.h"
#include "CodexInvenPickupData.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ACodexInvenPickupSpawner::ACodexInvenPickupSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	SetCanBeDamaged(false);
	PickupActorClass = ACodexInvenPickupActor::StaticClass();
}

void ACodexInvenPickupSpawner::BeginPlay()
{
	Super::BeginPlay();

	RandomStream.Initialize(FMath::Rand());

	if (!HasAuthority())
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SpawnTimerHandle,
			this,
			&ThisClass::HandleSpawnTimerElapsed,
			SpawnIntervalSeconds,
			true,
			SpawnIntervalSeconds);
	}
}

void ACodexInvenPickupSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

bool ACodexInvenPickupSpawner::CanSpawnWithActiveCount(const int32 InActiveCount, const int32 InMaxActivePickupCount)
{
	return InActiveCount < InMaxActivePickupCount;
}

FVector ACodexInvenPickupSpawner::SampleSpawnOffset2D(FRandomStream& InRandomStream, const float InMinSpawnDistance, const float InMaxSpawnDistance)
{
	const float AngleRadians = InRandomStream.FRandRange(0.0f, 2.0f * PI);
	const float RadiusSquared = InRandomStream.FRandRange(
		FMath::Square(InMinSpawnDistance),
		FMath::Square(FMath::Max(InMinSpawnDistance, InMaxSpawnDistance)));
	const float Radius = FMath::Sqrt(RadiusSquared);

	return FVector(FMath::Cos(AngleRadians) * Radius, FMath::Sin(AngleRadians) * Radius, 0.0f);
}

void ACodexInvenPickupSpawner::HandleSpawnTimerElapsed()
{
	if (!HasAuthority() || PickupActorClass == nullptr)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn == nullptr)
	{
		return;
	}

	if (!CanSpawnWithActiveCount(GetActivePickupCount(), MaxActivePickupCount))
	{
		return;
	}

	const ECodexInvenPickupType PickupType = ChooseRandomPickupType();
	const FVector SpawnLocation = ResolveSpawnLocation(*PlayerPawn);

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.Instigator = PlayerPawn;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (ACodexInvenPickupActor* SpawnedPickup = World->SpawnActor<ACodexInvenPickupActor>(PickupActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters))
	{
		SpawnedPickup->InitializeFromPickupType(PickupType);
		ActivePickupActors.Add(SpawnedPickup);
	}
}

ECodexInvenPickupType ACodexInvenPickupSpawner::ChooseRandomPickupType()
{
	const TConstArrayView<ECodexInvenPickupType> PickupTypes = CodexInvenPickupData::GetSpawnerPickupTypes();
	check(PickupTypes.Num() > 0);
	return PickupTypes[RandomStream.RandRange(0, PickupTypes.Num() - 1)];
}

FVector ACodexInvenPickupSpawner::ResolveSpawnLocation(const APawn& InPlayerPawn)
{
	const FVector PlayerLocation = InPlayerPawn.GetActorLocation();
	FVector SpawnLocation = PlayerLocation + SampleSpawnOffset2D(RandomStream, MinSpawnDistance, FMath::Max(MinSpawnDistance, MaxSpawnDistance));

	SpawnLocation.Z = PlayerLocation.Z + SpawnHeightOffset;

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return SpawnLocation;
	}

	const FVector TraceStart = FVector(SpawnLocation.X, SpawnLocation.Y, PlayerLocation.Z + GroundTraceStartOffset);
	const FVector TraceEnd = FVector(SpawnLocation.X, SpawnLocation.Y, PlayerLocation.Z - GroundTraceEndOffset);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CodexInvenPickupSpawnerGroundTrace), false);
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(&InPlayerPawn);

	FHitResult HitResult;
	if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		SpawnLocation = HitResult.ImpactPoint;
		SpawnLocation.Z += SpawnHeightOffset;
	}

	return SpawnLocation;
}

void ACodexInvenPickupSpawner::PruneInactivePickups()
{
	ActivePickupActors.RemoveAll([](const TWeakObjectPtr<ACodexInvenPickupActor>& InPickupActor)
	{
		return !InPickupActor.IsValid();
	});
}

int32 ACodexInvenPickupSpawner::GetActivePickupCount()
{
	PruneInactivePickups();
	return ActivePickupActors.Num();
}
