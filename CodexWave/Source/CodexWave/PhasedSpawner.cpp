#include "PhasedSpawner.h"

#include "Components/ArrowComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

APhasedSpawner::APhasedSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
    Arrow->ArrowColor = FColor(0, 200, 255);
    Arrow->ArrowSize = 1.5f;
    SetRootComponent(Arrow);

    Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
    Label->SetupAttachment(Arrow);
    Label->SetText(FText::FromString(TEXT("Phased Spawner")));
    Label->SetTextRenderColor(FColor::Cyan);
    Label->SetHorizontalAlignment(EHTA_Center);
    Label->SetWorldSize(48.f);
    Label->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
}

void APhasedSpawner::BeginPlay()
{
    Super::BeginPlay();

    // Double the label size at begin time
    if (Label)
    {
        Label->SetWorldSize(Label->WorldSize * 2.0f);
    }

    if (bAutoStart)
    {
        StartPhases();
    }
}

void APhasedSpawner::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (Label)
    {
        Label->SetHiddenInGame(!bShowLabelInGame);
    }

    if (bDrawDebug)
    {
        if (UWorld* World = GetWorld())
        {
            const FVector C = GetActorLocation();
            const FColor MinColor = FColor(0, 200, 255);
            const FColor MaxColor = FColor(0, 120, 255);
            DrawDebugCircle(World, C, MinDistance, 64, MinColor, false, 2.0f, 0, 2.0f, FVector(1,0,0), FVector(0,1,0), false);
            DrawDebugCircle(World, C, MaxDistance, 64, MaxColor, false, 2.0f, 0, 2.0f, FVector(1,0,0), FVector(0,1,0), false);
        }
    }

    UpdateLabelText();
}

void APhasedSpawner::StartPhases()
{
    if (Phases.Num() <= 0)
    {
        CurrentPhaseIndex = INDEX_NONE;
        AliveThisPhase = 0;
        UpdateLabelText();
        return;
    }
    StartSpecificPhase(0);
}

void APhasedSpawner::StartSpecificPhase(int32 PhaseIndex)
{
    if (!ensure(PhaseIndex >= 0 && PhaseIndex < Phases.Num()))
    {
        return;
    }
    CurrentPhaseIndex = PhaseIndex;
    AliveThisPhase = 0;
    UpdateLabelText();

    OnPhaseStarted.Broadcast(CurrentPhaseIndex);
    SpawnPhase(Phases[CurrentPhaseIndex]);

    // 스폰이 0건이면 즉시 다음 페이즈로 진행
    if (AliveThisPhase <= 0)
    {
        StartNextPhaseInternal();
    }
}

void APhasedSpawner::StartNextPhaseInternal()
{
    int32 NextIndex = (CurrentPhaseIndex == INDEX_NONE) ? 0 : (CurrentPhaseIndex + 1);
    const bool bDone = (Phases.Num() <= 0) || (NextIndex >= Phases.Num());
    if (bDone)
    {
        if (bLoopPhases && Phases.Num() > 0)
        {
            StartSpecificPhase(0);
        }
        else
        {
            OnAllPhasesCompleted.Broadcast();
            if (bDestroyOnComplete)
            {
                Destroy();
            }
            else
            {
                // 완료 상태 표시만 갱신
                CurrentPhaseIndex = INDEX_NONE;
                AliveThisPhase = 0;
                UpdateLabelText();
            }
        }
        return;
    }
    StartSpecificPhase(NextIndex);
}

void APhasedSpawner::SpawnPhase(const FSpawnPhase& Phase)
{
    // 기준 위치 결정 (플레이어/타겟/자기자신)
    AActor* CenterActor = nullptr;
    if (bSpawnAroundPlayer)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            CenterActor = PC->GetPawn();
        }
    }
    else
    {
        CenterActor = TargetActor ? TargetActor : this;
    }
    const FVector CenterLoc = CenterActor ? CenterActor->GetActorLocation() : GetActorLocation();

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    for (const FPhaseSpawnEntry& Entry : Phase.Entries)
    {
        if (!Entry.EnemyClass || Entry.Count <= 0)
        {
            continue;
        }

        for (int32 i = 0; i < Entry.Count; ++i)
        {
            const FVector SpawnLoc = FindSpawnLocation(CenterLoc);

            FRotator SpawnRot = FRotator::ZeroRotator;
            if (CenterActor)
            {
                SpawnRot = (CenterLoc - SpawnLoc).Rotation();
                SpawnRot.Pitch = 0.f;
                SpawnRot.Roll = 0.f;
            }

            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            if (bDrawDebug)
            {
                DrawDebugSphere(World, SpawnLoc, 24.f, 16, FColor::Cyan, false, 2.0f, 0, 2.0f);
            }

            if (AActor* Spawned = World->SpawnActor<AActor>(Entry.EnemyClass, SpawnLoc, SpawnRot, Params))
            {
                ++AliveThisPhase;
                // 파괴 시 카운트 감소
                Spawned->OnDestroyed.AddDynamic(this, &APhasedSpawner::HandleSpawnedDestroyed);
            }
        }
    }

    UpdateLabelText();
}

void APhasedSpawner::HandleSpawnedDestroyed(AActor* DestroyedActor)
{
    AliveThisPhase = FMath::Max(0, AliveThisPhase - 1);
    UpdateLabelText();

    if (AliveThisPhase <= 0)
    {
        StartNextPhaseInternal();
    }
}

FVector APhasedSpawner::FindSpawnLocation(const FVector& Center) const
{
    const float R = FMath::FRandRange(MinDistance, MaxDistance);
    const float Theta = FMath::FRandRange(0.f, 2 * PI);
    FVector Offset(FMath::Cos(Theta) * R, FMath::Sin(Theta) * R, 0.f);

    FVector Location = Center + Offset;

    if (bAlignToGround)
    {
        if (UWorld* World = GetWorld())
        {
            FVector Start = Location + FVector(0, 0, 1000.f);
            FVector End = Location - FVector(0, 0, 2000.f);
            FHitResult Hit;
            FCollisionQueryParams Params(SCENE_QUERY_STAT(PhasedSpawnerTrace), false, this);
            if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
            {
                Location = Hit.ImpactPoint + FVector(0, 0, 2.f);
            }
        }
    }

    return Location;
}

void APhasedSpawner::UpdateLabelText()
{
    if (!Label)
    {
        return;
    }

    if (CurrentPhaseIndex == INDEX_NONE)
    {
        Label->SetText(FText::FromString(TEXT("Phased Spawner (Idle/Completed)")));
        return;
    }
    const int32 TotalPhases = Phases.Num();
    const FString Txt = FString::Printf(TEXT("Phase %d/%d | Alive %d"), CurrentPhaseIndex + 1, TotalPhases, AliveThisPhase);
    Label->SetText(FText::FromString(Txt));
}
