#include "GameModes/PSVGameMode.h"

#include "Characters/PSVEnemyCharacter.h"
#include "Characters/PSVPlayerCharacter.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"
#include "UI/PSVHUD.h"

DEFINE_LOG_CATEGORY_STATIC(LogPSVSpawner, Log, All);

APSVGameMode::APSVGameMode()
{
    DefaultPawnClass = APSVPlayerCharacter::StaticClass();
    HUDClass = APSVHUD::StaticClass();
    DefaultEnemyClass = APSVEnemyCharacter::StaticClass();
}

void APSVGameMode::BeginPlay()
{
    Super::BeginPlay();

    InitializeSpawner();
}

void APSVGameMode::InitializeSpawner()
{
    if (!DefaultEnemyClass)
    {
        DefaultEnemyClass = APSVEnemyCharacter::StaticClass();
    }

    BuildDefaultSpawnScript(SpawnScript);
    ScheduleSpawnCommands();
}

void APSVGameMode::BuildDefaultSpawnScript(TArray<FPSVSpawnPatternCommand>& OutCommands) const
{
    OutCommands.Reset();

    // 간단한 기본 패턴: 초반 소수 리듬 → 간격 축소 후 물량 증가
    OutCommands.Emplace(0.f, 6, 600.f);
    OutCommands.Emplace(5.f, 10, 750.f, 0.2f);
    OutCommands.Emplace(12.f, 12, 850.f, 0.15f);
    OutCommands.Emplace(20.f, 16, 900.f, 0.08f);
}

void APSVGameMode::ScheduleSpawnCommands()
{
    if (!GetWorld())
    {
        return;
    }

    for (const FPSVSpawnPatternCommand& Command : SpawnScript)
    {
        if (Command.TimeOffset <= KINDA_SMALL_NUMBER)
        {
            ExecuteSpawnCommand(Command);
            continue;
        }

        FTimerDelegate SpawnDelegate;
        SpawnDelegate.BindUObject(this, &APSVGameMode::ExecuteSpawnCommand, Command);

        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, SpawnDelegate, Command.TimeOffset, false);
    }
}

void APSVGameMode::ExecuteSpawnCommand(FPSVSpawnPatternCommand Command)
{
    const int32 SpawnCount = FMath::Max(0, Command.NumEnemies);
    if (SpawnCount <= 0)
    {
        return;
    }

    for (int32 Index = 0; Index < SpawnCount; ++Index)
    {
        const float Delay = Command.IntervalBetweenSpawns * Index;

        if (Delay <= KINDA_SMALL_NUMBER)
        {
            SpawnEnemyFromCommand(Command, Index);
            continue;
        }

        FTimerDelegate SpawnDelegate;
        SpawnDelegate.BindUObject(this, &APSVGameMode::SpawnEnemyFromCommand, Command, Index);

        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, SpawnDelegate, Delay, false);
    }
}

void APSVGameMode::SpawnEnemyFromCommand(FPSVSpawnPatternCommand Command, int32 SequenceIndex)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    TSubclassOf<APSVEnemyCharacter> EnemyClassToSpawn = Command.EnemyClass ? Command.EnemyClass : DefaultEnemyClass;
    if (!EnemyClassToSpawn)
    {
        UE_LOG(LogPSVSpawner, Warning, TEXT("적 스폰이 건너뛰어졌습니다. EnemyClass가 지정되지 않았습니다."));
        return;
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
    const bool bHasPlayer = PlayerPawn != nullptr;
    const FVector AnchorLocation = bHasPlayer ? PlayerPawn->GetActorLocation() : FVector::ZeroVector;

    const FVector SpawnLocation = CalculateSpawnLocation(Command, SequenceIndex, AnchorLocation, bHasPlayer);
    FRotator SpawnRotation = FRotator::ZeroRotator;

    if (bHasPlayer)
    {
        SpawnRotation = (AnchorLocation - SpawnLocation).Rotation();
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APSVEnemyCharacter* SpawnedEnemy = World->SpawnActor<APSVEnemyCharacter>(EnemyClassToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
    if (!SpawnedEnemy)
    {
        UE_LOG(LogPSVSpawner, Warning, TEXT("적 스폰 실패: %s"), *EnemyClassToSpawn->GetName());
    }
}

FVector APSVGameMode::CalculateSpawnLocation(const FPSVSpawnPatternCommand& Command, int32 SequenceIndex, const FVector& AnchorLocation, bool bHasValidAnchor) const
{
    const float Radius = Command.SpawnRadius > KINDA_SMALL_NUMBER ? Command.SpawnRadius : DefaultSpawnRadius;

    float AngleDegrees = FMath::FRandRange(0.f, 360.f);
    if (Command.NumEnemies > 1)
    {
        const float ArcStep = 360.f / static_cast<float>(Command.NumEnemies);
        AngleDegrees = ArcStep * SequenceIndex + FMath::FRandRange(0.f, ArcStep * 0.5f);
    }

    const float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
    const FVector Offset(FMath::Cos(AngleRadians), FMath::Sin(AngleRadians), 0.f);

    FVector SpawnLocation = (bHasValidAnchor ? AnchorLocation : FVector::ZeroVector) + Offset * Radius;
    const float AnchorZ = bHasValidAnchor ? AnchorLocation.Z : 0.f;
    SpawnLocation.Z = AnchorZ + SpawnHeightOffset;

    return SpawnLocation;
}
