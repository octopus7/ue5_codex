// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/DragDropOperation.h"
#include "CodexInvenOwnershipComponent.h"
#include "CodexInvenInventoryDragDropOperation.generated.h"

class UTexture2D;
class UCodexInvenPlayerHudWidget;

UCLASS()
class CODEXINVEN_API UCodexInvenInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	void InitializeOperation(
		int32 InSourceSlotIndex,
		const FCodexInvenInventorySlotData& InSourceSlotData,
		UTexture2D* InIconTexture,
		UCodexInvenPlayerHudWidget* InOwningHudWidget);

	int32 GetSourceSlotIndex() const
	{
		return SourceSlotIndex;
	}

	const FCodexInvenInventorySlotData& GetSourceSlotData() const
	{
		return SourceSlotData;
	}

	UTexture2D* GetIconTexture() const
	{
		return IconTexture;
	}

protected:
	virtual void Drop_Implementation(const FPointerEvent& InPointerEvent) override;
	virtual void DragCancelled_Implementation(const FPointerEvent& InPointerEvent) override;

private:
	UPROPERTY(Transient)
	int32 SourceSlotIndex = INDEX_NONE;

	UPROPERTY(Transient)
	FCodexInvenInventorySlotData SourceSlotData;

	UPROPERTY(Transient)
	TObjectPtr<UTexture2D> IconTexture = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInvenPlayerHudWidget> OwningHudWidget = nullptr;
};
