// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionDualTileTransferPopupWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Interaction/CodexInteractionAssetPaths.h"
#include "Interaction/CodexInteractionDualTileTransferTileEntryWidget.h"
#include "Interaction/CodexInteractionDualTileTransferTileItem.h"
#include "Interaction/CodexInteractionSubsystem.h"
#include "Interaction/CodexTileTransferDragDropOperation.h"

namespace
{
	constexpr int32 MinTileNumber = 1;
	constexpr int32 MaxTileNumber = 99;
	constexpr int32 MinVisibleSlotCount = 80;

	bool IsValidTileNumber(const int32 Number)
	{
		return Number >= MinTileNumber && Number <= MaxTileNumber;
	}

	const TArray<FLinearColor>& GetPastelPalette()
	{
		static const TArray<FLinearColor> Palette =
		{
			FLinearColor(0.69f, 0.86f, 0.98f, 1.0f),
			FLinearColor(0.67f, 0.93f, 0.85f, 1.0f),
			FLinearColor(0.98f, 0.90f, 0.62f, 1.0f),
			FLinearColor(0.98f, 0.76f, 0.84f, 1.0f),
			FLinearColor(0.99f, 0.79f, 0.64f, 1.0f),
			FLinearColor(0.78f, 0.92f, 0.61f, 1.0f),
			FLinearColor(0.81f, 0.79f, 0.98f, 1.0f),
			FLinearColor(0.78f, 0.92f, 0.96f, 1.0f),
			FLinearColor(0.99f, 0.83f, 0.66f, 1.0f),
			FLinearColor(0.90f, 0.82f, 0.98f, 1.0f)
		};

		return Palette;
	}

	FLinearColor ResolveTintColorForNumber(const int32 Number)
	{
		const TArray<FLinearColor>& Palette = GetPastelPalette();
		const int32 PaletteIndex = FMath::Abs(Number - MinTileNumber) % Palette.Num();
		return Palette[PaletteIndex];
	}
}

void UCodexInteractionDualTileTransferPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_Close != nullptr)
	{
		BTN_Close->OnClicked.RemoveAll(this);
		BTN_Close->OnClicked.AddDynamic(this, &UCodexInteractionDualTileTransferPopupWidget::HandleCloseClicked);
	}

	if (BTN_LeftAdd != nullptr)
	{
		BTN_LeftAdd->OnClicked.RemoveAll(this);
		BTN_LeftAdd->OnClicked.AddDynamic(this, &UCodexInteractionDualTileTransferPopupWidget::HandleLeftAddClicked);
	}

	if (BTN_LeftRemove != nullptr)
	{
		BTN_LeftRemove->OnClicked.RemoveAll(this);
		BTN_LeftRemove->OnClicked.AddDynamic(this, &UCodexInteractionDualTileTransferPopupWidget::HandleLeftRemoveClicked);
	}

	if (BTN_RightAdd != nullptr)
	{
		BTN_RightAdd->OnClicked.RemoveAll(this);
		BTN_RightAdd->OnClicked.AddDynamic(this, &UCodexInteractionDualTileTransferPopupWidget::HandleRightAddClicked);
	}

	if (BTN_RightRemove != nullptr)
	{
		BTN_RightRemove->OnClicked.RemoveAll(this);
		BTN_RightRemove->OnClicked.AddDynamic(this, &UCodexInteractionDualTileTransferPopupWidget::HandleRightRemoveClicked);
	}

	if (TileView_Left != nullptr)
	{
		TileView_Left->SetSelectionMode(ESelectionMode::Single);
		TileView_Left->OnItemSelectionChanged().RemoveAll(this);
		TileView_Left->OnItemSelectionChanged().AddUObject(this, &UCodexInteractionDualTileTransferPopupWidget::HandleLeftSelectionChanged);
		TileView_Left->OnGetEntryClassForItem().BindUObject(this, &UCodexInteractionDualTileTransferPopupWidget::ResolveTileEntryWidgetClassForItem);
	}

	if (TileView_Right != nullptr)
	{
		TileView_Right->SetSelectionMode(ESelectionMode::Single);
		TileView_Right->OnItemSelectionChanged().RemoveAll(this);
		TileView_Right->OnItemSelectionChanged().AddUObject(this, &UCodexInteractionDualTileTransferPopupWidget::HandleRightSelectionChanged);
		TileView_Right->OnGetEntryClassForItem().BindUObject(this, &UCodexInteractionDualTileTransferPopupWidget::ResolveTileEntryWidgetClassForItem);
	}

	RefreshAllState();
}

