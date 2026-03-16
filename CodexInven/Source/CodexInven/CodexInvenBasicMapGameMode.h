// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "CodexInvenBasicMapGameMode.generated.h"

class ACodexInvenPickupSpawner;
class UCodexInvenOwnershipDebugWidget;

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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	TSubclassOf<ACodexInvenPickupSpawner> PickupSpawnerClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCodexInvenOwnershipDebugWidget> OwnershipDebugWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<ACodexInvenPickupSpawner> RuntimePickupSpawner = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInvenOwnershipDebugWidget> RuntimeOwnershipDebugWidget = nullptr;

private:
	void TrySpawnPickupSpawner();
	void TryInitializeOwnershipDebugWidget();
	void HandleOwnershipWidgetInitRetry();

	FTimerHandle WidgetInitRetryTimerHandle;
};
