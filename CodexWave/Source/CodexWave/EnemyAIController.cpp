#include "EnemyAIController.h"

#include "EnemyConeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

AEnemyAIController::AEnemyAIController()
{
    bAttachToPawn = true;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    CachedEnemy = Cast<AEnemyConeCharacter>(InPawn);
    CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);

    if (UCharacterMovementComponent* MoveComp = InPawn ? InPawn->FindComponentByClass<UCharacterMovementComponent>() : nullptr)
    {
        MoveComp->bOrientRotationToMovement = true;
        MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
    }

    // Start brain
    StartIdle(FMath::FRandRange(IdleTimeMin, IdleTimeMax));
    GetWorldTimerManager().SetTimer(ThinkTimerHandle, this, &AEnemyAIController::Think, 0.2f, true, 0.2f);
}

bool AEnemyAIController::CanSeePlayer(bool bStrict) const
{
    const APawn* Player = CachedPlayer.Get();
    const APawn* SelfPawn = GetPawn();
    if (!Player || !SelfPawn)
    {
        return false;
    }

    const FVector SelfLoc = SelfPawn->GetActorLocation();
    const FVector PlayerLoc = Player->GetActorLocation();
    const FVector ToPlayer = PlayerLoc - SelfLoc;
    const float Dist = ToPlayer.Size();
    if (bStrict)
    {
        if (Dist > SightRadius)
            return false;
    }
    else
    {
        if (Dist > LoseSightRadius)
            return false;
    }

    const FVector Forward = SelfPawn->GetActorForwardVector();
    const FVector Dir = ToPlayer.GetSafeNormal2D();
    const float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(FOVDegrees * 0.5f));
    const float Dot = FVector::DotProduct(Forward.GetSafeNormal2D(), Dir);
    if (Dot < CosHalfFOV)
    {
        return false;
    }

    // Optional: visibility trace
    FHitResult Hit;
    FCollisionQueryParams P(SCENE_QUERY_STAT(EnemySight), false, SelfPawn);
    P.AddIgnoredActor(SelfPawn);
    const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, SelfLoc + FVector(0,0,50), PlayerLoc + FVector(0,0,50), ECC_Visibility, P);
    if (bBlocked && Hit.GetActor() != Player)
    {
        return false;
    }
    return true;
}

bool AEnemyAIController::IsPlayerInAttackRange() const
{
    const APawn* Player = CachedPlayer.Get();
    const APawn* SelfPawn = GetPawn();
    if (!Player || !SelfPawn)
    {
        return false;
    }
    return FVector::Dist2D(Player->GetActorLocation(), SelfPawn->GetActorLocation()) <= AttackRange;
}

void AEnemyAIController::Think()
{
    // Evaluate player visibility
    const bool bSee = CanSeePlayer(true);
    const bool bKeepChase = CanSeePlayer(false);

    switch (State)
    {
    case EEnemyState::Chase:
        if (!bKeepChase)
        {
            StartIdle(FMath::FRandRange(IdleTimeMin, IdleTimeMax));
            return;
        }
        // Refresh chase target
        StartChase();
        break;
    case EEnemyState::Idle:
    case EEnemyState::Patrol:
    default:
        if (bSee)
        {
            StartChase();
            return;
        }
        break;
    }
}

void AEnemyAIController::StartIdle(float Duration)
{
    StopMovement();
    State = EEnemyState::Idle;
    GetWorldTimerManager().ClearTimer(IdleTimerHandle);
    GetWorldTimerManager().SetTimer(IdleTimerHandle, this, &AEnemyAIController::StartPatrol, Duration, false);

    if (bDrawDebug)
    {
        if (UWorld* World = GetWorld())
        {
            DrawDebugString(World, GetPawn()->GetActorLocation() + FVector(0,0,120), TEXT("Idle"), nullptr, FColor::White, 1.0f);
        }
    }
}

void AEnemyAIController::StartPatrol()
{
    if (!GetPawn()) return;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
    if (!NavSys) return;

    FNavLocation OutLoc;
    const FVector Origin = GetPawn()->GetActorLocation();
    if (NavSys->GetRandomReachablePointInRadius(Origin, PatrolRadius, OutLoc))
    {
        State = EEnemyState::Patrol;
        MoveToLocation(OutLoc.Location, /*AcceptanceRadius*/ 50.f, /*bStopOnOverlap*/ true);
        if (bDrawDebug)
        {
            DrawDebugSphere(GetWorld(), OutLoc.Location, 30.f, 12, FColor::Blue, false, 1.5f);
        }
    }
}

void AEnemyAIController::StartChase()
{
    if (!GetPawn()) return;
    APawn* Player = CachedPlayer.Get();
    if (!Player)
    {
        CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
        Player = CachedPlayer.Get();
        if (!Player) return;
    }

    State = EEnemyState::Chase;
    MoveToActor(Player, /*AcceptanceRadius*/ AttackRange * 0.8f, true);

    if (IsPlayerInAttackRange())
    {
        // Simple attack placeholder: stop and debug
        StopMovement();
        if (bDrawDebug)
        {
            DrawDebugString(GetWorld(), GetPawn()->GetActorLocation() + FVector(0,0,120), TEXT("Attack!"), nullptr, FColor::Red, 0.2f);
        }
        // You could ApplyDamage to player here if you add a health system
    }
}

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    if (State == EEnemyState::Patrol)
    {
        // After reaching a patrol point, idle a bit then pick another
        StartIdle(FMath::FRandRange(IdleTimeMin, IdleTimeMax));
    }
    else if (State == EEnemyState::Chase)
    {
        // Keep thinking; Think() will refresh chase target or fall back to idle
    }
}
