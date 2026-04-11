// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionMessagePopupWidget.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Interaction/CodexInteractionSubsystem.h"

void UCodexInteractionMessagePopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_Close)
	{
		BTN_Close->OnClicked.RemoveAll(this);
		BTN_Close->OnClicked.AddDynamic(this, &UCodexInteractionMessagePopupWidget::HandleCloseClicked);
	}

	if (BTN_Ok)
	{
		BTN_Ok->OnClicked.RemoveAll(this);
		BTN_Ok->OnClicked.AddDynamic(this, &UCodexInteractionMessagePopupWidget::HandleOkClicked);
	}

	if (BTN_Yes)
	{
		BTN_Yes->OnClicked.RemoveAll(this);
		BTN_Yes->OnClicked.AddDynamic(this, &UCodexInteractionMessagePopupWidget::HandleYesClicked);
	}

	if (BTN_No)
	{
		BTN_No->OnClicked.RemoveAll(this);
		BTN_No->OnClicked.AddDynamic(this, &UCodexInteractionMessagePopupWidget::HandleNoClicked);
	}

	RefreshButtonVisibility();
}

void UCodexInteractionMessagePopupWidget::ApplyPopupRequest(const FCodexInteractionPopupRequest& NewRequest, UCodexInteractionSubsystem& InInteractionSubsystem)
{
	ActiveRequest = NewRequest;
	InteractionSubsystem = &InInteractionSubsystem;

	if (TXT_Title)
	{
		TXT_Title->SetText(ActiveRequest.Title);
	}

	if (TXT_Message)
	{
		TXT_Message->SetText(ActiveRequest.Message);
	}

	RefreshButtonVisibility();
}

void UCodexInteractionMessagePopupWidget::HandleCloseClicked()
{
	SubmitPopupResult(ECodexPopupResult::Closed);
}

void UCodexInteractionMessagePopupWidget::HandleOkClicked()
{
	SubmitPopupResult(ECodexPopupResult::Ok);
}

void UCodexInteractionMessagePopupWidget::HandleYesClicked()
{
	SubmitPopupResult(ECodexPopupResult::Yes);
}

void UCodexInteractionMessagePopupWidget::HandleNoClicked()
{
	SubmitPopupResult(ECodexPopupResult::No);
}

void UCodexInteractionMessagePopupWidget::SubmitPopupResult(const ECodexPopupResult Result)
{
	if (InteractionSubsystem == nullptr)
	{
		return;
	}

	FCodexInteractionPopupResponse Response;
	Response.RequestId = ActiveRequest.RequestId;
	Response.InteractionRequest = ActiveRequest.InteractionRequest;
	Response.Result = Result;
	Response.bWasClosed = Result == ECodexPopupResult::Closed;
	InteractionSubsystem->SubmitInteractionPopupResult(Response);
}

void UCodexInteractionMessagePopupWidget::RefreshButtonVisibility() const
{
	const bool bIsYesNoLayout = ActiveRequest.ButtonLayout == ECodexPopupButtonLayout::YesNo;

	if (BTN_Ok)
	{
		BTN_Ok->SetVisibility(bIsYesNoLayout ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}

	if (BTN_Yes)
	{
		BTN_Yes->SetVisibility(bIsYesNoLayout ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (BTN_No)
	{
		BTN_No->SetVisibility(bIsYesNoLayout ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (HB_ActionButtons)
	{
		HB_ActionButtons->SetVisibility(ESlateVisibility::Visible);
	}
}
