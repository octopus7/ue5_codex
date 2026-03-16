// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CodexInvenPickupData.h"
#include "Components/ActorComponent.h"
#include "CodexInvenOwnershipComponent.generated.h"

USTRUCT()
struct FCodexInvenOwnedUniquePickup
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	int32 InstanceId = INDEX_NONE;

	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	ECodexInvenPickupType Type = ECodexInvenPickupType::CubeRed;
};

USTRUCT()
struct FCodexInvenInventorySlotData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	ECodexInvenPickupType PickupType = ECodexInvenPickupType::CubeRed;

	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	FText DisplayName;

	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	int32 Quantity = 0;

	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	bool bStackable = false;

	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	int32 UniqueInstanceId = INDEX_NONE;
};

DECLARE_MULTICAST_DELEGATE(FOnCodexInvenInventoryChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CODEXINVEN_API UCodexInvenOwnershipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCodexInvenOwnershipComponent();

	bool AddPickup(ECodexInvenPickupType InPickupType);
	int32 GetStackCount(ECodexInvenPickupType InPickupType) const;
	const TArray<FCodexInvenOwnedUniquePickup>& GetUniquePickups() const;
	TArray<FCodexInvenInventorySlotData> BuildInventorySnapshot() const;
	FText BuildDebugOwnershipText() const;

	FOnCodexInvenInventoryChanged OnInventoryChanged;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	TMap<ECodexInvenPickupType, int32> StackedPickupCounts;

	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	TArray<FCodexInvenOwnedUniquePickup> UniquePickups;

private:
	int32 GetUniqueCount(ECodexInvenPickupType InPickupType) const;
	FString BuildPickupChangeDebugMessage(ECodexInvenPickupType InPickupType, int32 InDelta, int32 InNewTotal) const;
	FString BuildUniquePickupDebugList(const TArray<FCodexInvenInventorySlotData>& InSnapshot, ECodexInvenPickupType InPickupType) const;

	int32 NextUniquePickupInstanceId = 1;
};
