#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PSVGameMode.generated.h"

class APSVEnemyCharacter;

USTRUCT(BlueprintType)
struct FPSVSpawnPatternCommand
{
    GENERATED_BODY()

    FPSVSpawnPatternCommand() = default;

    FPSVSpawnPatternCommand(float InTimeOffset, int32 InNumEnemies, float InSpawnRadius, float InInterval = 0.f, TSubclassOf<APSVEnemyCharacter> InEnemyClass = nullptr)
        : TimeOffset(InTimeOffset)
        , NumEnemies(InNumEnemies)
        , SpawnRadius(InSpawnRadius)
        , IntervalBetweenSpawns(InInterval)
        , EnemyClass(InEnemyClass)
    {
    }

    UPROPERTY(EditAnywhere, Category="Spawning")
    float TimeOffset = 0.f;

    UPROPERTY(EditAnywhere, Category="Spawning")
    int32 NumEnemies = 1;

    UPROPERTY(EditAnywhere, Category="Spawning")
    float SpawnRadius = 600.f;

    UPROPERTY(EditAnywhere, Category="Spawning")
    float IntervalBetweenSpawns = 0.f;

    UPROPERTY(EditAnywhere, Category="Spawning")
    TSubclassOf<APSVEnemyCharacter> EnemyClass;
};

UCLASS()
class PINKSURVIVOR_API APSVGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    APSVGameMode();

protected:
    virtual void BeginPlay() override;

    void InitializeSpawner();
    void BuildDefaultSpawnScript(TArray<FPSVSpawnPatternCommand>& OutCommands) const;
    void ScheduleSpawnCommands();
    void ExecuteSpawnCommand(FPSVSpawnPatternCommand Command);
    void SpawnEnemyFromCommand(FPSVSpawnPatternCommand Command, int32 SequenceIndex);
    FVector CalculateSpawnLocation(const FPSVSpawnPatternCommand& Command, int32 SequenceIndex, const FVector& AnchorLocation, bool bHasValidAnchor) const;

    UPROPERTY(EditDefaultsOnly, Category="Spawning")
    TSubclassOf<APSVEnemyCharacter> DefaultEnemyClass;

    UPROPERTY(EditDefaultsOnly, Category="Spawning", meta=(ClampMin="0.0"))
    float DefaultSpawnRadius = 800.f;

    UPROPERTY(EditDefaultsOnly, Category="Spawning", meta=(ClampMin="0.0"))
    float SpawnHeightOffset = 50.f;

    UPROPERTY()
    TArray<FPSVSpawnPatternCommand> SpawnScript;
};
