// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Interaction/CodexInteractionTypes.h"
#include "CodexInteractionDualTileTransferPopupWidget.generated.h"

class UButton;
class UDragDropOperation;
class UTextBlock;
class UTileView;
class UCodexInteractionDualTileTransferTileItem;
class UCodexInteractionSubsystem;
class UCodexTileTransferDragDropOperation;
class UUserWidget;
class FDragDropEvent;

UCLASS(Abstract, Blueprintable)
class CODEXUMG_API UCodexInteractionDualTileTransferPopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	void ApplyPopupRequest(const FCodexInteractionPopupRequest& NewRequest, UCodexInteractionSubsystem& InInteractionSubsystem);
	void HandleControllerCloseRequested();
	bool CanDropOnSlot(const UCodexInteractionDualTileTransferTileItem* TargetItem, const UCodexTileTransferDragDropOperation* Operation) const;
	bool TryHandleDropOnSlot(UCodexInteractionDualTileTransferTileItem* TargetItem, UDragDropOperation* InOperation);
	void NotifyTileDragStarted();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_LeftAdd;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_LeftRemove;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_RightAdd;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_RightRemove;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> TileView_Left;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> TileView_Right;

private:
	UFUNCTION()
	void HandleCloseClicked();

	UFUNCTION()
	void HandleLeftAddClicked();

	UFUNCTION()
	void HandleLeftRemoveClicked();

	UFUNCTION()
	void HandleRightAddClicked();

	UFUNCTION()
	void HandleRightRemoveClicked();

	void HandleLeftSelectionChanged(UObject* SelectedItem);
	void HandleRightSelectionChanged(UObject* SelectedItem);

	void RefreshAllState();
	void RefreshTitle() const;
	void RefreshTileViews();
	void RefreshButtonState() const;
	void RefreshSelectionFlags();
	void SetSelectedItem(ECodexTileTransferPanelSide PanelSide, UCodexInteractionDualTileTransferTileItem* Item);
	void ClearSelection(ECodexTileTransferPanelSide PanelSide);
	bool MoveTileToSlot(UCodexTileTransferDragDropOperation& Operation, ECodexTileTransferPanelSide TargetSide, int32 TargetSlotIndex);
	void AddTileToPanel(ECodexTileTransferPanelSide TargetSide);
	void RemoveSelectedTile(ECodexTileTransferPanelSide TargetSide);
	void SubmitPopupResult(ECodexPopupResult Result);

	UCodexInteractionDualTileTransferTileItem* CreateSlotItem(ECodexTileTransferPanelSide PanelSide, int32 SlotIndex);
	void ApplyTileContent(UCodexInteractionDualTileTransferTileItem& Item, int32 Number);
	void ClearTileContent(UCodexInteractionDualTileTransferTileItem& Item);
	void RebuildTileItemsFromRequest();
	int32 FindNextNumberForPanel(ECodexTileTransferPanelSide PanelSide) const;
	int32 FindFirstEmptySlot(ECodexTileTransferPanelSide PanelSide) const;
	int32 AppendEmptySlot(ECodexTileTransferPanelSide PanelSide);
	int32 FindNextOccupiedSlot(ECodexTileTransferPanelSide PanelSide, int32 StartSlotIndex) const;
	int32 FindPreviousOccupiedSlot(ECodexTileTransferPanelSide PanelSide, int32 StartSlotIndex) const;
	TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& GetPanelItems(ECodexTileTransferPanelSide PanelSide);
	const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& GetPanelItems(ECodexTileTransferPanelSide PanelSide) const;
	UCodexInteractionDualTileTransferTileItem* GetSelectedItem(ECodexTileTransferPanelSide PanelSide) const;
	class UCodexInteractionDualTileTransferTileEntryWidget* FindEntryWidgetForItem(const UCodexInteractionDualTileTransferTileItem* Item) const;
	UCodexInteractionDualTileTransferTileItem* FindDropTargetItemAtScreenPosition(const FVector2D& ScreenPosition) const;
	void SetHoveredDropTargetItem(UCodexInteractionDualTileTransferTileItem* Item);
	void ClearHoveredDropTargetItem();
	void ClearDisplayedDropTargetHighlights();
	TSubclassOf<UUserWidget> ResolveTileEntryWidgetClass();
	TSubclassOf<UUserWidget> ResolveTileEntryWidgetClassForItem(UObject* Item);

	TArray<int32> BuildResultNumbers(const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& Items) const;
	TArray<UObject*> BuildListItems(const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& Items) const;
	bool ContainsNumber(const TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>>& Items, int32 Number) const;

	FCodexInteractionPopupRequest ActiveRequest;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionSubsystem> InteractionSubsystem;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>> LeftItems;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCodexInteractionDualTileTransferTileItem>> RightItems;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionDualTileTransferTileItem> LeftSelectedItem;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionDualTileTransferTileItem> RightSelectedItem;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionDualTileTransferTileItem> HoveredDropTargetItem;

	UPROPERTY(Transient)
	TSubclassOf<UUserWidget> CachedTileEntryWidgetClass;
};
