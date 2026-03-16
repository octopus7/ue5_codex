// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CodexInvenInventoryTileEntryWidget.generated.h"

class UBorder;
class UImage;
class USizeBox;
class UTextBlock;
class UCodexInvenInventoryTileItemObject;

UCLASS()
class CODEXINVEN_API UCodexInvenInventoryTileEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	void SetTileItemObject(UCodexInvenInventoryTileItemObject* InItemObject);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

private:
	void BuildWidgetTreeIfNeeded();
	void RefreshFromItemObject(UCodexInvenInventoryTileItemObject* InItemObject) const;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> RootBorder = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UImage> IconImage = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> NameTextBlock = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuantityTextBlock = nullptr;
};
