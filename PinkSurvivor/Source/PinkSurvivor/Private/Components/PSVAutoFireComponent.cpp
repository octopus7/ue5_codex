#include "Components/PSVAutoFireComponent.h"

#include "Engine/World.h"
#include "Projectiles/PSVProjectile.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
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

    const FVector SpawnLocation = OwnerActor->GetActorLocation()
        + OwnerActor->GetActorForwardVector() * SpawnOffset.X
        + OwnerActor->GetActorRightVector() * SpawnOffset.Y
        + OwnerActor->GetActorUpVector() * SpawnOffset.Z;

    const FVector ForwardDirection = OwnerActor->GetActorForwardVector();
    if (ForwardDirection.IsNearlyZero())
    {
        return;
    }

    const FRotator SpawnRotation = ForwardDirection.Rotation();

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = OwnerActor;
    SpawnParameters.Instigator = OwnerActor->GetInstigator();
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APSVProjectile* Projectile = World->SpawnActor<APSVProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters);
    if (!Projectile)
    {
        return;
    }

    if (UProjectileMovementComponent* Movement = Projectile->GetProjectileMovement())
    {
        const float DesiredSpeed = ProjectileSpeedOverride > 0.f ? ProjectileSpeedOverride : Movement->InitialSpeed;
        Movement->Velocity = SpawnRotation.Vector() * DesiredSpeed;
    }
}
