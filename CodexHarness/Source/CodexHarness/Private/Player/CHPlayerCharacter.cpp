#include "Player/CHPlayerCharacter.h"

#include "Combat/CHHealthComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogCHPlayerCharacter, Log, All);

namespace CHPlayerCharacter
{
	static FVector GetPlanarDirection(const FVector& InputDirection, const FVector& FallbackDirection)
	{
		FVector PlanarDirection = InputDirection;
		PlanarDirection.Z = 0.0f;
		if (PlanarDirection.Normalize())
		{
			return PlanarDirection;
		}

		return FallbackDirection;
	}
}

ACHPlayerCharacter::ACHPlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationYaw = false;

	UCharacterMovementComponent* const MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->bUseControllerDesiredRotation = false;
	MovementComponent->bConstrainToPlane = true;
	MovementComponent->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);
	MovementComponent->bSnapToPlaneAtStart = true;

	HealthComponent = CreateDefaultSubobject<UCHHealthComponent>(TEXT("HealthComponent"));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 1200.0f;
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = false;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ViewCamera->bUsePawnControlRotation = false;
}

void ACHPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddUObject(this, &ACHPlayerCharacter::HandleHealthDepleted);
	}
}

float ACHPlayerCharacter::TakeDamage(
	const float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!HealthComponent)
	{
		return 0.0f;
	}

	float AppliedDamage = 0.0f;
	HealthComponent->ApplyDamage(DamageAmount, AppliedDamage);
	return AppliedDamage;
}

void ACHPlayerCharacter::Move(const FVector2D& MovementInput, const float DeltaSeconds)
{
	if (bIsDead)
	{
		return;
	}

	const FVector2D ClampedMovementInput = MovementInput.GetClampedToMaxSize(1.0f);
	if (ClampedMovementInput.IsNearlyZero())
	{
		return;
	}

	const FVector ForwardDirection = CHPlayerCharacter::GetPlanarDirection(
		ViewCamera ? ViewCamera->GetForwardVector() : FVector::ForwardVector,
		FVector::ForwardVector);
	const FVector RightDirection = CHPlayerCharacter::GetPlanarDirection(
		ViewCamera ? ViewCamera->GetRightVector() : FVector::RightVector,
		FVector::RightVector);

	FVector DesiredMovement = (ForwardDirection * ClampedMovementInput.Y) + (RightDirection * ClampedMovementInput.X);
	if (!DesiredMovement.Normalize())
	{
		return;
	}

	AddActorWorldOffset(DesiredMovement * MoveSpeed * DeltaSeconds, false);
}

void ACHPlayerCharacter::AimAt(const FVector& WorldPoint)
{
	if (bIsDead)
	{
		return;
	}

	FVector AimDirection = WorldPoint - GetActorLocation();
	AimDirection.Z = 0.0f;
	if (!AimDirection.Normalize())
	{
		return;
	}

	LastAimWorldPoint = WorldPoint;
	bHasAimWorldPoint = true;
	SetActorRotation(AimDirection.Rotation());
}

bool ACHPlayerCharacter::TryFire(FCHFireResult& OutFireResult)
{
	FCHFireResult FireResult;
	if (bIsDead)
	{
		LastFireResult = FireResult;
		OutFireResult = FireResult;
		return false;
	}

	FireResult.ShotStart = GetActorLocation();
	FireResult.AimWorldPoint = bHasAimWorldPoint
		? LastAimWorldPoint
		: (FireResult.ShotStart + (GetActorForwardVector().GetSafeNormal2D() * FireRange));

	UWorld* const World = GetWorld();
	if (!World)
	{
		LastFireResult = FireResult;
		OutFireResult = FireResult;
		return false;
	}

	const float WorldTimeSeconds = World->GetTimeSeconds();
	if ((WorldTimeSeconds - LastFireWorldTime) < FireCooldownSeconds)
	{
		FireResult.bBlockedByCooldown = true;
		LastFireResult = FireResult;
		OutFireResult = FireResult;
		return false;
	}

	FVector ShotDirection = FireResult.AimWorldPoint - FireResult.ShotStart;
	ShotDirection.Z = 0.0f;
	if (!ShotDirection.Normalize())
	{
		ShotDirection = GetActorForwardVector().GetSafeNormal2D();
	}

	FireResult.bShotExecuted = true;
	FireResult.TraceEnd = FireResult.ShotStart + (ShotDirection * FireRange);
	FireResult.ImpactPoint = FireResult.TraceEnd;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CHPlayerFireTrace), false, this);
	if (World->LineTraceSingleByChannel(HitResult, FireResult.ShotStart, FireResult.TraceEnd, ECC_Visibility, QueryParams))
	{
		FireResult.bHit = true;
		FireResult.TraceEnd = HitResult.ImpactPoint;
		FireResult.ImpactPoint = HitResult.ImpactPoint;
		if (const AActor* const HitActor = HitResult.GetActor())
		{
			FireResult.HitActorName = HitActor->GetFName();
		}

		if (AActor* const HitActor = HitResult.GetActor())
		{
			UGameplayStatics::ApplyDamage(HitActor, FireDamage, GetController(), this, UDamageType::StaticClass());
		}
	}

	LastFireWorldTime = WorldTimeSeconds;
	LastFireResult = FireResult;
	OutFireResult = FireResult;
	return true;
}

void ACHPlayerCharacter::HandleHealthDepleted(UCHHealthComponent* InHealthComponent)
{
	if (bIsDead || !InHealthComponent)
	{
		return;
	}

	bIsDead = true;
	if (UCharacterMovementComponent* const MovementComponent = GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->DisableMovement();
	}

	SetActorEnableCollision(false);
	UE_LOG(LogCHPlayerCharacter, Display, TEXT("플레이어 사망 처리 완료"));
}
