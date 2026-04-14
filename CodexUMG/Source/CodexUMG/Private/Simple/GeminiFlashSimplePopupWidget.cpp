// Copyright Epic Games, Inc. All Rights Reserved.

#include "Simple/GeminiFlashSimplePopupWidget.h"
#include "Simple/GeminiFlashSimpleSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UGeminiFlashSimplePopupWidget::NativeConstruct()
{
	Super::NativeConstruct();

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
