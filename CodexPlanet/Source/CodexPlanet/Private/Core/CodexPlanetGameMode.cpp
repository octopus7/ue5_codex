// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CodexPlanetGameMode.h"

#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/ExponentialHeightFog.h"
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
		if (bDisableTemplateHeightFog)
		{
			DisableTemplateHeightFog();
		}

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
		NormalizePlanetPresentation(*It);
		return *It;
	}

	if (!DefaultPlanetClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (ACodexPlanetActor* SpawnedPlanet = GetWorld()->SpawnActor<ACodexPlanetActor>(DefaultPlanetClass, DefaultPlanetTransform, SpawnParameters))
	{
		NormalizePlanetPresentation(SpawnedPlanet);
		return SpawnedPlanet;
	}

	return nullptr;
}

void ACodexPlanetGameMode::NormalizePlanetPresentation(ACodexPlanetActor* PlanetActor) const
{
	if (!PlanetActor)
	{
		return;
	}

	FVector PlanetLocation = PlanetActor->GetActorLocation();
	const float MinimumCenterHeight = PlanetActor->GetPlanetRadius() + DefaultPlanetGroundClearance;

	if (PlanetLocation.Z < MinimumCenterHeight)
	{
		PlanetLocation.Z = MinimumCenterHeight;
		PlanetActor->SetActorLocation(PlanetLocation);
		UE_LOG(LogTemp, Log, TEXT("Raised primary planet to Z=%.1f to clear template ground and fog."), PlanetLocation.Z);
	}
}

void ACodexPlanetGameMode::DisableTemplateHeightFog() const
{
	if (!GetWorld())
	{
		return;
	}

	int32 DisabledFogCount = 0;

	for (TActorIterator<AExponentialHeightFog> It(GetWorld()); It; ++It)
	{
		if (UExponentialHeightFogComponent* FogComponent = It->GetComponent())
		{
			FogComponent->SetFogDensity(0.0f);
			FogComponent->SetVolumetricFog(false);
			FogComponent->SetVisibility(false, true);
		}

		It->SetActorHiddenInGame(true);
		It->SetActorTickEnabled(false);
		++DisabledFogCount;
	}

	if (DisabledFogCount > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Disabled %d ExponentialHeightFog actor(s) for clearer planet presentation."), DisabledFogCount);
	}
}
