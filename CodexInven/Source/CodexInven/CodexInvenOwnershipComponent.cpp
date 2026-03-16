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

	OnInventoryChanged.Broadcast();

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

TArray<FCodexInvenInventorySlotData> UCodexInvenOwnershipComponent::BuildInventorySnapshot() const
{
	TArray<FCodexInvenInventorySlotData> Snapshot;
	Snapshot.Reserve(CodexInvenPickupData::GetAllPickupTypes().Num() + UniquePickups.Num());

	for (const ECodexInvenPickupType PickupType : CodexInvenPickupData::GetAllPickupTypes())
	{
		const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(PickupType);

		if (Definition.bStackable)
		{
			const int32 Quantity = GetStackCount(PickupType);
			if (Quantity <= 0)
			{
				continue;
			}

			FCodexInvenInventorySlotData& SlotData = Snapshot.AddDefaulted_GetRef();
			SlotData.PickupType = PickupType;
			SlotData.DisplayName = FText::FromString(Definition.DisplayName);
			SlotData.Quantity = Quantity;
			SlotData.bStackable = true;
			SlotData.UniqueInstanceId = INDEX_NONE;
			continue;
		}

		TArray<int32> MatchingInstanceIds;
		for (const FCodexInvenOwnedUniquePickup& UniquePickup : UniquePickups)
		{
			if (UniquePickup.Type == PickupType)
			{
				MatchingInstanceIds.Add(UniquePickup.InstanceId);
			}
		}

		MatchingInstanceIds.Sort();

		for (const int32 InstanceId : MatchingInstanceIds)
		{
			FCodexInvenInventorySlotData& SlotData = Snapshot.AddDefaulted_GetRef();
			SlotData.PickupType = PickupType;
			SlotData.DisplayName = FText::FromString(Definition.DisplayName);
			SlotData.Quantity = 1;
			SlotData.bStackable = false;
			SlotData.UniqueInstanceId = InstanceId;
		}
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
	Lines.Reserve(14);
	Lines.Add(TEXT("Ownership Debug"));
	Lines.Add(TEXT("Stacks"));
	Lines.Add(FString::Printf(TEXT("Cylinder Red: %d"), GetStackCount(ECodexInvenPickupType::CylinderRed)));
	Lines.Add(FString::Printf(TEXT("Cylinder Green: %d"), GetStackCount(ECodexInvenPickupType::CylinderGreen)));
	Lines.Add(FString::Printf(TEXT("Cylinder Blue: %d"), GetStackCount(ECodexInvenPickupType::CylinderBlue)));
	Lines.Add(TEXT(""));
	Lines.Add(TEXT("Unique Cubes"));
	Lines.Add(FString::Printf(TEXT("Cube Red: %s"), *BuildUniquePickupDebugList(Snapshot, ECodexInvenPickupType::CubeRed)));
	Lines.Add(FString::Printf(TEXT("Cube Green: %s"), *BuildUniquePickupDebugList(Snapshot, ECodexInvenPickupType::CubeGreen)));
	Lines.Add(FString::Printf(TEXT("Cube Blue: %s"), *BuildUniquePickupDebugList(Snapshot, ECodexInvenPickupType::CubeBlue)));
	Lines.Add(TEXT(""));
	Lines.Add(TEXT("Totals"));
	Lines.Add(FString::Printf(TEXT("Stacked Items: %d"), TotalStackedItems));
	Lines.Add(FString::Printf(TEXT("Unique Items: %d"), TotalUniqueItems));

	return FText::FromString(FString::Join(Lines, TEXT("\n")));
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
		if (!SlotData.bStackable && SlotData.PickupType == InPickupType && SlotData.UniqueInstanceId != INDEX_NONE)
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
