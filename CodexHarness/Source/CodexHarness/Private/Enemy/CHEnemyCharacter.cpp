#include "Enemy/CHEnemyCharacter.h"

#include "Combat/CHHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Player/CHPlayerCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogCHEnemyCharacter, Log, All);

ACHEnemyCharacter::ACHEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 88.0f);

	UCharacterMovementComponent* const MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->bUseControllerDesiredRotation = false;
	MovementComponent->bConstrainToPlane = true;
	MovementComponent->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);
	MovementComponent->bSnapToPlaneAtStart = true;

	HealthComponent = CreateDefaultSubobject<UCHHealthComponent>(TEXT("HealthComponent"));
}

void ACHEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	bEnemySmokeEnabled = FParse::Param(FCommandLine::Get(), TEXT("CHEnemySmoke"));
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddUObject(this, &ACHEnemyCharacter::HandleHealthDepleted);
	}
}

void ACHEnemyCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateChaseAndAttack(DeltaSeconds);
}

float ACHEnemyCharacter::TakeDamage(
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

void ACHEnemyCharacter::UpdateChaseAndAttack(const float DeltaTime)
{
	if (bIsDead)
	{
		return;
	}

	ACHPlayerCharacter* const PlayerCharacter = Cast<ACHPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacter || PlayerCharacter->IsDead())
	{
		return;
	}

	const FVector CurrentLocation = GetActorLocation();
	const FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	FVector ToPlayer = PlayerLocation - CurrentLocation;
	ToPlayer.Z = 0.0f;

	const float DistanceToPlayer = ToPlayer.Size();
	if (DistanceToPlayer <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	const FVector ChaseDirection = ToPlayer / DistanceToPlayer;
	SetActorRotation(ChaseDirection.Rotation());

	if (DistanceToPlayer > AttackRange)
	{
		AddActorWorldOffset(ChaseDirection * ChaseMoveSpeed * DeltaTime, false);
		if (bEnemySmokeEnabled && !bEnemySmokeChaseLogged)
		{
			UE_LOG(LogCHEnemyCharacter, Display, TEXT("CHEnemyChaseDistance=%.2f"), FVector::Dist2D(GetActorLocation(), PlayerLocation));
			bEnemySmokeChaseLogged = true;
		}
		return;
	}

	UWorld* const World = GetWorld();
	if (!World)
	{
		return;
	}

	const float WorldTimeSeconds = World->GetTimeSeconds();
	if ((WorldTimeSeconds - LastAttackWorldTime) < AttackCooldownSeconds)
	{
		return;
	}

	LastAttackWorldTime = WorldTimeSeconds;
	const float AppliedDamage = UGameplayStatics::ApplyDamage(PlayerCharacter, AttackDamage, nullptr, this, UDamageType::StaticClass());
	const UCHHealthComponent* const PlayerHealthComponent = PlayerCharacter->GetHealthComponent();
	UE_LOG(
		LogCHEnemyCharacter,
		Display,
		TEXT("CHEnemyAttack Applied=%.2f PlayerHealth=%.2f"),
		AppliedDamage,
		PlayerHealthComponent ? PlayerHealthComponent->GetCurrentHealth() : -1.0f);

	if (bEnemySmokeEnabled && !bEnemySmokeFinished)
	{
		bEnemySmokeFinished = true;
		if (APlayerController* const PlayerController = UGameplayStatics::GetPlayerController(this, 0))
		{
			PlayerController->ConsoleCommand(TEXT("quit"));
		}
	}
}

void ACHEnemyCharacter::HandleHealthDepleted(UCHHealthComponent* InHealthComponent)
{
	if (bIsDead || !InHealthComponent)
	{
		return;
	}

	bIsDead = true;
	SetActorEnableCollision(false);
	UE_LOG(LogCHEnemyCharacter, Display, TEXT("CHEnemyDeath Actor=%s"), *GetName());
	Destroy();
}
