// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "CodexInvenPickupSpawner.generated.h"

class ACodexInvenPickupActor;
class APawn;
enum class ECodexInvenPickupType : uint8;

UCLASS()
class CODEXINVEN_API ACodexInvenPickupSpawner : public AActor
{
	GENERATED_BODY()

public:
	ACodexInvenPickupSpawner();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	static bool CanSpawnWithActiveCount(int32 InActiveCount, int32 InMaxActivePickupCount);
	static FVector SampleSpawnOffset2D(FRandomStream& InRandomStream, float InMinSpawnDistance, float InMaxSpawnDistance);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	TSubclassOf<ACodexInvenPickupActor> PickupActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner", meta = (ClampMin = "0.01"))
	float SpawnIntervalSeconds = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner", meta = (ClampMin = "0.0"))
	float MinSpawnDistance = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner", meta = (ClampMin = "0.0"))
	float MaxSpawnDistance = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner", meta = (ClampMin = "1"))
	int32 MaxActivePickupCount = 24;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	float GroundTraceStartOffset = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	float GroundTraceEndOffset = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	float SpawnHeightOffset = 50.0f;

private:
	void HandleSpawnTimerElapsed();
	ECodexInvenPickupType ChooseRandomPickupType();
	FVector ResolveSpawnLocation(const APawn& InPlayerPawn);
	void PruneInactivePickups();
	int32 GetActivePickupCount();

	FTimerHandle SpawnTimerHandle;
	FRandomStream RandomStream;
	TArray<TWeakObjectPtr<ACodexInvenPickupActor>> ActivePickupActors;
};
