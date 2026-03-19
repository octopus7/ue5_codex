// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenOwnershipComponent.h"

#include "Engine/Engine.h"

namespace
{
	constexpr int32 DefaultInventoryCapacity = 100;
}

UCodexInvenOwnershipComponent::UCodexInvenOwnershipComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	InitializeInventoryCapacity(DefaultInventoryCapacity);
}

bool UCodexInvenOwnershipComponent::AddPickup(const ECodexInvenPickupType InPickupType)
{
	const FCodexInvenPickupDefinition* Definition = CodexInvenPickupData::FindPickupDefinition(InPickupType);
	if (Definition == nullptr)
	{
		return false;
	}

	int32 TargetSlotIndex = INDEX_NONE;
	if (Definition->bStackable)
	{
		TargetSlotIndex = FindStackableSlotIndex(InPickupType);
		if (TargetSlotIndex != INDEX_NONE)
		{
			++InventorySlots[TargetSlotIndex].Quantity;
		}
		else
		{
			TargetSlotIndex = FindFirstEmptyInventorySlotIndex();
			if (TargetSlotIndex == INDEX_NONE)
			{
				return false;
			}

			FCodexInvenInventorySlot& TargetSlot = InventorySlots[TargetSlotIndex];
			TargetSlot.bOccupied = true;
			TargetSlot.PickupType = InPickupType;
			TargetSlot.Quantity = 1;
			TargetSlot.bStackable = true;
			TargetSlot.Rarity = Definition->Rarity;
			TargetSlot.UniqueInstanceId = INDEX_NONE;
		}
	}
	else
	{
		TargetSlotIndex = FindFirstEmptyInventorySlotIndex();
		if (TargetSlotIndex == INDEX_NONE)
		{
			return false;
		}

		FCodexInvenInventorySlot& TargetSlot = InventorySlots[TargetSlotIndex];
		TargetSlot.bOccupied = true;
		TargetSlot.PickupType = InPickupType;
		TargetSlot.Quantity = 1;
		TargetSlot.bStackable = false;
		TargetSlot.Rarity = Definition->Rarity;
		TargetSlot.UniqueInstanceId = NextUniquePickupInstanceId++;
	}

	RebuildOwnershipCachesFromSlots();

	const int32 NewTotal = GetTotalForPickupType(InPickupType);
	OnInventoryChanged.Broadcast();

	if (GEngine != nullptr)
	{
		const uint64 MessageKey = static_cast<uint64>(reinterpret_cast<UPTRINT>(this));
		GEngine->AddOnScreenDebugMessage(MessageKey, 3.0f, FColor::White, BuildPickupChangeDebugMessage(InPickupType, 1, NewTotal));
	}

	return true;
}

bool UCodexInvenOwnershipComponent::CanAddPickup(const ECodexInvenPickupType InPickupType, const int32 InQuantity) const
{
	if (InQuantity <= 0)
	{
		return false;
	}

	const FCodexInvenPickupDefinition* Definition = CodexInvenPickupData::FindPickupDefinition(InPickupType);
	if (Definition == nullptr)
	{
		return false;
	}

	if (Definition->bStackable)
	{
		return FindStackableSlotIndex(InPickupType) != INDEX_NONE || FindFirstEmptyInventorySlotIndex() != INDEX_NONE;
	}

	int32 EmptySlotCount = 0;
	for (const FCodexInvenInventorySlot& InventorySlot : InventorySlots)
	{
		if (!InventorySlot.bOccupied)
		{
			++EmptySlotCount;
			if (EmptySlotCount >= InQuantity)
			{
				return true;
			}
		}
	}

	return false;
}

bool UCodexInvenOwnershipComponent::IncreaseInventoryCapacity(const int32 InAdditionalSlots)
{
	if (InAdditionalSlots <= 0)
	{
		return false;
	}

	InventorySlots.AddDefaulted(InAdditionalSlots);
	OnInventoryChanged.Broadcast();
	return true;
}

bool UCodexInvenOwnershipComponent::ClearInventorySlot(const int32 InSlotIndex)
{
	if (!IsValidInventorySlotIndex(InSlotIndex) || !InventorySlots[InSlotIndex].bOccupied)
	{
		return false;
	}

	ResetInventorySlot(InventorySlots[InSlotIndex]);
	RebuildOwnershipCachesFromSlots();
	OnInventoryChanged.Broadcast();
	return true;
}

