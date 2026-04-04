// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "NoisyPlanetMeshComponent.generated.h"

UCLASS(ClassGroup = (CodexPlanet), meta = (BlueprintSpawnableComponent))
class CODEXPLANET_API UNoisyPlanetMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UNoisyPlanetMeshComponent(const FObjectInitializer& ObjectInitializer);

	void RebuildPlanetMesh();

	float GetBaseRadius() const
	{
		return BaseRadius;
	}

	float GetMaxSurfaceRadius() const
	{
		return BaseRadius + FMath::Max(0.0f, NoiseAmplitude);
	}

protected:
	UPROPERTY(EditAnywhere, Category = "Planet")
	float BaseRadius = 500.0f;

	UPROPERTY(EditAnywhere, Category = "Planet", meta = (ClampMin = "6", UIMin = "6"))
	int32 LatitudeSegments = 24;

	UPROPERTY(EditAnywhere, Category = "Planet", meta = (ClampMin = "8", UIMin = "8"))
	int32 LongitudeSegments = 48;

	UPROPERTY(EditAnywhere, Category = "Planet")
	float NoiseAmplitude = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Planet", meta = (ClampMin = "0.01", UIMin = "0.01"))
	float NoiseFrequency = 2.4f;

	UPROPERTY(EditAnywhere, Category = "Planet")
	FVector NoiseOffset = FVector(13.0f, 5.0f, 29.0f);

private:
	float SampleSurfaceRadius(const FVector& UnitDirection) const;
};
