#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhasedSpawner.generated.h"

class UArrowComponent;
class UTextRenderComponent;

USTRUCT(BlueprintType)
struct FPhaseSpawnEntry
{
    GENERATED_BODY()

    // 스폰할 적 클래스와 수량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
    TSubclassOf<AActor> EnemyClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn", meta=(ClampMin="0"))
    int32 Count = 0;
};

USTRUCT(BlueprintType)
struct FSpawnPhase
{
    GENERATED_BODY()

    // 페이즈 이름(에디터 표기용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Phase")
    FName PhaseName = NAME_None;

    // 이 페이즈에서 동시에 스폰할 묶음들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Phase")
    TArray<FPhaseSpawnEntry> Entries;
};

// 단계별 그룹 스폰: 각 페이즈의 적을 동시 스폰, 모두 처치 시 다음 페이즈 진행
UCLASS(meta=(DisplayName="단계스폰_PhasedSpawner"))
class CODEXWAVE_API APhasedSpawner : public AActor
{
    GENERATED_BODY()

public:
    APhasedSpawner();

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

    // 페이즈 구성
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Phases")
    TArray<FSpawnPhase> Phases;

    UPROPERTY(EditAnywhere, Category="Phases")
    bool bAutoStart = true;

    UPROPERTY(EditAnywhere, Category="Phases")
    bool bLoopPhases = false;

    UPROPERTY(EditAnywhere, Category="Phases")
    bool bDestroyOnComplete = false;

    // 범위/대상 설정 (TempRangeSpawner와 유사)
    UPROPERTY(EditAnywhere, Category="Spawn|Range")
    bool bSpawnAroundPlayer = true;

    UPROPERTY(EditAnywhere, Category="Spawn|Range", meta=(EditCondition="!bSpawnAroundPlayer"))
    AActor* TargetActor = nullptr;

    UPROPERTY(EditAnywhere, Category="Spawn|Range", meta=(ClampMin="0.0"))
    float MinDistance = 500.f;

    UPROPERTY(EditAnywhere, Category="Spawn|Range", meta=(ClampMin="0.0"))
    float MaxDistance = 1500.f;

    UPROPERTY(EditAnywhere, Category="Spawn|Range")
    bool bAlignToGround = true;

    // 디버그/표시
    UPROPERTY(EditAnywhere, Category="Debug")
    bool bDrawDebug = true;

    UPROPERTY(EditAnywhere, Category="Debug")
    bool bShowLabelInGame = true;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
    UArrowComponent* Arrow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Debug")
    UTextRenderComponent* Label;

    // 진행 상태
    UPROPERTY(VisibleInstanceOnly, Category="State")
    int32 CurrentPhaseIndex = INDEX_NONE;

    UPROPERTY(VisibleInstanceOnly, Category="State")
    int32 AliveThisPhase = 0;

    // 블루프린트에서 바인딩 가능한 이벤트
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseStarted, int32, PhaseIndex);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPhasesCompleted);

    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnPhaseStarted OnPhaseStarted;

    UPROPERTY(BlueprintAssignable, Category="Events")
    FOnAllPhasesCompleted OnAllPhasesCompleted;

public:
    UFUNCTION(BlueprintCallable, Category="Phases")
    void StartPhases();

    UFUNCTION(BlueprintCallable, Category="Phases")
    void StartSpecificPhase(int32 PhaseIndex);

protected:
    void StartNextPhaseInternal();
    void SpawnPhase(const FSpawnPhase& Phase);
    void UpdateLabelText();
    FVector FindSpawnLocation(const FVector& Center) const;

    UFUNCTION()
    void HandleSpawnedDestroyed(AActor* DestroyedActor);
};