bool UCodexInvenOwnershipComponent::MoveInventorySlot(const int32 InFromSlotIndex, const int32 InToSlotIndex)
{
	if (!IsValidInventorySlotIndex(InFromSlotIndex) || !IsValidInventorySlotIndex(InToSlotIndex) || InFromSlotIndex == InToSlotIndex)
	{
		return false;
	}

	if (!InventorySlots[InFromSlotIndex].bOccupied || InventorySlots[InToSlotIndex].bOccupied)
	{
		return false;
	}

	InventorySlots[InToSlotIndex] = InventorySlots[InFromSlotIndex];
	ResetInventorySlot(InventorySlots[InFromSlotIndex]);
	RebuildOwnershipCachesFromSlots();
	OnInventoryChanged.Broadcast();
	return true;
}

bool UCodexInvenOwnershipComponent::SwapInventorySlots(const int32 InFirstSlotIndex, const int32 InSecondSlotIndex)
{
	if (!IsValidInventorySlotIndex(InFirstSlotIndex) || !IsValidInventorySlotIndex(InSecondSlotIndex) || InFirstSlotIndex == InSecondSlotIndex)
	{
		return false;
	}

	Swap(InventorySlots[InFirstSlotIndex], InventorySlots[InSecondSlotIndex]);
	RebuildOwnershipCachesFromSlots();
	OnInventoryChanged.Broadcast();
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

int32 UCodexInvenOwnershipComponent::GetInventoryCapacity() const
{
	return InventorySlots.Num();
}

int32 UCodexInvenOwnershipComponent::GetOccupiedSlotCount() const
{
	int32 OccupiedSlotCount = 0;
	for (const FCodexInvenInventorySlot& Slot : InventorySlots)
	{
		if (Slot.bOccupied)
		{
			++OccupiedSlotCount;
		}
	}

	return OccupiedSlotCount;
}

const TArray<FCodexInvenOwnedUniquePickup>& UCodexInvenOwnershipComponent::GetUniquePickups() const
{
	return UniquePickups;
}

TArray<FCodexInvenInventorySlotData> UCodexInvenOwnershipComponent::BuildInventorySnapshot() const
{
	TArray<FCodexInvenInventorySlotData> Snapshot;
	Snapshot.Reserve(InventorySlots.Num());

	for (int32 SlotIndex = 0; SlotIndex < InventorySlots.Num(); ++SlotIndex)
	{
		const FCodexInvenInventorySlot& Slot = InventorySlots[SlotIndex];
		FCodexInvenInventorySlotData& SlotData = Snapshot.AddDefaulted_GetRef();
		SlotData.SlotIndex = SlotIndex;
		SlotData.bIsEmpty = !Slot.bOccupied;

		if (!Slot.bOccupied)
		{
			continue;
		}

		const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(Slot.PickupType);
		SlotData.PickupType = Slot.PickupType;
		SlotData.DisplayName = FText::FromString(Definition.DisplayName);
		SlotData.Quantity = Slot.Quantity;
		SlotData.bStackable = Slot.bStackable;
		SlotData.Rarity = Definition.Rarity;
		SlotData.UniqueInstanceId = Slot.UniqueInstanceId;
	}

	return Snapshot;
}

FText UCodexInvenOwnershipComponent::BuildDebugOwnershipText() const
{
	const TArray<FCodexInvenInventorySlotData> Snapshot = BuildInventorySnapshot();

	int32 TotalStackedItems = 0;
	int32 TotalUniqueItems = 0;
	for (const FCodexInvenInventorySlotData& SlotData : Snapshot)
	{
		if (SlotData.bIsEmpty)
		{
			continue;
		}

		if (SlotData.bStackable)
		{
			TotalStackedItems += SlotData.Quantity;
		}
		else
		{
			++TotalUniqueItems;
		}
	}

	TArray<FString> Lines;
	Lines.Reserve(16);
	Lines.Add(TEXT("Ownership Debug"));
	Lines.Add(TEXT("Consumables"));
	for (const ECodexInvenPickupType PickupType : CodexInvenPickupData::GetAllPickupTypes())
	{
		const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(PickupType);
		if (!Definition.bStackable)
		{
			continue;
		}

		Lines.Add(FString::Printf(TEXT("%s: %d"), *Definition.DisplayName, GetStackCount(PickupType)));
	}

	Lines.Add(TEXT(""));
	Lines.Add(TEXT("Equipment"));
	for (const ECodexInvenPickupType PickupType : CodexInvenPickupData::GetAllPickupTypes())
	{
		const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(PickupType);
		if (Definition.bStackable)
		{
			continue;
		}

		Lines.Add(FString::Printf(TEXT("%s: %s"), *Definition.DisplayName, *BuildUniquePickupDebugList(Snapshot, PickupType)));
	}

	Lines.Add(TEXT(""));
	Lines.Add(TEXT("Totals"));
	Lines.Add(FString::Printf(TEXT("Stacked Items: %d"), TotalStackedItems));
	Lines.Add(FString::Printf(TEXT("Unique Items: %d"), TotalUniqueItems));

	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}

void UCodexInvenOwnershipComponent::InitializeInventoryCapacity(const int32 InCapacity)
{
	InventorySlots.Reset();
	if (InCapacity > 0)
	{
		InventorySlots.AddDefaulted(InCapacity);
	}
}

bool UCodexInvenOwnershipComponent::IsValidInventorySlotIndex(const int32 InSlotIndex) const
{
	return InventorySlots.IsValidIndex(InSlotIndex);
}

int32 UCodexInvenOwnershipComponent::FindFirstEmptyInventorySlotIndex() const
{
	for (int32 SlotIndex = 0; SlotIndex < InventorySlots.Num(); ++SlotIndex)
	{
		if (!InventorySlots[SlotIndex].bOccupied)
		{
			return SlotIndex;
		}
	}

	return INDEX_NONE;
}

int32 UCodexInvenOwnershipComponent::FindStackableSlotIndex(const ECodexInvenPickupType InPickupType) const
{
	for (int32 SlotIndex = 0; SlotIndex < InventorySlots.Num(); ++SlotIndex)
	{
		const FCodexInvenInventorySlot& Slot = InventorySlots[SlotIndex];
		if (Slot.bOccupied && Slot.bStackable && Slot.PickupType == InPickupType)
		{
			return SlotIndex;
		}
	}

	return INDEX_NONE;
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

void UCodexInvenOwnershipComponent::ResetInventorySlot(FCodexInvenInventorySlot& InSlot) const
{
	InSlot.bOccupied = false;
	InSlot.PickupType = ECodexInvenPickupType::CubeRed;
	InSlot.Quantity = 0;
	InSlot.bStackable = false;
	InSlot.Rarity = ECodexInvenPickupRarity::Common;
	InSlot.UniqueInstanceId = INDEX_NONE;
}

void UCodexInvenOwnershipComponent::RebuildOwnershipCachesFromSlots()
{
	StackedPickupCounts.Reset();
	UniquePickups.Reset();

	for (const FCodexInvenInventorySlot& Slot : InventorySlots)
	{
		if (!Slot.bOccupied)
		{
			continue;
		}

		if (Slot.bStackable)
		{
			StackedPickupCounts.FindOrAdd(Slot.PickupType) += Slot.Quantity;
			continue;
		}

		FCodexInvenOwnedUniquePickup& UniquePickup = UniquePickups.AddDefaulted_GetRef();
		UniquePickup.InstanceId = Slot.UniqueInstanceId;
		UniquePickup.Type = Slot.PickupType;
	}

	UniquePickups.Sort([](const FCodexInvenOwnedUniquePickup& Left, const FCodexInvenOwnedUniquePickup& Right)
	{
		return Left.InstanceId < Right.InstanceId;
	});
}

FString UCodexInvenOwnershipComponent::BuildPickupChangeDebugMessage(const ECodexInvenPickupType InPickupType, const int32 InDelta, const int32 InNewTotal) const
{
	const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(InPickupType);
	const TCHAR* TotalLabel = Definition.bStackable ? TEXT("Stack Total") : TEXT("Stored Total");
	return FString::Printf(TEXT("Picked up %s (+%d). %s: %d"), *Definition.DisplayName, InDelta, TotalLabel, InNewTotal);
}

FString UCodexInvenOwnershipComponent::BuildUniquePickupDebugList(const TArray<FCodexInvenInventorySlotData>& InSnapshot, const ECodexInvenPickupType InPickupType) const
{
	TArray<int32> MatchingInstanceIds;
	for (const FCodexInvenInventorySlotData& SlotData : InSnapshot)
	{
		if (!SlotData.bIsEmpty && !SlotData.bStackable && SlotData.PickupType == InPickupType && SlotData.UniqueInstanceId != INDEX_NONE)
		{
			MatchingInstanceIds.Add(SlotData.UniqueInstanceId);
		}
	}

	if (MatchingInstanceIds.IsEmpty())
	{
		return TEXT("None");
	}

	MatchingInstanceIds.Sort();

	TArray<FString> Tokens;
	Tokens.Reserve(MatchingInstanceIds.Num());
	for (const int32 InstanceId : MatchingInstanceIds)
	{
		Tokens.Add(FString::Printf(TEXT("#%d"), InstanceId));
	}

	return FString::Join(Tokens, TEXT(", "));
}
