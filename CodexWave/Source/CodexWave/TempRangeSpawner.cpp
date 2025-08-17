#include "TempRangeSpawner.h"

#include "EnemyConeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/TextRenderComponent.h"

ATempRangeSpawner::ATempRangeSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    // Default enemy class to our cone character
    EnemyClass = AEnemyConeCharacter::StaticClass();

    Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
    Arrow->ArrowColor = FColor(255, 0, 180); // magenta-ish
    Arrow->ArrowSize = 1.5f;
    SetRootComponent(Arrow);

    Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
    Label->SetupAttachment(Arrow);
    Label->SetText(FText::FromString(TEXT("범위스폰_임시")));
    Label->SetTextRenderColor(FColor::Purple);
    Label->SetHorizontalAlignment(EHTA_Center);
    Label->SetWorldSize(48.f);
    Label->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
}

void ATempRangeSpawner::BeginPlay()
{
    Super::BeginPlay();

    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ATempRangeSpawner::DoSpawn, SpawnInterval, true, SpawnInterval);
}

void ATempRangeSpawner::OnConstruction(const FTransform& Transform)
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
}

void ATempRangeSpawner::DoSpawn()
{
    if (!EnemyClass)
    {
        return;
    }
    if (SpawnedCount >= MaxSpawnCount && MaxSpawnCount > 0)
    {
        return;
    }

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
    const FVector SpawnLoc = FindSpawnLocation(CenterLoc);

    FRotator SpawnRot = FRotator::ZeroRotator;
    if (CenterActor)
    {
        SpawnRot = (CenterActor->GetActorLocation() - SpawnLoc).Rotation();
        SpawnRot.Pitch = 0.f; // keep horizontal facing
        SpawnRot.Roll = 0.f;
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    if (UWorld* World = GetWorld())
    {
        if (bDrawDebug)
        {
            DrawDebugSphere(World, SpawnLoc, 24.f, 16, FColor::Purple, false, 2.0f, 0, 2.0f);
        }

        if (AActor* Spawned = World->SpawnActor<AActor>(EnemyClass, SpawnLoc, SpawnRot, Params))
        {
            SpawnedCount++;
        }
    }
}

FVector ATempRangeSpawner::FindSpawnLocation(const FVector& Center) const
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
            FCollisionQueryParams Params(SCENE_QUERY_STAT(TempRangeSpawnerTrace), false, this);
            if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
            {
                Location = Hit.ImpactPoint + FVector(0, 0, 2.f);
            }
        }
    }

    return Location;
}
