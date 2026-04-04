#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Systems/Combat/CMWCombatTypes.h"
#include "CMWCombatComponent.generated.h"

class UCMWGameDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCMWAttackModeChangedSignature, ECMWAttackMode, NewAttackMode);

UCLASS(ClassGroup = (CodexMazeWalker), meta = (BlueprintSpawnableComponent))
class CODEXMAZEWALKER_API UCMWCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCMWCombatComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "CodexMazeWalker|Combat")
	void ToggleAttackMode();

	UFUNCTION(BlueprintCallable, Category = "CodexMazeWalker|Combat")
	void SetAttackMode(ECMWAttackMode NewAttackMode);

	UFUNCTION(BlueprintCallable, Category = "CodexMazeWalker|Combat")
	void PerformAttack(const FVector& AimWorldLocation);

	UFUNCTION(BlueprintPure, Category = "CodexMazeWalker|Combat")
	ECMWAttackMode GetCurrentAttackMode() const
	{
		return CurrentAttackMode;
	}

	UPROPERTY(BlueprintAssignable, Category = "CodexMazeWalker|Combat")
	FCMWAttackModeChangedSignature OnAttackModeChanged;

protected:
	const UCMWGameDataAsset* ResolveGameData() const;
	void PerformProjectileAttack(const UCMWGameDataAsset& GameData, const FVector& AimWorldLocation);
	void PerformMeleeAttack(const UCMWGameDataAsset& GameData, const FVector& AimWorldLocation);

	UPROPERTY(EditAnywhere, Category = "Combat")
	ECMWAttackMode CurrentAttackMode = ECMWAttackMode::Projectile;
};
