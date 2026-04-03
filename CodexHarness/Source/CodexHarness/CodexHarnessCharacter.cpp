#include "CodexHarnessCharacter.h"

#include "Camera/CameraShakeBase.h"
#include "CodexHarnessCollisionChannels.h"
#include "Camera/CameraComponent.h"
#include "CodexHarnessGameMode.h"
#include "CodexHarnessHealthComponent.h"
#include "CodexHarnessGameInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

ACodexHarnessCharacter::ACodexHarnessCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	GetMesh()->SetHiddenInGame(true);
	GetMesh()->SetVisibility(false);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1800.0f;
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	VisualMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMeshComponent"));
	VisualMeshComponent->SetupAttachment(RootComponent);
	VisualMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMeshComponent->SetGenerateOverlapEvents(false);
	VisualMeshComponent->SetCanEverAffectNavigation(false);

	HealthComponent = CreateDefaultSubobject<UCodexHarnessHealthComponent>(TEXT("HealthComponent"));

	RefreshVisualMeshDefaults();
}

void ACodexHarnessCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &ThisClass::HandleTakeAnyDamage);

	if (HealthComponent != nullptr)
	{
		HealthComponent->OnDeath().AddUObject(this, &ThisClass::HandleDeath);
	}
}

void ACodexHarnessCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	RefreshVisualMeshDefaults();
}

void ACodexHarnessCharacter::PostLoad()
{
	Super::PostLoad();

	RefreshVisualMeshDefaults();
}

#if WITH_EDITOR
void ACodexHarnessCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	RefreshVisualMeshDefaults();
}
#endif

void ACodexHarnessCharacter::RefreshVisualMeshDefaults()
{
	if (VisualMeshComponent == nullptr)
	{
		return;
	}

	VisualMeshComponent->SetRelativeLocation(DefaultVisualMeshOffset);
	VisualMeshComponent->SetRelativeScale3D(DefaultVisualMeshScale);
	VisualMeshComponent->SetStaticMesh(DefaultVisualMesh);
}

void ACodexHarnessCharacter::MoveInTopDownPlane(const FVector2D& MovementInput)
{
	if (!IsAlive() || Controller == nullptr || MovementInput.IsNearlyZero())
	{
		return;
	}

	const FVector CameraForward = FollowCamera != nullptr ? FollowCamera->GetForwardVector() : GetActorForwardVector();
	const FVector CameraRight = FollowCamera != nullptr ? FollowCamera->GetRightVector() : GetActorRightVector();

	const FVector ForwardDirection = FVector(CameraForward.X, CameraForward.Y, 0.0f).GetSafeNormal();
	const FVector RightDirection = FVector(CameraRight.X, CameraRight.Y, 0.0f).GetSafeNormal();
	if (ForwardDirection.IsNearlyZero() || RightDirection.IsNearlyZero())
	{
		return;
	}

	AddMovementInput(ForwardDirection, MovementInput.Y);
	AddMovementInput(RightDirection, MovementInput.X);
}

void ACodexHarnessCharacter::AimAtWorldLocation(const FVector& WorldLocation)
{
	if (!IsAlive())
	{
		return;
	}

	RotateTowardWorldDirection(WorldLocation - GetActorLocation());
}

void ACodexHarnessCharacter::FireAtWorldLocation(const FVector& WorldLocation)
{
	if (!IsAlive())
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	const FVector TraceStart = GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
	FVector FireDirection = WorldLocation - TraceStart;
	if (FireDirection.IsNearlyZero())
	{
		FireDirection = GetActorForwardVector();
	}
	FireDirection.Normalize();

	const FVector TraceEnd = TraceStart + (FireDirection * WeaponRange);
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CodexHarnessWeaponTrace), false);
	QueryParams.AddIgnoredActor(this);

	FHitResult HitResult;
	if (World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CodexHarnessCollisionChannels::PlayerWeaponTrace, QueryParams))
	{
		if (AActor* const HitActor = HitResult.GetActor())
		{
			if (HitActor->FindComponentByClass<UCodexHarnessHealthComponent>() != nullptr)
			{
				UGameplayStatics::ApplyDamage(HitActor, WeaponDamage, GetController(), this, UDamageType::StaticClass());
			}
		}
	}
}

