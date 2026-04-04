// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CodexPlanetGameMode.h"

#include "EngineUtils.h"
#include "Planets/CodexPlanetActor.h"
#include "Player/CodexPlanetPlayerController.h"
#include "Player/CodexPlanetViewPawn.h"

ACodexPlanetGameMode::ACodexPlanetGameMode()
{
	PlayerControllerClass = ACodexPlanetPlayerController::StaticClass();
	DefaultPawnClass = ACodexPlanetViewPawn::StaticClass();
	DefaultPlanetClass = ACodexPlanetActor::StaticClass();
}

void ACodexPlanetGameMode::StartPlay()
{
	Super::StartPlay();

	if (ACodexPlanetActor* PlanetActor = EnsurePrimaryPlanet())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (ACodexPlanetPlayerController* PlanetController = Cast<ACodexPlanetPlayerController>(It->Get()))
			{
				PlanetController->SetObservedPlanet(PlanetActor);
			}
		}
	}
}

ACodexPlanetActor* ACodexPlanetGameMode::EnsurePrimaryPlanet()
{
	if (!GetWorld())
	{
		return nullptr;
	}

	for (TActorIterator<ACodexPlanetActor> It(GetWorld()); It; ++It)
	{
		return *It;
	}

	if (!DefaultPlanetClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	return GetWorld()->SpawnActor<ACodexPlanetActor>(DefaultPlanetClass, DefaultPlanetTransform, SpawnParameters);
}
