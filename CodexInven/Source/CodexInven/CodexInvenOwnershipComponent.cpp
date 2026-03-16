// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenOwnershipComponent.h"

#include "Engine/Engine.h"

UCodexInvenOwnershipComponent::UCodexInvenOwnershipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UCodexInvenOwnershipComponent::AddPickup(const ECodexInvenPickupType InPickupType)
{
	const FCodexInvenPickupDefinition* Definition = CodexInvenPickupData::FindPickupDefinition(InPickupType);
	if (Definition == nullptr)
	{
		return false;
	}

	int32 NewTotal = 0;
	if (Definition->bStackable)
	{
		int32& StackCount = StackedPickupCounts.FindOrAdd(InPickupType);
		++StackCount;
		NewTotal = StackCount;
	}
	else
	{
		FCodexInvenOwnedUniquePickup& UniquePickup = UniquePickups.AddDefaulted_GetRef();
		UniquePickup.InstanceId = NextUniquePickupInstanceId++;
		UniquePickup.Type = InPickupType;
		NewTotal = GetUniqueCount(InPickupType);
	}

	OnOwnershipChanged.Broadcast(InPickupType, 1, NewTotal);

	if (GEngine != nullptr)
	{
		const uint64 MessageKey = static_cast<uint64>(reinterpret_cast<UPTRINT>(this));
		GEngine->AddOnScreenDebugMessage(MessageKey, 3.0f, FColor::White, BuildPickupChangeDebugMessage(InPickupType, 1, NewTotal));
	}

	return true;
}

int32 UCodexInvenOwnershipComponent::GetStackCount(const ECodexInvenPickupType InPickupType) const
{
	if (const int32* StackCount = StackedPickupCounts.Find(InPickupType))
	{
		return *StackCount;
	}

	return 0;
}

const TArray<FCodexInvenOwnedUniquePickup>& UCodexInvenOwnershipComponent::GetUniquePickups() const
{
	return UniquePickups;
}

int32 UCodexInvenOwnershipComponent::GetUniqueCount(const ECodexInvenPickupType InPickupType) const
{
	int32 UniqueCount = 0;
	for (const FCodexInvenOwnedUniquePickup& UniquePickup : UniquePickups)
	{
		if (UniquePickup.Type == InPickupType)
		{
			++UniqueCount;
		}
	}

	return UniqueCount;
}

int32 UCodexInvenOwnershipComponent::GetTotalForPickupType(const ECodexInvenPickupType InPickupType) const
{
	const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(InPickupType);
	return Definition.bStackable ? GetStackCount(InPickupType) : GetUniqueCount(InPickupType);
}

FString UCodexInvenOwnershipComponent::BuildPickupChangeDebugMessage(const ECodexInvenPickupType InPickupType, const int32 InDelta, const int32 InNewTotal) const
{
	const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(InPickupType);
	const TCHAR* TotalLabel = Definition.bStackable ? TEXT("Stack Total") : TEXT("Stored Total");
	return FString::Printf(TEXT("Picked up %s (+%d). %s: %d"), *Definition.DisplayName, InDelta, TotalLabel, InNewTotal);
}
