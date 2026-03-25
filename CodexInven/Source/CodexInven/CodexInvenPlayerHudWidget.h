// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CodexInvenPlayerHudWidget.generated.h"

class UBorder;
class UButton;
class UCanvasPanel;
class UScrollBox;
class USizeBox;
class UTextBlock;
class UWrapBox;
class UCodexInvenInventoryTileEntryWidget;
class UCodexInvenInventoryTileItemObject;
class UCodexInvenOwnershipComponent;
class UCodexInvenOwnershipDebugWidget;

DECLARE_MULTICAST_DELEGATE(FOnCodexInvenPickupPageToggleRequested);

UCLASS()
class CODEXINVEN_API UCodexInvenPlayerHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetObservedOwnershipComponent(UCodexInvenOwnershipComponent* InComponent);
	void BeginInventoryDrag(int32 InSourceSlotIndex);
	void SetHoveredInventoryDropTarget(int32 InTargetSlotIndex);
	void ClearHoveredInventoryDropTarget(int32 InExpectedTargetSlotIndex = INDEX_NONE);
	void EndInventoryDrag();
	bool HandleInventorySlotDrop(int32 InSourceSlotIndex, int32 InTargetSlotIndex);
	bool ShouldBlockFireInput() const;
	bool IsInventoryPanelVisible() const;
	FOnCodexInvenPickupPageToggleRequested& OnPickupPageToggleRequested();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	void BuildWidgetTreeIfNeeded();
	void RefreshInventoryItems();
	void RefreshInventorySummaryText() const;
	void RefreshInventoryEntryVisualStates() const;
	void SetInventoryPanelVisible(bool bInVisible);
	void SetDebugWidgetVisible(bool bInVisible);
	void UnbindObservedOwnershipComponent();
	void HandleInventoryChanged();
	bool IsButtonHovered(const UButton* InButton) const;

	UFUNCTION()
	void HandleInventoryToggleClicked();

	UFUNCTION()
	void HandleDebugToggleClicked();

	UFUNCTION()
	void HandleIncreaseCapacityClicked();

	UFUNCTION()
	void HandlePickupPageToggleClicked();

	UFUNCTION()
	void HandleLightColorToggleClicked();

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> RootCanvasPanel = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UButton> InventoryToggleButton = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UButton> DebugToggleButton = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UButton> PickupPageToggleButton = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UButton> LightColorToggleButton = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> InventoryPanelBorder = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USizeBox> InventoryPanelSizeBox = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventorySummaryTextBlock = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UButton> IncreaseCapacityButton = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UScrollBox> InventoryScrollBox = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UWrapBox> InventoryWrapBox = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInvenOwnershipDebugWidget> OwnershipDebugWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInvenOwnershipComponent> ObservedOwnershipComponent = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCodexInvenInventoryTileItemObject>> InventoryTileItems;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCodexInvenInventoryTileEntryWidget>> InventoryEntryWidgets;

	int32 ActiveDragSourceSlotIndex = INDEX_NONE;
	int32 HoveredDropTargetSlotIndex = INDEX_NONE;
	bool bIsInventoryVisible = false;
	bool bIsDebugVisible = true;
	FOnCodexInvenPickupPageToggleRequested PickupPageToggleRequested;
};
