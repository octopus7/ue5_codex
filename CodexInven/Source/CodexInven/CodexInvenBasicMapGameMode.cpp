// Fill out your copyright notice in the Description page of Project Settings.


#include "CodexInvenBasicMapGameMode.h"

#include "CodexInvenPickupSpawner.h"
#include "CodexInvenTopDownCharacter.h"
#include "CodexInvenTopDownPlayerController.h"
#include "EngineUtils.h"

ACodexInvenBasicMapGameMode::ACodexInvenBasicMapGameMode()
{
	DefaultPawnClass = ACodexInvenTopDownCharacter::StaticClass();
	PlayerControllerClass = ACodexInvenTopDownPlayerController::StaticClass();
	PickupSpawnerClass = ACodexInvenPickupSpawner::StaticClass();
}

void ACodexInvenBasicMapGameMode::StartPlay()
{
	Super::StartPlay();

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

