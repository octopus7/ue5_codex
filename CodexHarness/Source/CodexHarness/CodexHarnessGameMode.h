#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "CodexHarnessGameMode.generated.h"

class ACodexHarnessCharacter;
class ACodexHarnessEnemyCharacter;

UCLASS()
class CODEXHARNESS_API ACodexHarnessGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACodexHarnessGameMode();

	virtual void BeginPlay() override;

	void HandlePlayerDeath(ACodexHarnessCharacter* DeadCharacter);
	void HandleEnemyDeath(ACodexHarnessEnemyCharacter* DeadEnemy);

	UFUNCTION(BlueprintPure, Category = "Flow")
	bool IsGameOver() const;

	void RequestRestart();

	UFUNCTION(BlueprintPure, Category = "Enemy")
	int32 GetAliveEnemyCount() const;

	UFUNCTION(BlueprintPure, Category = "Enemy")
	TSubclassOf<ACodexHarnessEnemyCharacter> GetEnemyCharacterClass() const;

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetCurrentWave() const;

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetRemainingEnemyCount() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	TSubclassOf<ACodexHarnessEnemyCharacter> EnemyCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy")
	float EnemySpawnDistance = 900.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave")
	int32 BaseEnemiesPerWave = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave")
	float TimeBetweenWaves = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave")
	float TimeBetweenEnemySpawns = 0.2f;

private:
	void StartNextWave();
	void SpawnNextPendingEnemy();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Flow", meta = (AllowPrivateAccess = "true"))
	bool bIsGameOver = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy", meta = (AllowPrivateAccess = "true"))
	int32 AliveEnemyCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Wave", meta = (AllowPrivateAccess = "true"))
	int32 CurrentWave = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Wave", meta = (AllowPrivateAccess = "true"))
	int32 RemainingEnemyCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Wave", meta = (AllowPrivateAccess = "true"))
	int32 PendingEnemySpawnCount = 0;

	int32 WaveSpawnIndex = 0;
	FTimerHandle WaveTimerHandle;
	FTimerHandle EnemySpawnTimerHandle;
};
