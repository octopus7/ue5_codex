// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CodexProjectileConfigDataAsset.generated.h"

class ACodexProjectileActor;

UCLASS(BlueprintType)
class CODEXUMG_API UCodexProjectileConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Projectile")
	TSubclassOf<ACodexProjectileActor> ProjectileClass;

	TSubclassOf<ACodexProjectileActor> GetProjectileClass() const
	{
		return ProjectileClass;
	}
};
