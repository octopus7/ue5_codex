#include "Game/CHTopDownGameMode.h"

#include "Enemy/CHEnemyCharacter.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Player/CHPlayerCharacter.h"
#include "Player/CHPlayerController.h"
#include "TimerManager.h"
#include "UI/CHGameHUD.h"

DEFINE_LOG_CATEGORY_STATIC(LogCHTopDownGameMode, Log, All);

namespace CHTopDownGameMode
{
	static FTransform ResolveFallbackSpawnTransform(const UWorld* World)
	{
		if (World)
		{
			for (TActorIterator<APlayerStart> It(World); It; ++It)
			{
				return It->GetActorTransform();
			}
		}

		return FTransform(FRotator::ZeroRotator, FVector(0.0f, 0.0f, 120.0f));
	}
}

ACHTopDownGameMode::ACHTopDownGameMode()
{
	DefaultPawnClass = ACHPlayerCharacter::StaticClass();
	PlayerControllerClass = ACHPlayerController::StaticClass();
	HUDClass = ACHGameHUD::StaticClass();
}

void ACHTopDownGameMode::StartPlay()
{
	Super::StartPlay();

	bWaveSmokeEnabled = FParse::Param(FCommandLine::Get(), TEXT("CHWaveSmoke"));
	bWaveSmokeFinished = false;
	bIsGameOver = false;
	CurrentWave = 0;
	AliveEnemyCount = 0;
	TrackedEnemies.Reset();

	StartNextWave();
}

void ACHTopDownGameMode::RestartPlayer(AController* NewPlayer)
{
	if (!IsValid(NewPlayer))
	{
		return;
	}

	Super::RestartPlayer(NewPlayer);

	if (APawn* ExistingPawn = NewPlayer->GetPawn())
	{
		UE_LOG(LogCHTopDownGameMode, Display, TEXT("플레이어 스폰 완료: %s"), *GetNameSafe(ExistingPawn));
		return;
	}

	const FTransform FallbackSpawnTransform = CHTopDownGameMode::ResolveFallbackSpawnTransform(GetWorld());
	if (APawn* SpawnedPawn = SpawnDefaultPawnAtTransform(NewPlayer, FallbackSpawnTransform))
	{
		NewPlayer->Possess(SpawnedPawn);
		SetPlayerDefaults(SpawnedPawn);
		UE_LOG(LogCHTopDownGameMode, Display, TEXT("폴백 스폰으로 플레이어를 생성했습니다: %s"), *GetNameSafe(SpawnedPawn));
		return;
	}

	UE_LOG(LogCHTopDownGameMode, Error, TEXT("플레이어 폰 스폰에 실패했습니다."));
}

void ACHTopDownGameMode::StartNextWave()
{
	UWorld* const World = GetWorld();
	if (!World || bIsGameOver)
	{
		return;
	}

	APawn* const PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		World->GetTimerManager().SetTimerForNextTick(this, &ACHTopDownGameMode::StartNextWave);
		return;
	}

	++CurrentWave;
	TrackedEnemies.Reset();
	AliveEnemyCount = 0;

	const int32 EnemiesToSpawn = GetEnemyCountForWave(CurrentWave);
	for (int32 EnemyIndex = 0; EnemyIndex < EnemiesToSpawn; ++EnemyIndex)
	{
		const FVector SpawnLocation = PlayerPawn->GetActorLocation()
			+ EnemySpawnOffsetBase
			+ FVector(0.0f, EnemySpawnSpacing * EnemyIndex, 0.0f);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (ACHEnemyCharacter* const SpawnedEnemy = World->SpawnActor<ACHEnemyCharacter>(ACHEnemyCharacter::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParameters))
		{
			SpawnedEnemy->OnDestroyed.AddDynamic(this, &ACHTopDownGameMode::HandleTrackedEnemyDestroyed);
			TrackedEnemies.Add(SpawnedEnemy);
			++AliveEnemyCount;
			UE_LOG(LogCHTopDownGameMode, Display, TEXT("CHEnemySpawned Wave=%d Index=%d Location=%s"), CurrentWave, EnemyIndex, *SpawnLocation.ToCompactString());
		}
	}

	if (AliveEnemyCount <= 0)
	{
		UE_LOG(LogCHTopDownGameMode, Error, TEXT("웨이브 %d 적 스폰에 실패했습니다."), CurrentWave);
		return;
	}

	UE_LOG(LogCHTopDownGameMode, Display, TEXT("CHWaveStarted Wave=%d SpawnCount=%d Alive=%d"), CurrentWave, EnemiesToSpawn, AliveEnemyCount);

	if (bWaveSmokeEnabled)
	{
		World->GetTimerManager().SetTimerForNextTick(this, &ACHTopDownGameMode::RunWaveSmokeStep);
	}
}

void ACHTopDownGameMode::RunWaveSmokeStep()
{
	if (!bWaveSmokeEnabled || bWaveSmokeFinished)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	if (CurrentWave >= 2)
	{
		bWaveSmokeFinished = true;
		if (APlayerController* const PlayerController = UGameplayStatics::GetPlayerController(this, 0))
		{
			PlayerController->ConsoleCommand(TEXT("quit"));
		}
		return;
	}

	for (ACHEnemyCharacter* const TrackedEnemy : TrackedEnemies)
	{
		if (IsValid(TrackedEnemy))
		{
			UGameplayStatics::ApplyDamage(TrackedEnemy, 999.0f, nullptr, this, UDamageType::StaticClass());
		}
	}
}

int32 ACHTopDownGameMode::GetEnemyCountForWave(const int32 WaveIndex) const
{
	return FMath::Max(1, WaveIndex);
}

void ACHTopDownGameMode::HandleTrackedEnemyDestroyed(AActor* DestroyedActor)
{
	TrackedEnemies.Remove(Cast<ACHEnemyCharacter>(DestroyedActor));
	AliveEnemyCount = FMath::Max(AliveEnemyCount - 1, 0);
	UE_LOG(LogCHTopDownGameMode, Display, TEXT("CHEnemyCount Wave=%d Alive=%d"), CurrentWave, AliveEnemyCount);

	if (AliveEnemyCount <= 0)
	{
		if (UWorld* const World = GetWorld())
		{
			World->GetTimerManager().SetTimerForNextTick(this, &ACHTopDownGameMode::StartNextWave);
		}
	}
}

void ACHTopDownGameMode::NotifyPlayerDied()
{
	if (bIsGameOver)
	{
		return;
	}

	bIsGameOver = true;
	UE_LOG(LogCHTopDownGameMode, Display, TEXT("CHGameOverState=1"));
}
