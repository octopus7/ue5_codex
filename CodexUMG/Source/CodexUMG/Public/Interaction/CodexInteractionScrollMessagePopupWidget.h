// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Interaction/CodexInteractionTypes.h"
#include "CodexInteractionScrollMessagePopupWidget.generated.h"

class UButton;
class UScrollBox;
class UTextBlock;
class UCodexInteractionSubsystem;

UCLASS(Abstract, Blueprintable)
class CODEXUMG_API UCodexInteractionScrollMessagePopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void ApplyPopupRequest(const FCodexInteractionPopupRequest& NewRequest, UCodexInteractionSubsystem& InInteractionSubsystem);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_Message;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Message;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Ok;

private:
	UFUNCTION()
	void HandleOkClicked();

	void SubmitPopupResult(ECodexPopupResult Result);

	FCodexInteractionPopupRequest ActiveRequest;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionSubsystem> InteractionSubsystem;
};
