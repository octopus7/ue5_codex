// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CodexInvenBasicMapGameMode.generated.h"

class ACodexInvenPickupSpawner;
class ACodexInvenPickupActor;
class APawn;

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

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	TSubclassOf<ACodexInvenPickupActor> LevelStartPickupActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups", meta = (ClampMin = "0.0"))
	float LevelStartKeyDistance = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	float LevelStartGroundTraceStartOffset = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	float LevelStartGroundTraceEndOffset = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	float LevelStartSpawnHeightOffset = 50.0f;

	UPROPERTY(Transient)
	TObjectPtr<ACodexInvenPickupSpawner> RuntimePickupSpawner = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ACodexInvenPickupActor> RuntimeLevelStartKeyPickup = nullptr;

private:
	void TrySpawnPickupSpawner();
	void TrySpawnLevelStartKeyPickup();
	FVector ResolveLevelStartKeySpawnLocation(const APawn& InPlayerPawn) const;
};
