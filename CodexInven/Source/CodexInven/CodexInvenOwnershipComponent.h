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

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnCodexInvenOwnershipChanged, ECodexInvenPickupType, int32, int32);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CODEXINVEN_API UCodexInvenOwnershipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCodexInvenOwnershipComponent();

	bool AddPickup(ECodexInvenPickupType InPickupType);
	int32 GetStackCount(ECodexInvenPickupType InPickupType) const;
	const TArray<FCodexInvenOwnedUniquePickup>& GetUniquePickups() const;
	FText BuildDebugOwnershipText() const;

	FOnCodexInvenOwnershipChanged OnOwnershipChanged;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	TMap<ECodexInvenPickupType, int32> StackedPickupCounts;

	UPROPERTY(VisibleAnywhere, Category = "Ownership")
	TArray<FCodexInvenOwnedUniquePickup> UniquePickups;

private:
	int32 GetUniqueCount(ECodexInvenPickupType InPickupType) const;
	int32 GetTotalForPickupType(ECodexInvenPickupType InPickupType) const;
	FString BuildPickupChangeDebugMessage(ECodexInvenPickupType InPickupType, int32 InDelta, int32 InNewTotal) const;
	FString BuildUniquePickupDebugList(ECodexInvenPickupType InPickupType) const;

	int32 NextUniquePickupInstanceId = 1;
};
