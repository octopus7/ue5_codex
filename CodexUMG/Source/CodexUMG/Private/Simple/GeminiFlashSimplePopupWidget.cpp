// Copyright Epic Games, Inc. All Rights Reserved.

#include "Simple/GeminiFlashSimplePopupWidget.h"
#include "Simple/GeminiFlashSimpleSlotWidget.h"
#include "Interaction/CodexInteractionTypes.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Button.h"
#include "Interaction/CodexInteractionSubsystem.h"

void UGeminiFlashSimplePopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Close)
	{
		Button_Close->OnClicked.AddDynamic(this, &UGeminiFlashSimplePopupWidget::OnCloseClicked);
	}

	if (Grid_Slots)
	{
		Slots.Reset();
		// Collect all children that are GeminiFlashSimpleSlotWidgets
		for (UWidget* Child : Grid_Slots->GetAllChildren())
		{
			if (UGeminiFlashSimpleSlotWidget* SlotWidget = Cast<UGeminiFlashSimpleSlotWidget>(Child))
			{
				Slots.Add(SlotWidget);
			}
		}

		// Initialize with values 1, 2, 3 in the first three slots as requested
		for (int32 i = 0; i < Slots.Num(); ++i)
		{
			if (i < 3)
			{
				Slots[i]->SetValue(i + 1);
			}
			else
			{
				Slots[i]->SetValue(0); // Empty
			}
			Slots[i]->SetHighlight(false);
		}
	}
}

void UGeminiFlashSimplePopupWidget::HandleSwap(UGeminiFlashSimpleSlotWidget* FromSlot, UGeminiFlashSimpleSlotWidget* ToSlot)
{
	if (!FromSlot || !ToSlot || FromSlot == ToSlot)
	{
		return;
	}

	const int32 ValueFrom = FromSlot->GetValue();
	const int32 ValueTo = ToSlot->GetValue();

	// Swap values
	FromSlot->SetValue(ValueTo);
	ToSlot->SetValue(ValueFrom);
}

void UGeminiFlashSimplePopupWidget::ApplyPopupRequest(const FCodexInteractionPopupRequest& Request, UCodexInteractionSubsystem& Subsystem)
{
	InteractionSubsystem = &Subsystem;
	CurrentRequest = Request;
}

void UGeminiFlashSimplePopupWidget::OnCloseClicked()
{
	if (InteractionSubsystem)
	{
		FCodexInteractionPopupResponse Response;
		Response.RequestId = CurrentRequest.RequestId;
		Response.Result = ECodexPopupResult::Closed;
		Response.InteractionRequest = CurrentRequest.InteractionRequest;
		
		InteractionSubsystem->SubmitInteractionPopupResult(Response);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GeminiFlashSimplePopup: Cannot close, InteractionSubsystem is null!"));
	}
}
