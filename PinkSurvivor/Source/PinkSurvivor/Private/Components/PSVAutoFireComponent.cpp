#include "Components/PSVAutoFireComponent.h"

#include "Engine/World.h"
#include "Characters/PSVPlayerCharacter.h"
#include "Projectiles/PSVProjectile.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"

UPSVAutoFireComponent::UPSVAutoFireComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UPSVAutoFireComponent::BeginPlay()
{
    Super::BeginPlay();

    if (bStartOnBeginPlay)
    {
        StartFiring();
    }
}

void UPSVAutoFireComponent::StartFiring()
{
    if (!ProjectileClass || FireInterval <= 0.f)
    {
        return;
    }

    if (UWorld* World = GetWorld())
    {
        HandleFire();
        World->GetTimerManager().SetTimer(FireTimerHandle, this, &UPSVAutoFireComponent::HandleFire, FireInterval, true, FireInterval);
    }
}

void UPSVAutoFireComponent::StopFiring()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(FireTimerHandle);
    }
}

void UPSVAutoFireComponent::HandleFire()
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor || !ProjectileClass)
    {
        return;
    }

    UWorld* World = OwnerActor->GetWorld();
    if (!World)
    {
        return;
    }

    const FVector ForwardDirection = OwnerActor->GetActorForwardVector();
    if (ForwardDirection.IsNearlyZero())
    {
        return;
    }

    const FVector UpVector = OwnerActor->GetActorUpVector();
    const FRotator BaseRotation = ForwardDirection.Rotation();

    bool bUseEightWayFire = false;
    if (const APSVPlayerCharacter* PlayerCharacter = Cast<APSVPlayerCharacter>(OwnerActor))
    {
        bUseEightWayFire = PlayerCharacter->IsEightWayFireEnabled();
    }

    TArray<FRotator> SpawnRotations;
    if (bUseEightWayFire)
    {
        SpawnRotations.Reserve(8);
        for (int32 Step = 0; Step < 8; ++Step)
        {
            const float AdditionalYaw = 45.f * static_cast<float>(Step);
            SpawnRotations.Add(BaseRotation + FRotator(0.f, AdditionalYaw, 0.f));
        }
    }
    else
    {
        SpawnRotations.Add(BaseRotation);
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = OwnerActor;
    SpawnParameters.Instigator = Cast<APawn>(OwnerActor);
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    for (const FRotator& SpawnRotation : SpawnRotations)
    {
        const FVector Direction = SpawnRotation.Vector().GetSafeNormal();
        if (Direction.IsNearlyZero())
        {
            continue;
        }

        FVector RightVector = FVector::CrossProduct(UpVector, Direction).GetSafeNormal();
        if (RightVector.IsNearlyZero())
        {
            RightVector = OwnerActor->GetActorRightVector();
        }

        const FVector SpawnLocation = OwnerActor->GetActorLocation()
            + Direction * SpawnOffset.X
            + RightVector * SpawnOffset.Y
            + UpVector * SpawnOffset.Z;

        APSVProjectile* Projectile = World->SpawnActor<APSVProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters);
        if (!Projectile)
        {
            continue;
        }

        if (UProjectileMovementComponent* Movement = Projectile->GetProjectileMovement())
        {
            const float DesiredSpeed = ProjectileSpeedOverride > 0.f ? ProjectileSpeedOverride : Movement->InitialSpeed;
            Movement->Velocity = Direction * DesiredSpeed;
        }
    }
}
