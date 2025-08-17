#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TempRangeSpawner.generated.h"

// 범위 스폰 (임시): 일정 시간마다 대상 주변의 반경 내에 스폰
UCLASS(meta=(DisplayName="범위스폰_임시(TempRangeSpawner)"))
class CODEXWAVE_API ATempRangeSpawner : public AActor
{
    GENERATED_BODY()

public:
    ATempRangeSpawner();

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

    UPROPERTY(EditAnywhere, Category="Spawn|Class")
    TSubclassOf<AActor> EnemyClass;

    UPROPERTY(EditAnywhere, Category="Spawn|Timing")
    float SpawnInterval = 2.0f;

    UPROPERTY(EditAnywhere, Category="Spawn|Timing")
    int32 MaxSpawnCount = 20;

    UPROPERTY(EditAnywhere, Category="Spawn|Range")
    bool bSpawnAroundPlayer = true;

    UPROPERTY(EditAnywhere, Category="Spawn|Range", meta=(EditCondition="!bSpawnAroundPlayer"))
    AActor* TargetActor = nullptr;

    UPROPERTY(EditAnywhere, Category="Spawn|Range")
    float MinDistance = 500.f;

    UPROPERTY(EditAnywhere, Category="Spawn|Range")
    float MaxDistance = 1500.f;

    UPROPERTY(EditAnywhere, Category="Spawn|Range")
    bool bAlignToGround = true;

    UPROPERTY(EditAnywhere, Category="Debug")
    bool bDrawDebug = true;

    // Visual helpers to confirm placement in level
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
    class UArrowComponent* Arrow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
    class UTextRenderComponent* Label;

    UPROPERTY(EditAnywhere, Category="Debug")
    bool bShowLabelInGame = true;

private:
    FTimerHandle SpawnTimerHandle;
    int32 SpawnedCount = 0;

    void DoSpawn();
    FVector FindSpawnLocation(const FVector& Center) const;
};
