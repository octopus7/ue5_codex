#include "TempRangeSpawner.h"

#include "EnemyConeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ATempRangeSpawner::ATempRangeSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    // Default enemy class to our cone character
    EnemyClass = AEnemyConeCharacter::StaticClass();
}

void ATempRangeSpawner::BeginPlay()
{
    Super::BeginPlay();

    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ATempRangeSpawner::DoSpawn, SpawnInterval, true, SpawnInterval);
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

