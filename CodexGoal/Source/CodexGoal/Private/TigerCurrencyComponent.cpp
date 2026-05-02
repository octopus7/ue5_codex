// Copyright Epic Games, Inc. All Rights Reserved.

#include "TigerCurrencyComponent.h"

#include "Math/UnrealMathUtility.h"

UTigerCurrencyComponent::UTigerCurrencyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTigerCurrencyComponent::AddRiceCakes(int64 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	RiceCakeCount += Amount;
	OnCurrencyChanged.Broadcast();
}

bool UTigerCurrencyComponent::TryBuyClickUpgrade()
{
	const int64 Cost = GetNextClickUpgradeCost();
	if (RiceCakeCount < Cost)
	{
		return false;
	}

	RiceCakeCount -= Cost;
	++ClickUpgradeLevel;
	++ClickPower;
	OnCurrencyChanged.Broadcast();
	return true;
}

void UTigerCurrencyComponent::SetState(int64 InRiceCakeCount, int32 InClickPower, int32 InClickUpgradeLevel)
{
	RiceCakeCount = FMath::Max<int64>(0, InRiceCakeCount);
	ClickPower = FMath::Max(1, InClickPower);
	ClickUpgradeLevel = FMath::Max(0, InClickUpgradeLevel);
	OnCurrencyChanged.Broadcast();
}

bool UTigerCurrencyComponent::CanBuyClickUpgrade() const
{
	return RiceCakeCount >= GetNextClickUpgradeCost();
}

int64 UTigerCurrencyComponent::GetNextClickUpgradeCost() const
{
	const int64 LinearCost = BaseClickUpgradeCost + static_cast<int64>(ClickUpgradeLevel) * 15;
	return FMath::Max<int64>(BaseClickUpgradeCost, LinearCost);
}
