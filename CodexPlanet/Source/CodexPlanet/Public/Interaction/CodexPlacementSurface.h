// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "UObject/Interface.h"
#include "CodexPlacementSurface.generated.h"

class UPrimitiveComponent;

USTRUCT(BlueprintType)
struct CODEXPLANET_API FCodexPlacementSurfaceHit
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Placement")
	FVector WorldPosition = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Placement")
	FVector WorldNormal = FVector::UpVector;

	UPROPERTY(BlueprintReadOnly, Category = "Placement")
	TObjectPtr<AActor> SurfaceActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Placement")
	TObjectPtr<UPrimitiveComponent> SurfaceComponent = nullptr;
};

UINTERFACE(BlueprintType)
class CODEXPLANET_API UCodexPlacementSurface : public UInterface
{
	GENERATED_BODY()
};

class CODEXPLANET_API ICodexPlacementSurface
{
	GENERATED_BODY()

public:
	virtual UPrimitiveComponent* GetPlacementSurfaceComponent() const = 0;
	virtual bool ResolvePlacementSurfaceHit(const FHitResult& Hit, FCodexPlacementSurfaceHit& OutSurfaceHit) const = 0;
};
