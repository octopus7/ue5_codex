#include "CodexHarnessEnemyCharacter.h"

#include "CodexHarnessGameMode.h"
#include "CodexHarnessHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

ACodexHarnessEnemyCharacter::ACodexHarnessEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bRunPhysicsWithNoController = true;
	GetCharacterMovement()->MaxWalkSpeed = 320.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1600.0f;

	GetMesh()->SetHiddenInGame(true);
	GetMesh()->SetVisibility(false);

	VisualMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMeshComponent"));
	VisualMeshComponent->SetupAttachment(RootComponent);
	VisualMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMeshComponent->SetGenerateOverlapEvents(false);
	VisualMeshComponent->SetCanEverAffectNavigation(false);

	HealthComponent = CreateDefaultSubobject<UCodexHarnessHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetMaxHealth(30.0f);

	RefreshVisualMeshDefaults();
}

void ACodexHarnessEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent != nullptr)
	{
		HealthComponent->OnDeath().AddUObject(this, &ThisClass::HandleDeath);
	}
}

void ACodexHarnessEnemyCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	static_cast<void>(DeltaTime);

	TickChaseAndAttack();
}

void ACodexHarnessEnemyCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	RefreshVisualMeshDefaults();
}

void ACodexHarnessEnemyCharacter::PostLoad()
{
	Super::PostLoad();

	RefreshVisualMeshDefaults();
}

#if WITH_EDITOR
void ACodexHarnessEnemyCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	RefreshVisualMeshDefaults();
}
#endif

void ACodexHarnessEnemyCharacter::RefreshVisualMeshDefaults()
{
	if (VisualMeshComponent == nullptr)
	{
		return;
	}

	VisualMeshComponent->SetRelativeLocation(DefaultVisualMeshOffset);
	VisualMeshComponent->SetRelativeScale3D(DefaultVisualMeshScale);
	VisualMeshComponent->SetStaticMesh(DefaultVisualMesh);
}

UStaticMeshComponent* ACodexHarnessEnemyCharacter::GetVisualMeshComponent() const
{
	return VisualMeshComponent;
}

UCodexHarnessHealthComponent* ACodexHarnessEnemyCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

bool ACodexHarnessEnemyCharacter::IsAlive() const
{
	return HealthComponent != nullptr ? HealthComponent->IsAlive() : true;
}

void ACodexHarnessEnemyCharacter::TickChaseAndAttack()
{
	UWorld* const World = GetWorld();
	if (World == nullptr || !IsAlive())
	{
		return;
	}

	const ACodexHarnessGameMode* const CodexHarnessGameMode = World->GetAuthGameMode<ACodexHarnessGameMode>();
	if (CodexHarnessGameMode != nullptr && CodexHarnessGameMode->IsGameOver())
	{
		return;
	}

	APawn* const PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn == nullptr)
	{
		return;
	}

	const UCodexHarnessHealthComponent* const PlayerHealth = PlayerPawn->FindComponentByClass<UCodexHarnessHealthComponent>();
	if (PlayerHealth == nullptr || !PlayerHealth->IsAlive())
	{
		return;
	}

	FVector TargetOffset = PlayerPawn->GetActorLocation() - GetActorLocation();
	TargetOffset.Z = 0.0f;
	const float DistanceToPlayer = TargetOffset.Size();
	if (DistanceToPlayer <= KINDA_SMALL_NUMBER || DistanceToPlayer > FollowRange)
	{
		return;
	}

	const FVector MoveDirection = TargetOffset / DistanceToPlayer;
	SetActorRotation(MoveDirection.Rotation());

	if (DistanceToPlayer > AttackRange)
	{
		AddMovementInput(MoveDirection, 1.0f);
		return;
	}

	if (World->GetTimeSeconds() >= NextAttackTime)
	{
		UGameplayStatics::ApplyDamage(PlayerPawn, AttackDamage, GetController(), this, UDamageType::StaticClass());
		NextAttackTime = World->GetTimeSeconds() + AttackInterval;
	}
}

void ACodexHarnessEnemyCharacter::HandleDeath()
{
	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->DisableMovement();
	}

	if (ACodexHarnessGameMode* const CodexHarnessGameMode = GetWorld() != nullptr
		? GetWorld()->GetAuthGameMode<ACodexHarnessGameMode>()
		: nullptr)
	{
		CodexHarnessGameMode->HandleEnemyDeath(this);
	}

	Destroy();
}
