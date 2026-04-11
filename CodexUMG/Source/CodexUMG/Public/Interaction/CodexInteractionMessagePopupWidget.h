// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Interaction/CodexInteractionTypes.h"
#include "CodexInteractionMessagePopupWidget.generated.h"

class UButton;
class UHorizontalBox;
class UTextBlock;
class UCodexInteractionSubsystem;

UCLASS(Abstract, Blueprintable)
class CODEXUMG_API UCodexInteractionMessagePopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void ApplyPopupRequest(const FCodexInteractionPopupRequest& NewRequest, UCodexInteractionSubsystem& InInteractionSubsystem);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Message;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Close;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Ok;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_Yes;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BTN_No;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UHorizontalBox> HB_ActionButtons;

private:
	UFUNCTION()
	void HandleCloseClicked();

	UFUNCTION()
	void HandleOkClicked();

	UFUNCTION()
	void HandleYesClicked();

	UFUNCTION()
	void HandleNoClicked();

	void SubmitPopupResult(ECodexPopupResult Result);
	void RefreshButtonVisibility() const;

	FCodexInteractionPopupRequest ActiveRequest;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionSubsystem> InteractionSubsystem;
};
