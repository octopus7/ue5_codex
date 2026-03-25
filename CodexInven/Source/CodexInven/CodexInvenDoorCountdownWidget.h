// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CodexInvenDoorCountdownWidget.generated.h"

class UTextBlock;

UCLASS()
class CODEXINVEN_API UCodexInvenDoorCountdownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetRemainingSeconds(int32 InRemainingSeconds);
	void SetCountdownVisible(bool bInVisible);

protected:
	virtual void NativeOnInitialized() override;

private:
	void BuildWidgetTreeIfNeeded();

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> CountdownTextBlock = nullptr;
};
