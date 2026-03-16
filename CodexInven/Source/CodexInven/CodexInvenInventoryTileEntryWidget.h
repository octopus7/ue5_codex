// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CodexInvenInventoryTileEntryWidget.generated.h"

class FReply;
class FDragDropEvent;
struct FGeometry;
struct FPointerEvent;
class UBorder;
class UImage;
class USizeBox;
class UTextBlock;
class UVerticalBox;
class UDragDropOperation;
class UCodexInvenPlayerHudWidget;
class UCodexInvenInventoryTileItemObject;

UCLASS()
class CODEXINVEN_API UCodexInvenInventoryTileEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	void SetTileItemObject(UCodexInvenInventoryTileItemObject* InItemObject);
	void SetOwningHudWidget(UCodexInvenPlayerHudWidget* InOwningHudWidget);
	void SetVisualState(bool bInIsDragSource, bool bInIsDropTarget);
	int32 GetSlotIndex() const;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	void BuildWidgetTreeIfNeeded();
	void RefreshFromItemObject(UCodexInvenInventoryTileItemObject* InItemObject);
	void ApplyVisualState() const;
	const UCodexInvenInventoryTileItemObject* GetTileItemObject() const;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> RootBorder = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> ContentBox = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UImage> IconImage = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> NameTextBlock = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuantityTextBlock = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInvenInventoryTileItemObject> TileItemObject = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInvenPlayerHudWidget> OwningHudWidget = nullptr;

	bool bIsDragSource = false;
	bool bIsDropTarget = false;
};
