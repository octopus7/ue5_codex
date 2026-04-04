// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/CodexPlacementSurface.h"
#include "CodexPlaceablePropActor.generated.h"

class UPrimitiveComponent;
class UStaticMesh;
class UStaticMeshComponent;

UCLASS()
class CODEXPLANET_API ACodexPlaceablePropActor : public AActor, public ICodexPlacementSurface
{
	GENERATED_BODY()

public:
	ACodexPlaceablePropActor();

	void ConfigurePropMesh(UStaticMesh* InStaticMesh);

	virtual UPrimitiveComponent* GetPlacementSurfaceComponent() const override;
	virtual bool ResolvePlacementSurfaceHit(const FHitResult& Hit, FCodexPlacementSurfaceHit& OutSurfaceHit) const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop")
	TObjectPtr<UStaticMeshComponent> PropMeshComponent = nullptr;
};
