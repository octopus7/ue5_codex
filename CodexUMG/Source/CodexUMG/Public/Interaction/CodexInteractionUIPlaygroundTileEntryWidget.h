// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CodexInteractionUIPlaygroundTileEntryWidget.generated.h"

class UBorder;
class UDragDropOperation;
class UImage;
class UTextBlock;
class UCodexInteractionUIPlaygroundTileItem;
class FDragDropEvent;

UCLASS(Abstract, Blueprintable)
class CODEXUMG_API UCodexInteractionUIPlaygroundTileEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	void ApplyPreviewItem(UCodexInteractionUIPlaygroundTileItem* InItem);
	void SetDropTargetHighlighted(bool bHighlighted);
	UCodexInteractionUIPlaygroundTileItem* GetTileItem() const { return ActiveItem; }

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnEntryReleased() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_TileRoot;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> Border_Outline;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IMG_TileBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Label;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Value;

private:
	class UCodexInteractionUIPlaygroundPopupWidget* ResolvePopupWidget() const;
	TSubclassOf<UUserWidget> ResolveTileEntryWidgetClass() const;
	void RefreshVisualState() const;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionUIPlaygroundTileItem> ActiveItem;

	bool bIsDropTargetHighlighted = false;
};
