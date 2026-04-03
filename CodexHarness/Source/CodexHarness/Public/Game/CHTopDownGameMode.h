#pragma once

#include "GameFramework/GameModeBase.h"

#include "CHTopDownGameMode.generated.h"

class ACHEnemyCharacter;

UCLASS()
class CODEXHARNESS_API ACHTopDownGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACHTopDownGameMode();

	virtual void StartPlay() override;
	virtual void RestartPlayer(AController* NewPlayer) override;

	void NotifyPlayerDied();

	FORCEINLINE int32 GetCurrentWave() const { return CurrentWave; }
	FORCEINLINE int32 GetAliveEnemyCount() const { return AliveEnemyCount; }
	FORCEINLINE bool IsGameOver() const { return bIsGameOver; }

private:
	void StartNextWave();
	void RunWaveSmokeStep();
	int32 GetEnemyCountForWave(int32 WaveIndex) const;

	UFUNCTION()
	void HandleTrackedEnemyDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	FVector EnemySpawnOffsetBase = FVector(400.0f, 400.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	float EnemySpawnSpacing = 160.0f;

	UPROPERTY(Transient)
	TArray<TObjectPtr<ACHEnemyCharacter>> TrackedEnemies;

	bool bWaveSmokeEnabled = false;
	bool bWaveSmokeFinished = false;
	bool bIsGameOver = false;
	int32 CurrentWave = 0;
	int32 AliveEnemyCount = 0;
};
