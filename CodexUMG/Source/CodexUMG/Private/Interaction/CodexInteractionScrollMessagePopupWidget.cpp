// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionScrollMessagePopupWidget.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Interaction/CodexInteractionSubsystem.h"

void UCodexInteractionScrollMessagePopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_Ok)
	{
		BTN_Ok->OnClicked.RemoveAll(this);
		BTN_Ok->OnClicked.AddDynamic(this, &UCodexInteractionScrollMessagePopupWidget::HandleOkClicked);
	}
}

void UCodexInteractionScrollMessagePopupWidget::ApplyPopupRequest(
	const FCodexInteractionPopupRequest& NewRequest,
	UCodexInteractionSubsystem& InInteractionSubsystem)
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

	if (ScrollBox_Message)
	{
		ScrollBox_Message->ScrollToStart();
	}
}

void UCodexInteractionScrollMessagePopupWidget::HandleOkClicked()
{
	SubmitPopupResult(ECodexPopupResult::Ok);
}

void UCodexInteractionScrollMessagePopupWidget::SubmitPopupResult(const ECodexPopupResult Result)
{
	if (InteractionSubsystem == nullptr)
	{
		return;
	}

	FCodexInteractionPopupResponse Response;
	Response.RequestId = ActiveRequest.RequestId;
	Response.InteractionRequest = ActiveRequest.InteractionRequest;
	Response.Result = Result;
	InteractionSubsystem->SubmitInteractionPopupResult(Response);
}
