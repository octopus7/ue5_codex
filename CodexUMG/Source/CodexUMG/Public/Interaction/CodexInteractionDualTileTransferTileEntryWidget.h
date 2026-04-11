// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CodexInteractionDualTileTransferTileEntryWidget.generated.h"

class UBorder;
class UDragDropOperation;
class UImage;
class UTextBlock;
class UCodexInteractionDualTileTransferTileItem;
class FDragDropEvent;

UCLASS(Abstract, Blueprintable)
class CODEXUMG_API UCodexInteractionDualTileTransferTileEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	void ApplyPreviewItem(UCodexInteractionDualTileTransferTileItem* InPreviewItem);
	UCodexInteractionDualTileTransferTileItem* GetTileItem() const;
	void SetDropTargetHighlighted(bool bHighlighted);

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnEntryReleased() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IMG_TileBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Number;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> Border_SelectedOutline;

private:
	class UCodexInteractionDualTileTransferPopupWidget* ResolvePopupWidget() const;
	TSubclassOf<UUserWidget> ResolveTileEntryWidgetClass() const;
	void RefreshVisualState() const;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionDualTileTransferTileItem> ActiveItem;

	bool bIsDropTargetHighlighted = false;
};
