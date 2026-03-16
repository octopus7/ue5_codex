// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CodexInvenOwnershipDebugWidget.generated.h"

class UTextBlock;
class UCodexInvenOwnershipComponent;

UCLASS()
class CODEXINVEN_API UCodexInvenOwnershipDebugWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetObservedOwnershipComponent(UCodexInvenOwnershipComponent* InComponent);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	void BuildWidgetTreeIfNeeded();
	void RefreshOwnershipText() const;
	void UnbindObservedOwnershipComponent();
	void HandleInventoryChanged();

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> OwnershipTextBlock = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInvenOwnershipComponent> ObservedOwnershipComponent = nullptr;
};
