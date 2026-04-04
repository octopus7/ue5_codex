#include "Systems/Combat/CMWCombatComponent.h"

#include "Engine/EngineTypes.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Systems/Combat/CMWProjectile.h"
#include "Systems/Game/CMWGameDataAsset.h"
#include "Systems/Game/CMWGameInstance.h"

UCMWCombatComponent::UCMWCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCMWCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const UCMWGameDataAsset* GameData = ResolveGameData())
	{
		CurrentAttackMode = GameData->InitialAttackMode;
	}
}

void UCMWCombatComponent::ToggleAttackMode()
{
	SetAttackMode(CurrentAttackMode == ECMWAttackMode::Projectile ? ECMWAttackMode::Melee : ECMWAttackMode::Projectile);
}

void UCMWCombatComponent::SetAttackMode(ECMWAttackMode NewAttackMode)
{
	if (CurrentAttackMode == NewAttackMode)
	{
		return;
	}

	CurrentAttackMode = NewAttackMode;
	OnAttackModeChanged.Broadcast(CurrentAttackMode);
}

void UCMWCombatComponent::PerformAttack(const FVector& AimWorldLocation)
{
	const UCMWGameDataAsset* GameData = ResolveGameData();
	AActor* OwnerActor = GetOwner();
	UWorld* World = GetWorld();
	if (!GameData || !OwnerActor || !World)
	{
		return;
	}

	switch (CurrentAttackMode)
	{
	case ECMWAttackMode::Projectile:
		PerformProjectileAttack(*GameData, AimWorldLocation);
		break;
	case ECMWAttackMode::Melee:
		PerformMeleeAttack(*GameData, AimWorldLocation);
		break;
	default:
		break;
	}
}

const UCMWGameDataAsset* UCMWCombatComponent::ResolveGameData() const
{
	if (const UCMWGameInstance* GameInstance = UCMWGameInstance::Get(this))
	{
		return GameInstance->GetGameData();
	}

	return nullptr;
}

void UCMWCombatComponent::PerformProjectileAttack(const UCMWGameDataAsset& GameData, const FVector& AimWorldLocation)
{
	AActor* OwnerActor = GetOwner();
	UWorld* World = GetWorld();
	if (!OwnerActor || !World)
	{
		return;
	}

	FVector AttackDirection = AimWorldLocation - OwnerActor->GetActorLocation();
	AttackDirection.Z = 0.0f;
	if (AttackDirection.IsNearlyZero())
	{
		AttackDirection = OwnerActor->GetActorForwardVector();
	}
	AttackDirection.Normalize();

	const FVector SpawnLocation = OwnerActor->GetActorLocation() + AttackDirection * GameData.ProjectileSpawnDistance + FVector(0.0f, 0.0f, 50.0f);
	const FRotator SpawnRotation = AttackDirection.Rotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = OwnerActor;
	SpawnParameters.Instigator = Cast<APawn>(OwnerActor);
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	UClass* ProjectileClass = GameData.ProjectileClass ? *GameData.ProjectileClass : ACMWProjectile::StaticClass();
	if (ACMWProjectile* Projectile = World->SpawnActor<ACMWProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters))
	{
		Projectile->InitializeProjectile(OwnerActor, GameData.ProjectileDamage);
	}
}

void UCMWCombatComponent::PerformMeleeAttack(const UCMWGameDataAsset& GameData, const FVector& AimWorldLocation)
{
	AActor* OwnerActor = GetOwner();
	UWorld* World = GetWorld();
	if (!OwnerActor || !World)
	{
		return;
	}

	FVector AttackDirection = AimWorldLocation - OwnerActor->GetActorLocation();
	AttackDirection.Z = 0.0f;
	if (AttackDirection.IsNearlyZero())
	{
		AttackDirection = OwnerActor->GetActorForwardVector();
	}
	AttackDirection.Normalize();

	const FVector AttackCenter = OwnerActor->GetActorLocation() + AttackDirection * GameData.MeleeRange * 0.5f;
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(GameData.MeleeRange);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CMWMeleeAttack), false, OwnerActor);

	TArray<FOverlapResult> Overlaps;
	if (!World->OverlapMultiByObjectType(Overlaps, AttackCenter, FQuat::Identity, ObjectQueryParams, CollisionShape, QueryParams))
	{
		return;
	}

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (!HitActor || HitActor == OwnerActor)
		{
			continue;
		}

		UGameplayStatics::ApplyDamage(HitActor, GameData.MeleeDamage, Cast<APawn>(OwnerActor) ? Cast<APawn>(OwnerActor)->GetController() : nullptr, OwnerActor, nullptr);
	}
}