bool UCodexInteractionDualTileTransferPopupWidget::NativeOnDragOver(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	(void)InGeometry;

	const UCodexTileTransferDragDropOperation* Operation = Cast<UCodexTileTransferDragDropOperation>(InOperation);
	UCodexInteractionDualTileTransferTileItem* TargetItem =
		Operation != nullptr ? FindDropTargetItemAtScreenPosition(InDragDropEvent.GetScreenSpacePosition()) : nullptr;

	if (TargetItem != nullptr && CanDropOnSlot(TargetItem, Operation))
	{
		SetHoveredDropTargetItem(TargetItem);
		return true;
	}

	ClearHoveredDropTargetItem();
	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool UCodexInteractionDualTileTransferPopupWidget::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	(void)InGeometry;

	UCodexInteractionDualTileTransferTileItem* TargetItem = FindDropTargetItemAtScreenPosition(InDragDropEvent.GetScreenSpacePosition());
	ClearHoveredDropTargetItem();

	if (TryHandleDropOnSlot(TargetItem, InOperation))
	{
		return true;
	}

	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UCodexInteractionDualTileTransferPopupWidget::NativeOnDragLeave(
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	ClearHoveredDropTargetItem();
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

void UCodexInteractionDualTileTransferPopupWidget::ApplyPopupRequest(
	const FCodexInteractionPopupRequest& NewRequest,
	UCodexInteractionSubsystem& InInteractionSubsystem)
{
	ActiveRequest = NewRequest;
	InteractionSubsystem = &InInteractionSubsystem;
	RebuildTileItemsFromRequest();
	RefreshAllState();
}

void UCodexInteractionDualTileTransferPopupWidget::HandleControllerCloseRequested()
{
	SubmitPopupResult(ECodexPopupResult::Closed);
}

void UCodexInteractionDualTileTransferPopupWidget::NotifyTileDragStarted()
{
	ClearHoveredDropTargetItem();
}

bool UCodexInteractionDualTileTransferPopupWidget::CanDropOnSlot(
	const UCodexInteractionDualTileTransferTileItem* TargetItem,
	const UCodexTileTransferDragDropOperation* Operation) const
{
	if (TargetItem == nullptr || Operation == nullptr || Operation->Item == nullptr)
	{
		return false;
	}

	const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& SourceItems = GetPanelItems(Operation->SourcePanelSide);
	const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& TargetItems = GetPanelItems(TargetItem->PanelSide);
	if (!TargetItems.Contains(TargetItem))
	{
		return false;
	}

	UCodexInteractionDualTileTransferTileItem* SourceItem = nullptr;
	if (SourceItems.IsValidIndex(Operation->SourceIndex))
	{
		SourceItem = SourceItems[Operation->SourceIndex];
	}

	if (SourceItem == nullptr || SourceItem != Operation->Item)
	{
		SourceItem = Operation->Item;
	}

	if (SourceItem == nullptr || !SourceItems.Contains(SourceItem) || !SourceItem->HasNumber())
	{
		return false;
	}

	return !(SourceItem->PanelSide == TargetItem->PanelSide && SourceItem->SlotIndex == TargetItem->SlotIndex);
}

bool UCodexInteractionDualTileTransferPopupWidget::TryHandleDropOnSlot(
	UCodexInteractionDualTileTransferTileItem* TargetItem,
	UDragDropOperation* InOperation)
{
	ClearHoveredDropTargetItem();

	UCodexTileTransferDragDropOperation* Operation = Cast<UCodexTileTransferDragDropOperation>(InOperation);
	if (!CanDropOnSlot(TargetItem, Operation) || TargetItem == nullptr || Operation == nullptr)
	{
		return false;
	}

	return MoveTileToSlot(*Operation, TargetItem->PanelSide, TargetItem->SlotIndex);
}

void UCodexInteractionDualTileTransferPopupWidget::HandleCloseClicked()
{
	SubmitPopupResult(ECodexPopupResult::Closed);
}

void UCodexInteractionDualTileTransferPopupWidget::HandleLeftAddClicked()
{
	AddTileToPanel(ECodexTileTransferPanelSide::Left);
}

void UCodexInteractionDualTileTransferPopupWidget::HandleLeftRemoveClicked()
{
	RemoveSelectedTile(ECodexTileTransferPanelSide::Left);
}

void UCodexInteractionDualTileTransferPopupWidget::HandleRightAddClicked()
{
	AddTileToPanel(ECodexTileTransferPanelSide::Right);
}

void UCodexInteractionDualTileTransferPopupWidget::HandleRightRemoveClicked()
{
	RemoveSelectedTile(ECodexTileTransferPanelSide::Right);
}

void UCodexInteractionDualTileTransferPopupWidget::HandleLeftSelectionChanged(UObject* SelectedItem)
{
	UCodexInteractionDualTileTransferTileItem* TileItem = Cast<UCodexInteractionDualTileTransferTileItem>(SelectedItem);
	if (TileItem != nullptr && !TileItem->HasNumber())
	{
		if (TileView_Left != nullptr)
		{
			TileView_Left->ClearSelection();
		}
		SetSelectedItem(ECodexTileTransferPanelSide::Left, nullptr);
		return;
	}

	SetSelectedItem(ECodexTileTransferPanelSide::Left, TileItem);
}

void UCodexInteractionDualTileTransferPopupWidget::HandleRightSelectionChanged(UObject* SelectedItem)
{
	UCodexInteractionDualTileTransferTileItem* TileItem = Cast<UCodexInteractionDualTileTransferTileItem>(SelectedItem);
	if (TileItem != nullptr && !TileItem->HasNumber())
	{
		if (TileView_Right != nullptr)
		{
			TileView_Right->ClearSelection();
		}
		SetSelectedItem(ECodexTileTransferPanelSide::Right, nullptr);
		return;
	}

	SetSelectedItem(ECodexTileTransferPanelSide::Right, TileItem);
}

void UCodexInteractionDualTileTransferPopupWidget::RefreshAllState()
{
	if (LeftSelectedItem != nullptr && (!LeftItems.Contains(LeftSelectedItem) || !LeftSelectedItem->HasNumber()))
	{
		LeftSelectedItem = nullptr;
	}

	if (RightSelectedItem != nullptr && (!RightItems.Contains(RightSelectedItem) || !RightSelectedItem->HasNumber()))
	{
		RightSelectedItem = nullptr;
	}

	if (HoveredDropTargetItem != nullptr
		&& ((!LeftItems.Contains(HoveredDropTargetItem) && !RightItems.Contains(HoveredDropTargetItem))
			|| !HoveredDropTargetItem->HasNumber()))
	{
		HoveredDropTargetItem = nullptr;
	}

	RefreshTitle();
	RefreshSelectionFlags();
	RefreshTileViews();
	RefreshButtonState();
}

void UCodexInteractionDualTileTransferPopupWidget::RefreshTitle() const
{
	if (TXT_Title != nullptr)
	{
		TXT_Title->SetText(ActiveRequest.Title);
	}
}

void UCodexInteractionDualTileTransferPopupWidget::RefreshTileViews()
{
	if (ResolveTileEntryWidgetClass() == nullptr)
	{
		return;
	}

	if (TileView_Left != nullptr)
	{
		TileView_Left->SetListItems(BuildListItems(LeftItems));
		TileView_Left->RegenerateAllEntries();
		if (LeftSelectedItem != nullptr)
		{
			TileView_Left->SetSelectedItem(LeftSelectedItem);
		}
		else
		{
			TileView_Left->ClearSelection();
		}
	}

	if (TileView_Right != nullptr)
	{
		TileView_Right->SetListItems(BuildListItems(RightItems));
		TileView_Right->RegenerateAllEntries();
		if (RightSelectedItem != nullptr)
		{
			TileView_Right->SetSelectedItem(RightSelectedItem);
		}
		else
		{
			TileView_Right->ClearSelection();
		}
	}
}

void UCodexInteractionDualTileTransferPopupWidget::RefreshButtonState() const
{
	if (BTN_LeftAdd != nullptr)
	{
		BTN_LeftAdd->SetIsEnabled(FindNextNumberForPanel(ECodexTileTransferPanelSide::Left) != INDEX_NONE);
	}

	if (BTN_RightAdd != nullptr)
	{
		BTN_RightAdd->SetIsEnabled(FindNextNumberForPanel(ECodexTileTransferPanelSide::Right) != INDEX_NONE);
	}

	if (BTN_LeftRemove != nullptr)
	{
		BTN_LeftRemove->SetIsEnabled(LeftSelectedItem != nullptr && LeftSelectedItem->HasNumber());
	}

	if (BTN_RightRemove != nullptr)
	{
		BTN_RightRemove->SetIsEnabled(RightSelectedItem != nullptr && RightSelectedItem->HasNumber());
	}
}

void UCodexInteractionDualTileTransferPopupWidget::RefreshSelectionFlags()
{
	for (UCodexInteractionDualTileTransferTileItem* Item : LeftItems)
	{
		if (Item != nullptr)
		{
			Item->bIsSelected = Item->HasNumber() && Item == LeftSelectedItem;
		}
	}

	for (UCodexInteractionDualTileTransferTileItem* Item : RightItems)
	{
		if (Item != nullptr)
		{
			Item->bIsSelected = Item->HasNumber() && Item == RightSelectedItem;
		}
	}
}

void UCodexInteractionDualTileTransferPopupWidget::SetSelectedItem(
	const ECodexTileTransferPanelSide PanelSide,
	UCodexInteractionDualTileTransferTileItem* Item)
{
	TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& PanelItems = GetPanelItems(PanelSide);
	if (Item != nullptr && (!PanelItems.Contains(Item) || !Item->HasNumber()))
	{
		Item = nullptr;
	}

	if (PanelSide == ECodexTileTransferPanelSide::Left)
	{
		LeftSelectedItem = Item;
	}
	else
	{
		RightSelectedItem = Item;
	}

	RefreshSelectionFlags();
	RefreshButtonState();
}

void UCodexInteractionDualTileTransferPopupWidget::ClearSelection(const ECodexTileTransferPanelSide PanelSide)
{
	SetSelectedItem(PanelSide, nullptr);
}

bool UCodexInteractionDualTileTransferPopupWidget::MoveTileToSlot(
	UCodexTileTransferDragDropOperation& Operation,
	const ECodexTileTransferPanelSide TargetSide,
	const int32 TargetSlotIndex)
{
	ClearHoveredDropTargetItem();

	TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& SourceItems = GetPanelItems(Operation.SourcePanelSide);
	TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& TargetItems = GetPanelItems(TargetSide);

	int32 SourceSlotIndex = Operation.SourceIndex;
	if (!SourceItems.IsValidIndex(SourceSlotIndex) || SourceItems[SourceSlotIndex] != Operation.Item)
	{
		SourceSlotIndex = SourceItems.IndexOfByKey(Operation.Item);
	}

	if (!SourceItems.IsValidIndex(SourceSlotIndex) || !TargetItems.IsValidIndex(TargetSlotIndex))
	{
		return false;
	}

	UCodexInteractionDualTileTransferTileItem* SourceItem = SourceItems[SourceSlotIndex];
	UCodexInteractionDualTileTransferTileItem* TargetItem = TargetItems[TargetSlotIndex];
	if (SourceItem == nullptr || TargetItem == nullptr || !SourceItem->HasNumber())
	{
		return false;
	}

	if (SourceItem == TargetItem)
	{
		return false;
	}

	const int32 SourceNumber = SourceItem->Number;
	const int32 TargetNumber = TargetItem->HasNumber() ? TargetItem->Number : 0;

	ApplyTileContent(*TargetItem, SourceNumber);

	if (TargetNumber > 0)
	{
		ApplyTileContent(*SourceItem, TargetNumber);
	}
	else
	{
		ClearTileContent(*SourceItem);
	}

	if (Operation.SourcePanelSide != TargetSide)
	{
		ClearSelection(Operation.SourcePanelSide);
	}

	SetSelectedItem(TargetSide, TargetItem);
	RefreshAllState();
	return true;
}

void UCodexInteractionDualTileTransferPopupWidget::AddTileToPanel(const ECodexTileTransferPanelSide TargetSide)
{
	const int32 NewNumber = FindNextNumberForPanel(TargetSide);
	if (NewNumber == INDEX_NONE)
	{
		return;
	}

	int32 EmptySlotIndex = FindFirstEmptySlot(TargetSide);
	if (EmptySlotIndex == INDEX_NONE)
	{
		EmptySlotIndex = AppendEmptySlot(TargetSide);
	}

	TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& TargetItems = GetPanelItems(TargetSide);
	if (!TargetItems.IsValidIndex(EmptySlotIndex) || TargetItems[EmptySlotIndex] == nullptr)
	{
		return;
	}

	ApplyTileContent(*TargetItems[EmptySlotIndex], NewNumber);
	SetSelectedItem(TargetSide, TargetItems[EmptySlotIndex]);
	RefreshAllState();
}

void UCodexInteractionDualTileTransferPopupWidget::RemoveSelectedTile(const ECodexTileTransferPanelSide TargetSide)
{
	UCodexInteractionDualTileTransferTileItem* SelectedItem = GetSelectedItem(TargetSide);
	if (SelectedItem == nullptr || !SelectedItem->HasNumber())
	{
		return;
	}

	const int32 RemovedSlotIndex = SelectedItem->SlotIndex;
	ClearTileContent(*SelectedItem);

	UCodexInteractionDualTileTransferTileItem* NextSelectedItem = nullptr;
	TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& TargetItems = GetPanelItems(TargetSide);

	const int32 NextOccupiedSlotIndex = FindNextOccupiedSlot(TargetSide, RemovedSlotIndex);
	if (TargetItems.IsValidIndex(NextOccupiedSlotIndex))
	{
		NextSelectedItem = TargetItems[NextOccupiedSlotIndex];
	}
	else
	{
		const int32 PreviousOccupiedSlotIndex = FindPreviousOccupiedSlot(TargetSide, RemovedSlotIndex);
		if (TargetItems.IsValidIndex(PreviousOccupiedSlotIndex))
		{
			NextSelectedItem = TargetItems[PreviousOccupiedSlotIndex];
		}
	}

	SetSelectedItem(TargetSide, NextSelectedItem);
	RefreshAllState();
}

void UCodexInteractionDualTileTransferPopupWidget::SubmitPopupResult(const ECodexPopupResult Result)
{
	if (InteractionSubsystem == nullptr)
	{
		return;
	}

	FCodexInteractionPopupResponse Response;
	Response.RequestId = ActiveRequest.RequestId;
	Response.InteractionRequest = ActiveRequest.InteractionRequest;
	Response.Result = Result;
	Response.LeftNumbers = BuildResultNumbers(LeftItems);
	Response.RightNumbers = BuildResultNumbers(RightItems);
	Response.bWasClosed = Result == ECodexPopupResult::Closed;
	InteractionSubsystem->SubmitInteractionPopupResult(Response);
}

UCodexInteractionDualTileTransferTileItem* UCodexInteractionDualTileTransferPopupWidget::CreateSlotItem(
	const ECodexTileTransferPanelSide PanelSide,
	const int32 SlotIndex)
{
	UCodexInteractionDualTileTransferTileItem* Item = NewObject<UCodexInteractionDualTileTransferTileItem>(this);
	Item->PanelSide = PanelSide;
	Item->SlotIndex = SlotIndex;
	ClearTileContent(*Item);
	return Item;
}

void UCodexInteractionDualTileTransferPopupWidget::ApplyTileContent(
	UCodexInteractionDualTileTransferTileItem& Item,
	const int32 Number)
{
	Item.Number = Number;
	Item.TintColor = ResolveTintColorForNumber(Number);
	Item.bIsEmpty = false;
}

void UCodexInteractionDualTileTransferPopupWidget::ClearTileContent(UCodexInteractionDualTileTransferTileItem& Item)
{
	Item.Number = 0;
	Item.TintColor = FLinearColor::White;
	Item.bIsEmpty = true;
	Item.bIsSelected = false;
}

void UCodexInteractionDualTileTransferPopupWidget::RebuildTileItemsFromRequest()
{
	LeftItems.Reset();
	RightItems.Reset();
	LeftSelectedItem = nullptr;
	RightSelectedItem = nullptr;
	HoveredDropTargetItem = nullptr;

	TSet<int32> GlobalUsedNumbers;

	auto NormalizePanelNumbers =
		[this, &GlobalUsedNumbers](const TArray<int32>& RequestedNumbers)
		{
			const bool bUseExplicitSlots = RequestedNumbers.Contains(0);
			TArray<int32> NormalizedNumbers;
			TSet<int32> PanelUsedNumbers;

			auto TryAcceptNumber =
				[this, &GlobalUsedNumbers, &PanelUsedNumbers](const int32 CandidateNumber, int32& OutNumber)
				{
					OutNumber = 0;
					if (!IsValidTileNumber(CandidateNumber))
					{
						return;
					}

					if (ActiveRequest.bAllowDuplicateNumbers)
					{
						if (PanelUsedNumbers.Contains(CandidateNumber))
						{
							return;
						}

						PanelUsedNumbers.Add(CandidateNumber);
						OutNumber = CandidateNumber;
						return;
					}

					if (GlobalUsedNumbers.Contains(CandidateNumber))
					{
						return;
					}

					GlobalUsedNumbers.Add(CandidateNumber);
					PanelUsedNumbers.Add(CandidateNumber);
					OutNumber = CandidateNumber;
				};

			if (bUseExplicitSlots)
			{
				NormalizedNumbers.Reserve(RequestedNumbers.Num());
				for (const int32 CandidateNumber : RequestedNumbers)
				{
					int32 AcceptedNumber = 0;
					TryAcceptNumber(CandidateNumber, AcceptedNumber);
					NormalizedNumbers.Add(AcceptedNumber);
				}
			}
			else
			{
				for (const int32 CandidateNumber : RequestedNumbers)
				{
					int32 AcceptedNumber = 0;
					TryAcceptNumber(CandidateNumber, AcceptedNumber);
					if (AcceptedNumber > 0)
					{
						NormalizedNumbers.Add(AcceptedNumber);
					}
				}
			}

			if (NormalizedNumbers.Num() < MinVisibleSlotCount)
			{
				NormalizedNumbers.SetNumZeroed(MinVisibleSlotCount);
			}

			return NormalizedNumbers;
		};

	const TArray<int32> NormalizedLeftNumbers = NormalizePanelNumbers(ActiveRequest.LeftNumbers);
	const TArray<int32> NormalizedRightNumbers = NormalizePanelNumbers(ActiveRequest.RightNumbers);

	auto BuildSlotItems =
		[this](const TArray<int32>& NormalizedNumbers, const ECodexTileTransferPanelSide PanelSide, TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& OutItems)
		{
			OutItems.Reserve(NormalizedNumbers.Num());
			for (int32 SlotIndex = 0; SlotIndex < NormalizedNumbers.Num(); ++SlotIndex)
			{
				UCodexInteractionDualTileTransferTileItem* SlotItem = CreateSlotItem(PanelSide, SlotIndex);
				if (SlotItem == nullptr)
				{
					continue;
				}

				if (NormalizedNumbers[SlotIndex] > 0)
				{
					ApplyTileContent(*SlotItem, NormalizedNumbers[SlotIndex]);
				}

				OutItems.Add(SlotItem);
			}
		};

	BuildSlotItems(NormalizedLeftNumbers, ECodexTileTransferPanelSide::Left, LeftItems);
	BuildSlotItems(NormalizedRightNumbers, ECodexTileTransferPanelSide::Right, RightItems);
}

int32 UCodexInteractionDualTileTransferPopupWidget::FindNextNumberForPanel(const ECodexTileTransferPanelSide PanelSide) const
{
	const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& TargetItems = GetPanelItems(PanelSide);

	for (int32 Number = MinTileNumber; Number <= MaxTileNumber; ++Number)
	{
		if (!ActiveRequest.bAllowDuplicateNumbers)
		{
			if (!ContainsNumber(LeftItems, Number) && !ContainsNumber(RightItems, Number))
			{
				return Number;
			}
		}
		else if (!ContainsNumber(TargetItems, Number))
		{
			return Number;
		}
	}

	return INDEX_NONE;
}

int32 UCodexInteractionDualTileTransferPopupWidget::FindFirstEmptySlot(const ECodexTileTransferPanelSide PanelSide) const
{
	const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& PanelItems = GetPanelItems(PanelSide);
	for (int32 SlotIndex = 0; SlotIndex < PanelItems.Num(); ++SlotIndex)
	{
		if (PanelItems[SlotIndex] != nullptr && !PanelItems[SlotIndex]->HasNumber())
		{
			return SlotIndex;
		}
	}

	return INDEX_NONE;
}

int32 UCodexInteractionDualTileTransferPopupWidget::AppendEmptySlot(const ECodexTileTransferPanelSide PanelSide)
{
	TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& PanelItems = GetPanelItems(PanelSide);
	const int32 SlotIndex = PanelItems.Num();
	PanelItems.Add(CreateSlotItem(PanelSide, SlotIndex));
	return SlotIndex;
}

int32 UCodexInteractionDualTileTransferPopupWidget::FindNextOccupiedSlot(
	const ECodexTileTransferPanelSide PanelSide,
	const int32 StartSlotIndex) const
{
	const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& PanelItems = GetPanelItems(PanelSide);
	for (int32 SlotIndex = StartSlotIndex + 1; SlotIndex < PanelItems.Num(); ++SlotIndex)
	{
		if (PanelItems[SlotIndex] != nullptr && PanelItems[SlotIndex]->HasNumber())
		{
			return SlotIndex;
		}
	}

	return INDEX_NONE;
}

int32 UCodexInteractionDualTileTransferPopupWidget::FindPreviousOccupiedSlot(
	const ECodexTileTransferPanelSide PanelSide,
	const int32 StartSlotIndex) const
{
	const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& PanelItems = GetPanelItems(PanelSide);
	for (int32 SlotIndex = StartSlotIndex - 1; SlotIndex >= 0; --SlotIndex)
	{
		if (PanelItems[SlotIndex] != nullptr && PanelItems[SlotIndex]->HasNumber())
		{
			return SlotIndex;
		}
	}

	return INDEX_NONE;
}

TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& UCodexInteractionDualTileTransferPopupWidget::GetPanelItems(
	const ECodexTileTransferPanelSide PanelSide)
{
	return PanelSide == ECodexTileTransferPanelSide::Left ? LeftItems : RightItems;
}

const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& UCodexInteractionDualTileTransferPopupWidget::GetPanelItems(
	const ECodexTileTransferPanelSide PanelSide) const
{
	return PanelSide == ECodexTileTransferPanelSide::Left ? LeftItems : RightItems;
}

UCodexInteractionDualTileTransferTileItem* UCodexInteractionDualTileTransferPopupWidget::GetSelectedItem(
	const ECodexTileTransferPanelSide PanelSide) const
{
	return PanelSide == ECodexTileTransferPanelSide::Left ? LeftSelectedItem : RightSelectedItem;
}

UCodexInteractionDualTileTransferTileEntryWidget* UCodexInteractionDualTileTransferPopupWidget::FindEntryWidgetForItem(
	const UCodexInteractionDualTileTransferTileItem* Item) const
{
	if (Item == nullptr)
	{
		return nullptr;
	}

	const UTileView* TileView = Item->PanelSide == ECodexTileTransferPanelSide::Left ? TileView_Left : TileView_Right;
	return TileView != nullptr ? TileView->GetEntryWidgetFromItem<UCodexInteractionDualTileTransferTileEntryWidget>(Item) : nullptr;
}

UCodexInteractionDualTileTransferTileItem* UCodexInteractionDualTileTransferPopupWidget::FindDropTargetItemAtScreenPosition(
	const FVector2D& ScreenPosition) const
{
	auto FindInTileView =
		[ScreenPosition](const UTileView* TileView) -> UCodexInteractionDualTileTransferTileItem*
		{
			if (TileView == nullptr)
			{
				return nullptr;
			}

			for (UUserWidget* EntryWidget : TileView->GetDisplayedEntryWidgets())
			{
				const UCodexInteractionDualTileTransferTileEntryWidget* TileEntryWidget =
					Cast<UCodexInteractionDualTileTransferTileEntryWidget>(EntryWidget);
				if (TileEntryWidget != nullptr && TileEntryWidget->GetCachedGeometry().IsUnderLocation(ScreenPosition))
				{
					return TileEntryWidget->GetTileItem();
				}
			}

			return nullptr;
		};

	if (UCodexInteractionDualTileTransferTileItem* LeftTargetItem = FindInTileView(TileView_Left))
	{
		return LeftTargetItem;
	}

	return FindInTileView(TileView_Right);
}

void UCodexInteractionDualTileTransferPopupWidget::SetHoveredDropTargetItem(UCodexInteractionDualTileTransferTileItem* Item)
{
	if (HoveredDropTargetItem == Item)
	{
		return;
	}

	HoveredDropTargetItem = Item;
	ClearDisplayedDropTargetHighlights();

	if (UCodexInteractionDualTileTransferTileEntryWidget* EntryWidget = FindEntryWidgetForItem(HoveredDropTargetItem))
	{
		EntryWidget->SetDropTargetHighlighted(true);
	}
}

void UCodexInteractionDualTileTransferPopupWidget::ClearHoveredDropTargetItem()
{
	HoveredDropTargetItem = nullptr;
	ClearDisplayedDropTargetHighlights();
}

void UCodexInteractionDualTileTransferPopupWidget::ClearDisplayedDropTargetHighlights()
{
	auto ClearInTileView =
		[](UTileView* TileView)
		{
			if (TileView == nullptr)
			{
				return;
			}

			for (UUserWidget* EntryWidget : TileView->GetDisplayedEntryWidgets())
			{
				if (UCodexInteractionDualTileTransferTileEntryWidget* TileEntryWidget =
					Cast<UCodexInteractionDualTileTransferTileEntryWidget>(EntryWidget))
				{
					TileEntryWidget->SetDropTargetHighlighted(false);
				}
			}
		};

	ClearInTileView(TileView_Left);
	ClearInTileView(TileView_Right);
}

TSubclassOf<UUserWidget> UCodexInteractionDualTileTransferPopupWidget::ResolveTileEntryWidgetClass()
{
	if (CachedTileEntryWidgetClass == nullptr)
	{
		CachedTileEntryWidgetClass = LoadClass<UUserWidget>(
			nullptr,
			*CodexInteractionAssetPaths::MakeGeneratedClassObjectPath(CodexInteractionAssetPaths::DualTileTransferTileEntryWidgetObjectPath));
	}

	return CachedTileEntryWidgetClass;
}

TSubclassOf<UUserWidget> UCodexInteractionDualTileTransferPopupWidget::ResolveTileEntryWidgetClassForItem(UObject* Item)
{
	(void)Item;
	return ResolveTileEntryWidgetClass();
}

TArray<int32> UCodexInteractionDualTileTransferPopupWidget::BuildResultNumbers(
	const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& Items) const
{
	TArray<int32> Numbers;
	Numbers.Reserve(Items.Num());
	for (const UCodexInteractionDualTileTransferTileItem* Item : Items)
	{
		Numbers.Add(Item != nullptr && Item->HasNumber() ? Item->Number : 0);
	}

	return Numbers;
}

TArray<UObject*> UCodexInteractionDualTileTransferPopupWidget::BuildListItems(
	const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& Items) const
{
	TArray<UObject*> Objects;
	Objects.Reserve(Items.Num());
	for (UCodexInteractionDualTileTransferTileItem* Item : Items)
	{
		if (Item != nullptr)
		{
			Objects.Add(Item);
		}
	}

	return Objects;
}

bool UCodexInteractionDualTileTransferPopupWidget::ContainsNumber(
	const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& Items,
	const int32 Number) const
{
	for (const UCodexInteractionDualTileTransferTileItem* Item : Items)
	{
		if (Item != nullptr && Item->HasNumber() && Item->Number == Number)
		{
			return true;
		}
	}

	return false;
}
