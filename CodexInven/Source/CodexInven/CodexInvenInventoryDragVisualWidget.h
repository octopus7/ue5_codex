// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CodexInvenOwnershipComponent.h"
#include "CodexInvenInventoryDragVisualWidget.generated.h"

class UImage;
class USizeBox;
class UTextBlock;
class UVerticalBox;
class UTexture2D;

UCLASS()
class CODEXINVEN_API UCodexInvenInventoryDragVisualWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeFromSlotData(const FCodexInvenInventorySlotData& InSlotData, UTexture2D* InIconTexture);

protected:
	virtual void NativeOnInitialized() override;

private:
	void BuildWidgetTreeIfNeeded();

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> RootBox = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UImage> IconImage = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> NameTextBlock = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuantityTextBlock = nullptr;
};
