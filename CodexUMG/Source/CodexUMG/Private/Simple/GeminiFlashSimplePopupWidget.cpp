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

		// Initialize with values 1, 2, 3 twice (total 6 unique items)
		for (int32 i = 0; i < Slots.Num(); ++i)
		{
			if (i < 3)
			{
				// First set: 1, 2, 3
				Slots[i]->SetItemInstance(FGeminiFlashItemInstance(i + 1));
			}
			else if (i < 6)
			{
				// Second set: 1, 2, 3 (but will have unique GUIDs)
				Slots[i]->SetItemInstance(FGeminiFlashItemInstance(i - 3 + 1));
			}
			else
			{
				// Empty slots
				Slots[i]->SetItemInstance(FGeminiFlashItemInstance());
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

	const FGeminiFlashItemInstance InstanceFrom = FromSlot->GetItemInstance();
	const FGeminiFlashItemInstance InstanceTo = ToSlot->GetItemInstance();

	// Swap complete structs
	FromSlot->SetItemInstance(InstanceTo);
	ToSlot->SetItemInstance(InstanceFrom);
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
