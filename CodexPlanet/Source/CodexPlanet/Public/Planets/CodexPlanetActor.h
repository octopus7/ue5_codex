// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/CodexPlacementSurface.h"
#include "CodexPlanetActor.generated.h"

class UNoisyPlanetMeshComponent;
class UPrimitiveComponent;
class USceneComponent;

UCLASS()
class CODEXPLANET_API ACodexPlanetActor : public AActor, public ICodexPlacementSurface
{
	GENERATED_BODY()

public:
	ACodexPlanetActor();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual UPrimitiveComponent* GetPlacementSurfaceComponent() const override;
	virtual bool ResolvePlacementSurfaceHit(const FHitResult& Hit, FCodexPlacementSurfaceHit& OutSurfaceHit) const override;

	void ApplyTrackballRotation(const FQuat& DeltaRotation);
	FVector GetPlanetCenter() const;
	float GetPlanetRadius() const;
	void RebuildPlanetVisual();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Planet")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Planet")
	TObjectPtr<USceneComponent> VisualRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Planet")
	TObjectPtr<UNoisyPlanetMeshComponent> GeneratedPlanetMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Planet")
	bool bRebuildOnConstruction = true;

private:
	UPROPERTY(Transient)
	TObjectPtr<UPrimitiveComponent> ActiveSurfaceComponent = nullptr;

	void SetActiveSurfaceComponent(UPrimitiveComponent* InSurfaceComponent);
};
