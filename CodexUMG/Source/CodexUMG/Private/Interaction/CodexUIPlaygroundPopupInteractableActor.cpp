// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexUIPlaygroundPopupInteractableActor.h"

#include "Interaction/CodexInteractionUIPlaygroundPayload.h"

ACodexUIPlaygroundPopupInteractableActor::ACodexUIPlaygroundPopupInteractableActor()
{
	PopupTitle = FText::FromString(TEXT("UI Playground"));
	PopupMessage = FText::FromString(TEXT("Test popup for reusable UMG controls."));
	PopupButtonLayout = ECodexPopupButtonLayout::Ok;

	UIPlaygroundPayload = CreateDefaultSubobject<UCodexInteractionUIPlaygroundPayload>(TEXT("UIPlaygroundPayload"));
	if (UIPlaygroundPayload != nullptr)
	{
		UIPlaygroundPayload->Title = FText::FromString(TEXT("UI Playground"));
		UIPlaygroundPayload->StatusText = FText::FromString(TEXT("Ready for interaction tests."));
		UIPlaygroundPayload->InitialSection = ECodexUIPlaygroundSection::Basic;
		UIPlaygroundPayload->BasicDescription = FText::FromString(TEXT("Exercise buttons, text, lists, drag/drop, and section switching in one popup."));
		UIPlaygroundPayload->InputText = FText::FromString(TEXT("Type here"));
		UIPlaygroundPayload->bToggleValue = true;
		UIPlaygroundPayload->SliderValue = 0.35f;
		UIPlaygroundPayload->SpinValue = 2;
		UIPlaygroundPayload->SelectedPreset = TEXT("Preset A");
		UIPlaygroundPayload->PresetOptions = { TEXT("Preset A"), TEXT("Preset B"), TEXT("Preset C"), TEXT("Preset D") };
		UIPlaygroundPayload->ListEntries =
		{
			{ FText::FromString(TEXT("Buttons")), FText::FromString(TEXT("Buttons and action states.")), FText::FromString(TEXT("Collection")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.26f, 0.38f, 0.52f, 1.0f), true },
			{ FText::FromString(TEXT("Text")), FText::FromString(TEXT("Editable and read-only text controls.")), FText::FromString(TEXT("Collection")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.24f, 0.48f, 0.35f, 1.0f), true },
			{ FText::FromString(TEXT("Images")), FText::FromString(TEXT("Brush, tint, and icon slots.")), FText::FromString(TEXT("Collection")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.47f, 0.34f, 0.58f, 1.0f), true },
			{ FText::FromString(TEXT("Drag and Drop")), FText::FromString(TEXT("Tile slots with drop targets.")), FText::FromString(TEXT("Advanced")), ECodexUIPlaygroundSection::Advanced, FLinearColor(0.55f, 0.41f, 0.25f, 1.0f), true }
		};
		UIPlaygroundPayload->TileSlots =
		{
			{ 0, FText::FromString(TEXT("Slot 1")), 11, FLinearColor(0.68f, 0.84f, 0.98f, 1.0f), false, ECodexUIPlaygroundSection::Advanced },
			{ 1, FText::FromString(TEXT("Slot 2")), 12, FLinearColor(0.67f, 0.93f, 0.85f, 1.0f), false, ECodexUIPlaygroundSection::Advanced },
			{ 2, FText::FromString(TEXT("Slot 3")), 0, FLinearColor::White, true, ECodexUIPlaygroundSection::Advanced },
			{ 3, FText::FromString(TEXT("Slot 4")), 24, FLinearColor(0.98f, 0.76f, 0.84f, 1.0f), false, ECodexUIPlaygroundSection::Advanced }
		};
	}
}

ECodexInteractionPopupStyle ACodexUIPlaygroundPopupInteractableActor::GetPopupStyle() const
{
	return ECodexInteractionPopupStyle::UIPlayground;
}

void ACodexUIPlaygroundPopupInteractableActor::PopulatePopupRequest(FCodexInteractionPopupRequest& PopupRequest) const
{
	Super::PopulatePopupRequest(PopupRequest);
	PopupRequest.PopupStyle = ECodexInteractionPopupStyle::UIPlayground;
	PopupRequest.UIPlaygroundPayload = UIPlaygroundPayload;
}
