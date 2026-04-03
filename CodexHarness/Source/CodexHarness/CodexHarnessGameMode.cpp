#include "CodexHarnessGameMode.h"

#include "CodexHarnessCharacter.h"
#include "CodexHarnessEnemyCharacter.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ACodexHarnessGameMode::ACodexHarnessGameMode()
{
}

void ACodexHarnessGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* const World = GetWorld())
	{
		World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::StartNextWave);
	}
}

void ACodexHarnessGameMode::HandlePlayerDeath(ACodexHarnessCharacter* DeadCharacter)
{
	static_cast<void>(DeadCharacter);

	bIsGameOver = true;

	if (UWorld* const World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaveTimerHandle);
		World->GetTimerManager().ClearTimer(EnemySpawnTimerHandle);
	}
}

void ACodexHarnessGameMode::HandleEnemyDeath(ACodexHarnessEnemyCharacter* DeadEnemy)
{
	static_cast<void>(DeadEnemy);

	AliveEnemyCount = FMath::Max(0, AliveEnemyCount - 1);
	RemainingEnemyCount = FMath::Max(0, RemainingEnemyCount - 1);

	if (!bIsGameOver && RemainingEnemyCount == 0 && PendingEnemySpawnCount == 0)
	{
		if (UWorld* const World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				WaveTimerHandle,
				this,
				&ThisClass::StartNextWave,
				TimeBetweenWaves,
				false);
		}
	}
}

bool ACodexHarnessGameMode::IsGameOver() const
{
	return bIsGameOver;
}

void ACodexHarnessGameMode::RequestRestart()
{
	if (!bIsGameOver)
	{
		return;
	}

	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	if (!CurrentLevelName.IsEmpty())
	{
		UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
	}
}

int32 ACodexHarnessGameMode::GetAliveEnemyCount() const
{
	return AliveEnemyCount;
}

TSubclassOf<ACodexHarnessEnemyCharacter> ACodexHarnessGameMode::GetEnemyCharacterClass() const
{
	return EnemyCharacterClass;
}

int32 ACodexHarnessGameMode::GetCurrentWave() const
{
	return CurrentWave;
}

int32 ACodexHarnessGameMode::GetRemainingEnemyCount() const
{
	return RemainingEnemyCount;
}

void ACodexHarnessGameMode::StartNextWave()
{
	if (bIsGameOver || EnemyCharacterClass == nullptr)
	{
		return;
	}

	CurrentWave += 1;
	RemainingEnemyCount = BaseEnemiesPerWave + CurrentWave - 1;
	PendingEnemySpawnCount = RemainingEnemyCount;
	WaveSpawnIndex = 0;

	SpawnNextPendingEnemy();
}

void ACodexHarnessGameMode::SpawnNextPendingEnemy()
{
	if (bIsGameOver || EnemyCharacterClass == nullptr || PendingEnemySpawnCount <= 0)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	APawn* const PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn == nullptr)
	{
		World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::SpawnNextPendingEnemy);
		return;
	}

	const float SpawnAngleDegrees = static_cast<float>(WaveSpawnIndex) * 45.0f;
	const FVector SpawnDirection = FRotator(0.0f, SpawnAngleDegrees, 0.0f).Vector();
	WaveSpawnIndex += 1;

	const FVector SpawnLocation = PlayerPawn->GetActorLocation() + (SpawnDirection * EnemySpawnDistance);
	const FRotator SpawnRotation = (PlayerPawn->GetActorLocation() - SpawnLocation).Rotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (World->SpawnActor<ACodexHarnessEnemyCharacter>(EnemyCharacterClass, SpawnLocation, SpawnRotation, SpawnParameters) != nullptr)
	{
		AliveEnemyCount += 1;
		PendingEnemySpawnCount -= 1;
	}

	if (PendingEnemySpawnCount > 0)
	{
		World->GetTimerManager().SetTimer(
			EnemySpawnTimerHandle,
			this,
			&ThisClass::SpawnNextPendingEnemy,
			TimeBetweenEnemySpawns,
			false);
	}
}
