// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CodexInvenBasicMapGameMode.generated.h"

class ACodexInvenPickupSpawner;

/**
 * 
 */
UCLASS()
class CODEXINVEN_API ACodexInvenBasicMapGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACodexInvenBasicMapGameMode();

	virtual void StartPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	TSubclassOf<ACodexInvenPickupSpawner> PickupSpawnerClass;

	UPROPERTY(Transient)
	TObjectPtr<ACodexInvenPickupSpawner> RuntimePickupSpawner = nullptr;
};
