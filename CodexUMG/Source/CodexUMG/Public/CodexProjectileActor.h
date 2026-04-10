// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CodexProjectileActor.generated.h"

class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class CODEXUMG_API ACodexProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	ACodexProjectileActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Codex|Projectile")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Codex|Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
};
