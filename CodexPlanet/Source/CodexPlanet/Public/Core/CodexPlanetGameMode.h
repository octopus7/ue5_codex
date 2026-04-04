// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CodexPlanetGameMode.generated.h"

class ACodexPlanetActor;

UCLASS()
class CODEXPLANET_API ACodexPlanetGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACodexPlanetGameMode();

	virtual void StartPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Planet")
	TSubclassOf<ACodexPlanetActor> DefaultPlanetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Planet")
	FTransform DefaultPlanetTransform = FTransform::Identity;

	UPROPERTY(EditDefaultsOnly, Category = "Planet", meta = (ClampMin = "0.0"))
	float DefaultPlanetGroundClearance = 250.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Rendering")
	bool bDisableTemplateHeightFog = true;

private:
	ACodexPlanetActor* EnsurePrimaryPlanet();
	void NormalizePlanetPresentation(ACodexPlanetActor* PlanetActor) const;
	void DisableTemplateHeightFog() const;
};
