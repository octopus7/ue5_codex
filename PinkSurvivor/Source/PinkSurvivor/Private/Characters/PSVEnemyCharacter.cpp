#include "Characters/PSVEnemyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/PSVHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Pickups/PSVExperienceGem.h"
#include "TimerManager.h"

APSVEnemyCharacter::APSVEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    HealthComponent = CreateDefaultSubobject<UPSVHealthComponent>(TEXT("HealthComponent"));

    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->MaxWalkSpeed = MoveSpeed;
        Movement->bOrientRotationToMovement = true;
    }

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void APSVEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &APSVEnemyCharacter::HandleDeath);
        HealthComponent->InitializeHealth(MaxHealth, true);
    }

    if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        TargetPawn = PlayerPawn;
    }
}

void APSVEnemyCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsDead)
    {
        return;
    }

    if (!TargetPawn.IsValid())
    {
        TargetPawn = UGameplayStatics::GetPlayerPawn(this, 0);
        if (!TargetPawn.IsValid())
        {
            return;
        }
    }

    UpdateMovement(DeltaSeconds);
    TryAttack();
}

float APSVEnemyCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.f && HealthComponent && !bIsDead)
    {
        HealthComponent->ApplyDamage(ActualDamage);
    }

    return ActualDamage;
}

void APSVEnemyCharacter::UpdateMovement(float DeltaSeconds)
{
    if (!TargetPawn.IsValid())
    {
        return;
    }

    const FVector ToTarget = TargetPawn->GetActorLocation() - GetActorLocation();
    FVector ToTarget2D(ToTarget.X, ToTarget.Y, 0.f);
    const float DistanceToTarget = ToTarget2D.Size();

    if (DistanceToTarget > AttackRange + AttackRadiusTolerance)
    {
        StopAttackLoop();

        const FVector MoveDirection = ToTarget2D.GetSafeNormal();
        AddMovementInput(MoveDirection, 1.f);
    }
    else
    {
        if (UCharacterMovementComponent* Movement = GetCharacterMovement())
        {
            Movement->StopMovementImmediately();
        }
    }

    if (!ToTarget2D.IsNearlyZero())
    {
        const FRotator DesiredRotation = ToTarget2D.ToOrientationRotator();
        const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaSeconds, 8.f);
        SetActorRotation(NewRotation);
    }
}

void APSVEnemyCharacter::TryAttack()
{
    if (!TargetPawn.IsValid())
    {
        return;
    }

    const FVector ToTarget = TargetPawn->GetActorLocation() - GetActorLocation();
    const float Distance2D = FVector(ToTarget.X, ToTarget.Y, 0.f).Size();

    if (Distance2D <= AttackRange)
    {
        StartAttackLoop();
    }
    else
    {
        StopAttackLoop();
    }
}

void APSVEnemyCharacter::StartAttackLoop()
{
    if (bIsAttacking || AttackInterval <= 0.f)
    {
        return;
    }

    bIsAttacking = true;

    PerformAttack();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(AttackTimerHandle, this, &APSVEnemyCharacter::PerformAttack, AttackInterval, true, AttackInterval);
    }
}

void APSVEnemyCharacter::StopAttackLoop()
{
    if (!bIsAttacking)
    {
        return;
    }

    bIsAttacking = false;

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AttackTimerHandle);
    }
}

void APSVEnemyCharacter::PerformAttack()
{
    if (!TargetPawn.IsValid() || bIsDead)
    {
        StopAttackLoop();
        return;
    }

    const FVector ToTarget = TargetPawn->GetActorLocation() - GetActorLocation();
    const float Distance2D = FVector(ToTarget.X, ToTarget.Y, 0.f).Size();

    if (Distance2D > AttackRange + AttackRadiusTolerance)
    {
        StopAttackLoop();
        return;
    }

    AActor* TargetActor = TargetPawn.Get();
    if (TargetActor && AttackDamage > 0.f)
    {
        UGameplayStatics::ApplyDamage(TargetActor, AttackDamage, GetController(), this, nullptr);
    }
}

void APSVEnemyCharacter::HandleDeath()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;
    StopAttackLoop();

    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->DisableMovement();
    }

    if (UCapsuleComponent* Capsule = GetCapsuleComponent())
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    SpawnExperienceGem();

    SetLifeSpan(2.0f);
}

void APSVEnemyCharacter::SpawnExperienceGem()
{
    if (ExperienceGemClasses.Num() == 0)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    TSubclassOf<APSVExperienceGem> GemClass = nullptr;
    if (ExperienceGemClasses.Num() == 1)
    {
        GemClass = ExperienceGemClasses[0];
    }
    else
    {
        const int32 RandomIndex = FMath::RandRange(0, ExperienceGemClasses.Num() - 1);
        GemClass = ExperienceGemClasses.IsValidIndex(RandomIndex) ? ExperienceGemClasses[RandomIndex] : nullptr;
    }

    if (!GemClass)
    {
        return;
    }

    FVector SpawnLocation = GetActorLocation();
    SpawnLocation.Z += GemSpawnOffsetZ;

    const FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    World->SpawnActor<APSVExperienceGem>(GemClass, SpawnLocation, SpawnRotation, SpawnParams);
}