void ACodexHarnessCharacter::HandleDeath()
{
	if (GetCharacterMovement() != nullptr)
	{
		GetCharacterMovement()->DisableMovement();
	}

	if (ACodexHarnessGameMode* const CodexHarnessGameMode = GetWorld() != nullptr
		? GetWorld()->GetAuthGameMode<ACodexHarnessGameMode>()
		: nullptr)
	{
		CodexHarnessGameMode->HandlePlayerDeath(this);
	}
}

void ACodexHarnessCharacter::HandleTakeAnyDamage(
	AActor* DamagedActor,
	const float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser)
{
	static_cast<void>(DamagedActor);
	static_cast<void>(DamageType);

	if (Damage <= 0.0f)
	{
		return;
	}

	PlayDamageReactionEffect();

	if (DamageKnockbackStrength <= 0.0f || !IsAlive())
	{
		return;
	}

	FVector KnockbackDirection = -GetActorForwardVector();
	KnockbackDirection.Z = 0.0f;

	if (DamageCauser != nullptr)
	{
		KnockbackDirection = GetActorLocation() - DamageCauser->GetActorLocation();
		KnockbackDirection.Z = 0.0f;
	}
	else if (InstigatedBy != nullptr && InstigatedBy->GetPawn() != nullptr)
	{
		KnockbackDirection = GetActorLocation() - InstigatedBy->GetPawn()->GetActorLocation();
		KnockbackDirection.Z = 0.0f;
	}

	KnockbackDirection.Normalize();
	if (KnockbackDirection.IsNearlyZero())
	{
		return;
	}

	const FVector KnockbackVelocity =
		(KnockbackDirection * DamageKnockbackStrength) + (FVector::UpVector * DamageKnockbackUpwardVelocity);
	LaunchCharacter(KnockbackVelocity, true, DamageKnockbackUpwardVelocity > 0.0f);
}

void ACodexHarnessCharacter::PlayDamageReactionEffect() const
{
	const UCodexHarnessGameInstance* const CodexHarnessGameInstance = Cast<UCodexHarnessGameInstance>(GetGameInstance());
	const UCodexHarnessEffectsConfigDataAsset* const EffectsConfig = CodexHarnessGameInstance != nullptr
		? CodexHarnessGameInstance->GetEffectsConfig()
		: nullptr;
	const UNiagaraSystem* const PlayerHitReactionSystem = EffectsConfig != nullptr
		? EffectsConfig->PlayerHitReactionSystem
		: nullptr;
	if (PlayerHitReactionSystem == nullptr)
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this,
		const_cast<UNiagaraSystem*>(PlayerHitReactionSystem),
		GetActorLocation() + EffectsConfig->PlayerHitReactionLocationOffset,
		FRotator::ZeroRotator,
		EffectsConfig->PlayerHitReactionScale,
		true,
		true);

	if (APlayerController* const PlayerController = Cast<APlayerController>(GetController()))
	{
		if (TSubclassOf<UCameraShakeBase> CameraShakeClass = CodexHarnessGameInstance->GetPlayerHitCameraShakeClass())
		{
			PlayerController->ClientStartCameraShake(
				CameraShakeClass,
				CodexHarnessGameInstance->GetPlayerHitCameraShakeScale());
		}
	}
}

void ACodexHarnessCharacter::RotateTowardWorldDirection(const FVector& WorldDirection)
{
	FVector FlatDirection = WorldDirection;
	FlatDirection.Z = 0.0f;
	if (FlatDirection.IsNearlyZero())
	{
		return;
	}

	SetActorRotation(FlatDirection.Rotation());
}

USpringArmComponent* ACodexHarnessCharacter::GetCameraBoom() const
{
	return CameraBoom;
}

UCameraComponent* ACodexHarnessCharacter::GetFollowCamera() const
{
	return FollowCamera;
}

UStaticMeshComponent* ACodexHarnessCharacter::GetVisualMeshComponent() const
{
	return VisualMeshComponent;
}

UCodexHarnessHealthComponent* ACodexHarnessCharacter::GetHealthComponent() const
{
	return HealthComponent;
}

bool ACodexHarnessCharacter::IsAlive() const
{
	return HealthComponent != nullptr ? HealthComponent->IsAlive() : true;
}
