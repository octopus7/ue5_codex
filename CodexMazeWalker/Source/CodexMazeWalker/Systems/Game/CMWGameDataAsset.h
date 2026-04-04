#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Widgets/Layout/Anchors.h"
#include "Systems/Combat/CMWCombatTypes.h"
#include "CMWGameDataAsset.generated.h"

class ACMWProjectile;
class UCMWMinimapWidget;
class UInputAction;
class UInputMappingContext;
class UMaterialInterface;

UCLASS(BlueprintType)
class CODEXMAZEWALKER_API UCMWGameDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleAttackModeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	ECMWAttackMode InitialAttackMode = ECMWAttackMode::Projectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<ACMWProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float ProjectileSpawnDistance = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float ProjectileDamage = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float MeleeRange = 180.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (ClampMin = "0.0"))
	float MeleeDamage = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	TSubclassOf<UCMWMinimapWidget> MinimapWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	FAnchors MinimapAnchors = FAnchors(1.0f, 0.0f, 1.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	FVector2D MinimapAlignment = FVector2D(1.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	FVector2D MinimapPosition = FVector2D(-24.0f, 24.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap", meta = (ClampMin = "64.0"))
	FVector2D MinimapSize = FVector2D(224.0f, 224.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap", meta = (ClampMin = "1"))
	int32 MinimapVisibleTileRadius = 6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap", meta = (ClampMin = "10.0"))
	float MinimapTileWorldSize = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap", meta = (ClampMin = "0.05"))
	float MinimapRefreshIntervalSeconds = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voxel")
	TObjectPtr<UMaterialInterface> SharedVoxelMaterial;
};
