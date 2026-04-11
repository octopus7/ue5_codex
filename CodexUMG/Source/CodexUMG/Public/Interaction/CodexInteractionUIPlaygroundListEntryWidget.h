// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CodexInteractionUIPlaygroundListEntryWidget.generated.h"

class UBorder;
class UTextBlock;
class UCodexInteractionUIPlaygroundListItem;

UCLASS(Abstract, Blueprintable)
class CODEXUMG_API UCodexInteractionUIPlaygroundListEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	void ApplyPreviewItem(UCodexInteractionUIPlaygroundListItem* InPreviewItem);

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnEntryReleased() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_Root;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> Border_Selection;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Label;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Description;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_State;

private:
	void RefreshVisualState() const;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionUIPlaygroundListItem> ActiveItem;
};
