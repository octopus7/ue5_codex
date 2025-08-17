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

    // Start in Chase state regardless of sight
    State = EEnemyState::Chase;
    StartChase();
    GetWorldTimerManager().SetTimer(ThinkTimerHandle, this, &AEnemyAIController::Think, ThinkInterval, true, ThinkInterval);
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

    if (bUseFOV)
    {
        const FVector Forward = SelfPawn->GetActorForwardVector();
        const FVector Dir = ToPlayer.GetSafeNormal2D();
        const float CosHalfFOV = FMath::Cos(FMath::DegreesToRadians(FOVDegrees * 0.5f));
        const float Dot = FVector::DotProduct(Forward.GetSafeNormal2D(), Dir);
        if (Dot < CosHalfFOV)
        {
            return false;
        }
    }

    // Optional: visibility trace
    if (bUseLineOfSight)
    {
        FHitResult Hit;
        FCollisionQueryParams P(SCENE_QUERY_STAT(EnemySight), false, SelfPawn);
        P.AddIgnoredActor(SelfPawn);
        const bool bBlocked = GetWorld()->LineTraceSingleByChannel(Hit, SelfLoc + FVector(0,0,SightHeightOffset), PlayerLoc + FVector(0,0,SightHeightOffset), ECC_Visibility, P);
        if (bBlocked && Hit.GetActor() != Player)
        {
            return false;
        }
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
    // Refresh player pointer if needed
    if (!CachedPlayer.IsValid())
    {
        CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
    }

    // Evaluate player visibility
    // bStrict=true uses SightRadius, false uses LoseSightRadius
    const bool bSee = CanSeePlayer(true);
    const bool bKeepChase = CanSeePlayer(false);

    switch (State)
    {
    case EEnemyState::Chase:
        // If player not yet valid (e.g., early possess), keep trying to acquire and chase
        if (!CachedPlayer.IsValid())
        {
            StartChase();
            break;
        }
        if (!bKeepChase)
        {
            StartIdle(FMath::FRandRange(IdleTimeMin, IdleTimeMax));
            return;
        }
        // Refresh chase target while chasing
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

    if (bDrawDebug)
    {
        if (APawn* SelfPawn = GetPawn())
        {
            if (APawn* Player = CachedPlayer.Get())
            {
                const bool bLOS = CanSeePlayer(true);
                const FColor Col = bLOS ? FColor::Green : FColor::Red;
                DrawDebugLine(GetWorld(), SelfPawn->GetActorLocation()+FVector(0,0,SightHeightOffset), Player->GetActorLocation()+FVector(0,0,SightHeightOffset), Col, false, ThinkInterval*1.1f, 0, 1.5f);
            }
            if (bUseFOV)
            {
                DrawFOVDebug(SightRadius, FColor(0, 255, 0));
                DrawFOVDebug(LoseSightRadius, FColor(255, 215, 0)); // gold-ish
            }
        }
    }
}

void AEnemyAIController::DrawFOVDebug(float Radius, const FColor& Color) const
{
    if (!GetPawn() || Radius <= 0.f)
    {
        return;
    }
    UWorld* World = GetWorld();
    const FVector Center = GetPawn()->GetActorLocation() + FVector(0,0,SightHeightOffset);
    const FVector Forward = GetPawn()->GetActorForwardVector();

    const float HalfFOV = FMath::Max(0.f, FOVDegrees * 0.5f);
    const int32 Segments = FMath::Clamp(FMath::CeilToInt(FMath::Max(8.f, FOVDegrees / 5.f)), 8, 90);
    const float Step = (Segments > 0) ? (FOVDegrees / Segments) : FOVDegrees;

    const FVector Up = FVector::UpVector;
    FVector StartDir = Forward.RotateAngleAxis(-HalfFOV, Up).GetSafeNormal2D();
    FVector Prev = Center + StartDir * Radius;
    for (int32 i = 1; i <= Segments; ++i)
    {
        const float Angle = -HalfFOV + Step * i;
        const FVector Dir = Forward.RotateAngleAxis(Angle, Up).GetSafeNormal2D();
        const FVector Pt = Center + Dir * Radius;
        DrawDebugLine(World, Prev, Pt, Color, false, ThinkInterval*1.1f, 0, 1.5f);
        Prev = Pt;
    }

    // Boundary rays
    const FVector LeftDir = Forward.RotateAngleAxis(-HalfFOV, Up).GetSafeNormal2D();
    const FVector RightDir = Forward.RotateAngleAxis(HalfFOV, Up).GetSafeNormal2D();
    DrawDebugLine(World, Center, Center + LeftDir * Radius, Color, false, ThinkInterval*1.1f, 0, 1.0f);
    DrawDebugLine(World, Center, Center + RightDir * Radius, Color, false, ThinkInterval*1.1f, 0, 1.0f);
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

float AEnemyAIController::GetIdleTimeRemaining() const
{
    if (State != EEnemyState::Idle)
    {
        return 0.f;
    }
    return const_cast<AEnemyAIController*>(this)->GetWorldTimerManager().GetTimerRemaining(const_cast<AEnemyAIController*>(this)->IdleTimerHandle);
}
