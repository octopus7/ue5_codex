// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionUIPlaygroundPopupWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/ListView.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Components/WidgetSwitcher.h"
#include "InputCoreTypes.h"
#include "Interaction/CodexInteractionAssetPaths.h"
#include "Interaction/CodexInteractionSubsystem.h"
#include "Interaction/CodexInteractionUIPlaygroundListEntryWidget.h"
#include "Interaction/CodexInteractionUIPlaygroundListItem.h"
#include "Interaction/CodexInteractionUIPlaygroundPayload.h"
#include "Interaction/CodexInteractionUIPlaygroundTileEntryWidget.h"
#include "Interaction/CodexInteractionUIPlaygroundTileItem.h"
#include "Interaction/CodexUIPlaygroundDragDropOperation.h"

namespace
{
	FText MakeSectionStatusText(const ECodexUIPlaygroundSection Section)
	{
		switch (Section)
		{
		case ECodexUIPlaygroundSection::Basic:
			return FText::FromString(TEXT("Click Primary, Secondary, or Ping to compare button behavior."));
		case ECodexUIPlaygroundSection::Input:
			return FText::FromString(TEXT("Type text, toggle the checkbox, and adjust the slider, spin box, or preset."));
		case ECodexUIPlaygroundSection::Collection:
			return FText::FromString(TEXT("Click a row to select it, then add, remove, or clear selection."));
		case ECodexUIPlaygroundSection::Advanced:
		default:
			return FText::FromString(TEXT("Click or drag tiles to verify selection, focus, and drop handling."));
		}
	}

	TArray<FCodexUIPlaygroundListEntryData> BuildDefaultListEntries()
	{
		return
		{
			{ FText::FromString(TEXT("Buttons")), FText::FromString(TEXT("Primary, secondary, disabled, and ping button states.")), FText::FromString(TEXT("Clickable")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.26f, 0.38f, 0.52f, 1.0f), true },
			{ FText::FromString(TEXT("Text Input")), FText::FromString(TEXT("Single-line and multi-line text editing.")), FText::FromString(TEXT("Editable")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.24f, 0.48f, 0.35f, 1.0f), true },
			{ FText::FromString(TEXT("Checkbox")), FText::FromString(TEXT("Binary on/off state handling.")), FText::FromString(TEXT("Toggle")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.32f, 0.55f, 0.40f, 1.0f), true },
			{ FText::FromString(TEXT("Slider")), FText::FromString(TEXT("Continuous value changes with immediate feedback.")), FText::FromString(TEXT("Value")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.44f, 0.52f, 0.78f, 1.0f), true },
			{ FText::FromString(TEXT("Reusable Entries")), FText::FromString(TEXT("ListView items rendered by a reusable entry widget.")), FText::FromString(TEXT("Virtualized")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.47f, 0.34f, 0.58f, 1.0f), true },
			{ FText::FromString(TEXT("Selection State")), FText::FromString(TEXT("Selection highlight can be set, cleared, and restored.")), FText::FromString(TEXT("Selectable")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.55f, 0.44f, 0.28f, 1.0f), true },
			{ FText::FromString(TEXT("Scrolling")), FText::FromString(TEXT("Add rows until the list viewport starts scrolling.")), FText::FromString(TEXT("Overflow")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.32f, 0.62f, 0.66f, 1.0f), true },
			{ FText::FromString(TEXT("Drag and Drop Preview")), FText::FromString(TEXT("The Advanced tab continues into tile drag and drop.")), FText::FromString(TEXT("Next Step")), ECodexUIPlaygroundSection::Collection, FLinearColor(0.60f, 0.46f, 0.30f, 1.0f), true }
		};
	}

	FLinearColor ResolveGeneratedListColor(const int32 Index)
	{
		static const TArray<FLinearColor> Palette =
		{
			FLinearColor(0.36f, 0.58f, 0.76f, 1.0f),
			FLinearColor(0.32f, 0.62f, 0.52f, 1.0f),
			FLinearColor(0.56f, 0.46f, 0.74f, 1.0f),
			FLinearColor(0.70f, 0.56f, 0.34f, 1.0f)
		};

		return Palette[Index % Palette.Num()];
	}

	FCodexUIPlaygroundListEntryData MakeGeneratedListEntryData(const int32 Index)
	{
		FCodexUIPlaygroundListEntryData Data;
		Data.Title = FText::FromString(FString::Printf(TEXT("Entry %d"), Index + 1));
		Data.Description = FText::FromString(TEXT("Added from the Collection controls to test runtime list refresh."));
		Data.StateText = FText::FromString(TEXT("Added"));
		Data.Section = ECodexUIPlaygroundSection::Collection;
		Data.TintColor = ResolveGeneratedListColor(Index);
		Data.bIsEnabled = true;
		return Data;
	}

	FText BuildCollectionSummaryText(
		const TArray<TObjectPtr<UCodexInteractionUIPlaygroundListItem>>& Items,
		const UCodexInteractionUIPlaygroundListItem* SelectedItem)
	{
		const FText CountText = FText::AsNumber(Items.Num());
		if (SelectedItem != nullptr)
		{
			return FText::Format(
				FText::FromString(TEXT("Click a row to select it. Add creates rows, Remove deletes the selected row, and Select / Clear toggles the highlight. Items: {0}. Selected: {1}.")),
				CountText,
				SelectedItem->Title);
		}

		return FText::Format(
			FText::FromString(TEXT("Click a row to select it. Add creates rows, Remove deletes the selected row or the last row if none is selected, and Select / Clear toggles the highlight. Items: {0}.")),
			CountText);
	}

	UCodexInteractionUIPlaygroundPayload* BuildDefaultPayload(UObject* Outer)
	{
		UCodexInteractionUIPlaygroundPayload* Payload = NewObject<UCodexInteractionUIPlaygroundPayload>(Outer);
		Payload->Title = FText::FromString(TEXT("UI Playground"));
		Payload->StatusText = MakeSectionStatusText(ECodexUIPlaygroundSection::Basic);
		Payload->InitialSection = ECodexUIPlaygroundSection::Basic;
		Payload->BasicDescription = FText::FromString(TEXT("Start here: click Primary, Secondary, or Ping. The disabled button should stay inert while the status bar updates.")); 
		Payload->InputText = FText::FromString(TEXT("Type here"));
		Payload->bToggleValue = true;
		Payload->SliderValue = 0.35f;
		Payload->SpinValue = 2;
		Payload->SelectedPreset = TEXT("Preset A");
		Payload->PresetOptions = { TEXT("Preset A"), TEXT("Preset B"), TEXT("Preset C"), TEXT("Preset D") };
		Payload->ListEntries = BuildDefaultListEntries();
		Payload->TileSlots =
		{
			{ 0, FText::FromString(TEXT("Slot 1")), 11, FLinearColor(0.68f, 0.84f, 0.98f, 1.0f), false, ECodexUIPlaygroundSection::Advanced },
			{ 1, FText::FromString(TEXT("Slot 2")), 12, FLinearColor(0.67f, 0.93f, 0.85f, 1.0f), false, ECodexUIPlaygroundSection::Advanced },
			{ 2, FText::FromString(TEXT("Slot 3")), 0, FLinearColor::White, true, ECodexUIPlaygroundSection::Advanced },
			{ 3, FText::FromString(TEXT("Slot 4")), 24, FLinearColor(0.98f, 0.76f, 0.84f, 1.0f), false, ECodexUIPlaygroundSection::Advanced }
		};
		return Payload;
	}

	TArray<UObject*> ToObjectArray(const TArray<TObjectPtr<UCodexInteractionUIPlaygroundListItem>>& Items)
	{
		TArray<UObject*> Result;
		Result.Reserve(Items.Num());
		for (UCodexInteractionUIPlaygroundListItem* Item : Items)
		{
			Result.Add(Item);
		}
		return Result;
	}

	TArray<UObject*> ToObjectArray(const TArray<TObjectPtr<UCodexInteractionUIPlaygroundTileItem>>& Items)
	{
		TArray<UObject*> Result;
		Result.Reserve(Items.Num());
		for (UCodexInteractionUIPlaygroundTileItem* Item : Items)
		{
			Result.Add(Item);
		}
		return Result;
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);

	if (BTN_Close != nullptr)
	{
		BTN_Close->OnClicked.RemoveAll(this);
		BTN_Close->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleCloseClicked);
	}

	if (BTN_Reset != nullptr)
	{
		BTN_Reset->OnClicked.RemoveAll(this);
		BTN_Reset->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleResetClicked);
	}

	if (BTN_Submit != nullptr)
	{
		BTN_Submit->OnClicked.RemoveAll(this);
		BTN_Submit->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleSubmitClicked);
	}

	if (BTN_Primary != nullptr)
	{
		BTN_Primary->OnClicked.RemoveAll(this);
		BTN_Primary->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandlePrimaryClicked);
	}

	if (BTN_Secondary != nullptr)
	{
		BTN_Secondary->OnClicked.RemoveAll(this);
		BTN_Secondary->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleSecondaryClicked);
	}

	if (BTN_StatusPing != nullptr)
	{
		BTN_StatusPing->OnClicked.RemoveAll(this);
		BTN_StatusPing->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleStatusPingClicked);
	}

	if (BTN_TabBasic != nullptr)
	{
		BTN_TabBasic->OnClicked.RemoveAll(this);
		BTN_TabBasic->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleTabBasicClicked);
	}

	if (BTN_TabInput != nullptr)
	{
		BTN_TabInput->OnClicked.RemoveAll(this);
		BTN_TabInput->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleTabInputClicked);
	}

	if (BTN_TabCollection != nullptr)
	{
		BTN_TabCollection->OnClicked.RemoveAll(this);
		BTN_TabCollection->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleTabCollectionClicked);
	}

	if (BTN_TabAdvanced != nullptr)
	{
		BTN_TabAdvanced->OnClicked.RemoveAll(this);
		BTN_TabAdvanced->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleTabAdvancedClicked);
	}

	if (ETB_InputText != nullptr)
	{
		ETB_InputText->OnTextCommitted.RemoveAll(this);
		ETB_InputText->OnTextCommitted.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleInputTextCommitted);
	}

	if (MultiLineEditableTextBox_Notes != nullptr)
	{
		MultiLineEditableTextBox_Notes->OnTextCommitted.RemoveAll(this);
		MultiLineEditableTextBox_Notes->OnTextCommitted.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleNotesTextCommitted);
	}

	if (CHK_Toggle != nullptr)
	{
		CHK_Toggle->OnCheckStateChanged.RemoveAll(this);
		CHK_Toggle->OnCheckStateChanged.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleToggleChanged);
	}

	if (SLD_Value != nullptr)
	{
		SLD_Value->OnValueChanged.RemoveAll(this);
		SLD_Value->OnValueChanged.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleSliderChanged);
	}

	if (SPN_Value != nullptr)
	{
		SPN_Value->OnValueChanged.RemoveAll(this);
		SPN_Value->OnValueChanged.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleSpinBoxChanged);
	}

	if (CB_Preset != nullptr)
	{
		CB_Preset->OnSelectionChanged.RemoveAll(this);
		CB_Preset->OnSelectionChanged.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandlePresetChanged);
	}

	if (ListView_Items != nullptr)
	{
		ListView_Items->SetSelectionMode(ESelectionMode::Single);
		ListView_Items->OnItemSelectionChanged().RemoveAll(this);
		ListView_Items->OnItemSelectionChanged().AddUObject(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleListSelectionChanged);
	}

	if (BTN_ListAdd != nullptr)
	{
		BTN_ListAdd->OnClicked.RemoveAll(this);
		BTN_ListAdd->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleListAddClicked);
	}

	if (BTN_ListRemove != nullptr)
	{
		BTN_ListRemove->OnClicked.RemoveAll(this);
		BTN_ListRemove->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleListRemoveClicked);
	}

	if (BTN_ListToggleSelection != nullptr)
	{
		BTN_ListToggleSelection->OnClicked.RemoveAll(this);
		BTN_ListToggleSelection->OnClicked.AddDynamic(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleListToggleSelectionClicked);
	}

	if (TileView_Slots != nullptr)
	{
		TileView_Slots->SetSelectionMode(ESelectionMode::Single);
		TileView_Slots->OnItemSelectionChanged().RemoveAll(this);
		TileView_Slots->OnItemSelectionChanged().AddUObject(this, &UCodexInteractionUIPlaygroundPopupWidget::HandleTileSelectionChanged);
	}

	if (ActivePayload != nullptr)
	{
		RefreshAll();
	}
}

FReply UCodexInteractionUIPlaygroundPopupWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	(void)InGeometry;

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		SubmitPopupResult(ECodexPopupResult::Closed);
		return FReply::Handled();
	}

	if (InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		SubmitPopupResult(ECodexPopupResult::Ok);
		return FReply::Handled();
	}

	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		const int32 CurrentIndex = ResolveSectionIndex(ActiveSection);
		const int32 Step = InKeyEvent.IsShiftDown() ? -1 : 1;
		const int32 NextIndex = (CurrentIndex + Step + 4) % 4;
		SetActiveSection(static_cast<ECodexUIPlaygroundSection>(NextIndex));
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UCodexInteractionUIPlaygroundPopupWidget::ApplyPopupRequest(
	const FCodexInteractionPopupRequest& NewRequest,
	UCodexInteractionSubsystem& InInteractionSubsystem)
{
	ActiveRequest = NewRequest;
	InteractionSubsystem = &InInteractionSubsystem;

	UCodexInteractionUIPlaygroundPayload* SourcePayload = ActiveRequest.UIPlaygroundPayload.Get();
	if (SourcePayload == nullptr)
	{
		SourcePayload = BuildDefaultPayload(this);
	}

	InitialPayloadSnapshot = DuplicateObject<UCodexInteractionUIPlaygroundPayload>(SourcePayload, this);
	ActivePayload = DuplicateObject<UCodexInteractionUIPlaygroundPayload>(SourcePayload, this);
	EnsurePayloadContent();
	ActiveSection = ActivePayload != nullptr ? ActivePayload->InitialSection : ECodexUIPlaygroundSection::Basic;
	BuildItemsFromPayload();
	RefreshAll();
}

void UCodexInteractionUIPlaygroundPopupWidget::NotifyTileDragStarted()
{
	SetStatusMessage(FText::FromString(TEXT("Dragging a tile. Drop it on another slot in the same panel.")));
}

bool UCodexInteractionUIPlaygroundPopupWidget::CanDropOnSlot(
	const UCodexInteractionUIPlaygroundTileItem* TargetItem,
	const UCodexUIPlaygroundDragDropOperation* Operation) const
{
	return TargetItem != nullptr
		&& Operation != nullptr
		&& Operation->Item != nullptr
		&& TargetItem != Operation->Item
		&& TargetItem->Section == Operation->Section;
}

bool UCodexInteractionUIPlaygroundPopupWidget::TryHandleDropOnSlot(
	UCodexInteractionUIPlaygroundTileItem* TargetItem,
	UDragDropOperation* InOperation)
{
	UCodexUIPlaygroundDragDropOperation* Operation = Cast<UCodexUIPlaygroundDragDropOperation>(InOperation);
	if (!CanDropOnSlot(TargetItem, Operation))
	{
		return false;
	}

	UCodexInteractionUIPlaygroundTileItem* SourceItem = Operation->Item;
	if (SourceItem == nullptr || TargetItem == nullptr)
	{
		return false;
	}

	const FText SourceLabel = SourceItem->Label;
	const FText TargetLabel = TargetItem->Label;

	Swap(SourceItem->SlotIndex, TargetItem->SlotIndex);
	Swap(SourceItem->Label, TargetItem->Label);
	Swap(SourceItem->Value, TargetItem->Value);
	Swap(SourceItem->TintColor, TargetItem->TintColor);
	Swap(SourceItem->bIsEmpty, TargetItem->bIsEmpty);

	UpdateSelectionFlags();
	CopyTileItemsToPayload();
	if (TileView_Slots != nullptr)
	{
		TileView_Slots->RequestRefresh();
	}

	SetStatusMessage(FText::Format(
		FText::FromString(TEXT("Moved {0} onto {1}.")),
		SourceLabel,
		TargetLabel));
	return true;
}

void UCodexInteractionUIPlaygroundPopupWidget::SetActiveSection(ECodexUIPlaygroundSection InSection)
{
	ActiveSection = InSection;
	if (ActivePayload != nullptr)
	{
		ActivePayload->InitialSection = InSection;
	}

	RefreshSectionVisibility();
	SetStatusMessage(MakeSectionStatusText(InSection));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleCloseClicked()
{
	SubmitPopupResult(ECodexPopupResult::Closed);
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleResetClicked()
{
	ResetPopup();
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleSubmitClicked()
{
	SubmitPopupResult(ECodexPopupResult::Ok);
}

void UCodexInteractionUIPlaygroundPopupWidget::HandlePrimaryClicked()
{
	SetStatusMessage(FText::FromString(TEXT("Primary button clicked. Compare its enabled style against the disabled sample.")));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleSecondaryClicked()
{
	SetStatusMessage(FText::FromString(TEXT("Secondary button clicked. Use it to compare alternate emphasis.")));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleStatusPingClicked()
{
	if (ProgressBar_Sample != nullptr)
	{
		float NextPercent = ProgressBar_Sample->GetPercent() + 0.14f;
		if (NextPercent > 1.0f)
		{
			NextPercent = 0.18f;
		}

		ProgressBar_Sample->SetPercent(NextPercent);
		SetStatusMessage(FText::Format(
			FText::FromString(TEXT("Ping updated the sample progress bar to {0}%.")),
			FText::AsNumber(FMath::RoundToInt(NextPercent * 100.0f))));
		return;
	}

	SetStatusMessage(FText::FromString(TEXT("Ping button clicked.")));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleTabBasicClicked()
{
	SetActiveSection(ECodexUIPlaygroundSection::Basic);
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleTabInputClicked()
{
	SetActiveSection(ECodexUIPlaygroundSection::Input);
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleTabCollectionClicked()
{
	SetActiveSection(ECodexUIPlaygroundSection::Collection);
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleTabAdvancedClicked()
{
	SetActiveSection(ECodexUIPlaygroundSection::Advanced);
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	(void)CommitMethod;
	if (bIsRefreshingControls || ActivePayload == nullptr)
	{
		return;
	}

	ActivePayload->InputText = Text;
	SetStatusMessage(FText::Format(
		FText::FromString(TEXT("Name field committed: {0}.")),
		Text.IsEmpty() ? FText::FromString(TEXT("empty")) : Text));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleNotesTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	(void)CommitMethod;
	if (bIsRefreshingControls)
	{
		return;
	}

	SetStatusMessage(FText::Format(
		FText::FromString(TEXT("Notes committed. {0} characters entered.")),
		FText::AsNumber(Text.ToString().Len())));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleToggleChanged(const bool bIsChecked)
{
	if (bIsRefreshingControls || ActivePayload == nullptr)
	{
		return;
	}

	ActivePayload->bToggleValue = bIsChecked;
	SetStatusMessage(FText::FromString(bIsChecked ? TEXT("Checkbox enabled.") : TEXT("Checkbox disabled.")));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleSliderChanged(const float Value)
{
	if (bIsRefreshingControls || ActivePayload == nullptr)
	{
		return;
	}

	ActivePayload->SliderValue = Value;
	SetStatusMessage(FText::Format(
		FText::FromString(TEXT("Slider changed to {0}%.")),
		FText::AsNumber(FMath::RoundToInt(Value * 100.0f))));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleSpinBoxChanged(const float Value)
{
	if (bIsRefreshingControls || ActivePayload == nullptr)
	{
		return;
	}

	ActivePayload->SpinValue = FMath::RoundToInt(Value);
	SetStatusMessage(FText::Format(
		FText::FromString(TEXT("Spin box changed to {0}.")),
		FText::AsNumber(ActivePayload->SpinValue)));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandlePresetChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	(void)SelectionType;
	if (bIsRefreshingControls || ActivePayload == nullptr)
	{
		return;
	}

	ActivePayload->SelectedPreset = MoveTemp(SelectedItem);
	SetStatusMessage(FText::Format(
		FText::FromString(TEXT("Preset changed to {0}.")),
		FText::FromString(ActivePayload->SelectedPreset)));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleListSelectionChanged(UObject* SelectedItem)
{
	if (bIsRefreshingItems)
	{
		return;
	}

	SelectedListItem = Cast<UCodexInteractionUIPlaygroundListItem>(SelectedItem);
	UpdateListSelectionFlags();
	RefreshCollectionSection();

	if (SelectedListItem != nullptr)
	{
		SetStatusMessage(FText::Format(
			FText::FromString(TEXT("Selected list entry: {0}.")),
			SelectedListItem->Title));
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleListAddClicked()
{
	const FCodexUIPlaygroundListEntryData Data = MakeGeneratedListEntryData(ListItems.Num());
	UCodexInteractionUIPlaygroundListItem* NewItem = CreateListItemFromData(Data);
	ListItems.Add(NewItem);
	SelectedListItem = NewItem;
	UpdateListSelectionFlags();
	CopyListItemsToPayload();
	RefreshCollectionSection();

	if (ListView_Items != nullptr)
	{
		ListView_Items->ScrollIndexIntoView(ListItems.Num() - 1);
	}

	SetStatusMessage(FText::Format(
		FText::FromString(TEXT("Added {0}. Keep adding rows to test scrolling.")),
		NewItem->Title));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleListRemoveClicked()
{
	if (ListItems.Num() == 0)
	{
		SetStatusMessage(FText::FromString(TEXT("There are no collection rows to remove.")));
		return;
	}

	int32 RemoveIndex = SelectedListItem != nullptr ? ListItems.IndexOfByKey(SelectedListItem) : INDEX_NONE;
	if (!ListItems.IsValidIndex(RemoveIndex))
	{
		RemoveIndex = ListItems.Num() - 1;
	}

	const FText RemovedTitle = ListItems[RemoveIndex] != nullptr ? ListItems[RemoveIndex]->Title : FText::FromString(TEXT("entry"));
	ListItems.RemoveAt(RemoveIndex);
	SelectedListItem = ListItems.IsValidIndex(RemoveIndex) ? ListItems[RemoveIndex] : (ListItems.Num() > 0 ? ListItems.Last() : nullptr);
	UpdateListSelectionFlags();
	CopyListItemsToPayload();
	RefreshCollectionSection();

	SetStatusMessage(FText::Format(
		FText::FromString(TEXT("Removed {0}.")),
		RemovedTitle));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleListToggleSelectionClicked()
{
	if (ListItems.Num() == 0)
	{
		SetStatusMessage(FText::FromString(TEXT("Add a collection row first, then toggle selection.")));
		return;
	}

	if (SelectedListItem == nullptr)
	{
		SelectedListItem = ListItems[0];
		UpdateListSelectionFlags();
		RefreshCollectionSection();

		if (ListView_Items != nullptr)
		{
			ListView_Items->SetSelectedItem(SelectedListItem);
			ListView_Items->ScrollIndexIntoView(0);
		}

		SetStatusMessage(FText::Format(
			FText::FromString(TEXT("Selected {0}. Click the button again to clear selection.")),
			SelectedListItem->Title));
		return;
	}

	SelectedListItem = nullptr;
	UpdateListSelectionFlags();
	RefreshCollectionSection();
	if (ListView_Items != nullptr)
	{
		ListView_Items->ClearSelection();
	}

	SetStatusMessage(FText::FromString(TEXT("Collection selection cleared.")));
}

void UCodexInteractionUIPlaygroundPopupWidget::HandleTileSelectionChanged(UObject* SelectedItem)
{
	SelectedTileItem = Cast<UCodexInteractionUIPlaygroundTileItem>(SelectedItem);
	UpdateSelectionFlags();
	if (SelectedTileItem != nullptr)
	{
		SetStatusMessage(FText::Format(
			FText::FromString(TEXT("Selected tile slot: {0}.")),
			SelectedTileItem->Label));
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::RefreshAll()
{
	RefreshTitleAndStatus();
	RefreshBasicSection();
	RefreshInputSection();
	RefreshCollectionSection();
	RefreshAdvancedSection();
	RefreshSectionVisibility();
}

void UCodexInteractionUIPlaygroundPopupWidget::RefreshTitleAndStatus() const
{
	if (TXT_Title != nullptr)
	{
		TXT_Title->SetText(ActivePayload != nullptr ? ActivePayload->Title : FText::FromString(TEXT("UI Playground")));
	}

	if (TXT_Status != nullptr)
	{
		const FText BaseStatus = ActivePayload != nullptr ? ActivePayload->StatusText : FText::FromString(TEXT("Ready"));
		TXT_Status->SetText(FText::Format(
			FText::FromString(TEXT("{0} [{1}]")),
			BaseStatus,
			FText::FromString(ResolveSectionLabel(ActiveSection))));
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::RefreshBasicSection() const
{
	if (TXT_BasicDescription != nullptr)
	{
		TXT_BasicDescription->SetText(ActivePayload != nullptr ? ActivePayload->BasicDescription : FText::GetEmpty());
	}

	if (ProgressBar_Sample != nullptr)
	{
		ProgressBar_Sample->SetPercent(0.42f);
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::RefreshInputSection()
{
	bIsRefreshingControls = true;

	if (ETB_InputText != nullptr)
	{
		ETB_InputText->SetText(ActivePayload != nullptr ? ActivePayload->InputText : FText::GetEmpty());
	}

	if (CHK_Toggle != nullptr)
	{
		CHK_Toggle->SetIsChecked(ActivePayload != nullptr && ActivePayload->bToggleValue);
	}

	if (SLD_Value != nullptr)
	{
		SLD_Value->SetValue(ActivePayload != nullptr ? ActivePayload->SliderValue : 0.0f);
	}

	if (SPN_Value != nullptr)
	{
		SPN_Value->SetValue(ActivePayload != nullptr ? static_cast<float>(ActivePayload->SpinValue) : 0.0f);
	}

	if (TXT_InputSummary != nullptr)
	{
		TXT_InputSummary->SetText(FText::FromString(TEXT("Try this: commit the name field, type notes, toggle the checkbox, move the slider, change the spin box, and pick a preset. The status bar should react to each input.")));
	}

	RefreshComboOptions();
	bIsRefreshingControls = false;
}

void UCodexInteractionUIPlaygroundPopupWidget::RefreshCollectionSection()
{
	UpdateListSelectionFlags();

	if (TXT_CollectionSummary != nullptr)
	{
		TXT_CollectionSummary->SetText(BuildCollectionSummaryText(ListItems, SelectedListItem));
	}

	if (ListView_Items != nullptr)
	{
		bIsRefreshingItems = true;
		ListView_Items->SetListItems(ToObjectArray(ListItems));
		if (SelectedListItem != nullptr)
		{
			ListView_Items->SetSelectedItem(SelectedListItem);
		}
		else
		{
			ListView_Items->ClearSelection();
		}
		ListView_Items->RequestRefresh();
		bIsRefreshingItems = false;
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::RefreshAdvancedSection()
{
	if (TXT_FocusStatus != nullptr)
	{
		TXT_FocusStatus->SetText(FText::FromString(TEXT("Try this: click a tile to verify selection focus, then drag a filled tile onto another slot to swap them.")));
	}

	if (TXT_DragStatus != nullptr)
	{
		TXT_DragStatus->SetText(FText::FromString(TEXT("Only tiles in the same panel can be swapped. Filled and empty slots should both respond visibly.")));
	}

	if (TileView_Slots != nullptr)
	{
		TileView_Slots->SetListItems(ToObjectArray(TileItems));
		TileView_Slots->RequestRefresh();
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::RefreshSectionVisibility()
{
	if (Switcher_Sections != nullptr)
	{
		Switcher_Sections->SetActiveWidgetIndex(ResolveSectionIndex(ActiveSection));
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::RefreshListItems()
{
	if (ListView_Items == nullptr)
	{
		return;
	}

	bIsRefreshingItems = true;
	ListView_Items->SetListItems(ToObjectArray(ListItems));
	if (SelectedListItem != nullptr)
	{
		ListView_Items->SetSelectedItem(SelectedListItem);
	}
	ListView_Items->RequestRefresh();
	bIsRefreshingItems = false;
}

void UCodexInteractionUIPlaygroundPopupWidget::RefreshTileItems()
{
	if (TileView_Slots == nullptr)
	{
		return;
	}

	TileView_Slots->SetListItems(ToObjectArray(TileItems));
	UpdateSelectionFlags();
	TileView_Slots->RequestRefresh();
}

void UCodexInteractionUIPlaygroundPopupWidget::RefreshComboOptions()
{
	if (CB_Preset == nullptr)
	{
		return;
	}

	const FString SelectedOption = ActivePayload != nullptr ? ActivePayload->SelectedPreset : FString();
	CB_Preset->ClearOptions();
	if (ActivePayload != nullptr)
	{
		for (const FString& Option : ActivePayload->PresetOptions)
		{
			CB_Preset->AddOption(Option);
		}

		if (!SelectedOption.IsEmpty())
		{
			CB_Preset->SetSelectedOption(SelectedOption);
		}
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::UpdateListSelectionFlags()
{
	for (UCodexInteractionUIPlaygroundListItem* Item : ListItems)
	{
		if (Item != nullptr)
		{
			Item->bIsSelected = Item == SelectedListItem;
		}
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::UpdateSelectionFlags()
{
	for (UCodexInteractionUIPlaygroundTileItem* Item : TileItems)
	{
		if (Item != nullptr)
		{
			Item->bIsSelected = Item == SelectedTileItem;
		}
	}

	if (TileView_Slots != nullptr)
	{
		TileView_Slots->RequestRefresh();
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::ResetPopup()
{
	if (InitialPayloadSnapshot == nullptr)
	{
		return;
	}

	SelectedListItem = nullptr;
	SelectedTileItem = nullptr;
	ActivePayload = DuplicateObject<UCodexInteractionUIPlaygroundPayload>(InitialPayloadSnapshot, this);
	ActiveSection = ActivePayload != nullptr ? ActivePayload->InitialSection : ECodexUIPlaygroundSection::Basic;
	BuildItemsFromPayload();
	RefreshAll();
}

void UCodexInteractionUIPlaygroundPopupWidget::SubmitPopupResult(const ECodexPopupResult Result)
{
	if (InteractionSubsystem == nullptr)
	{
		return;
	}

	CopyInputControlsToPayload();
	CopyListItemsToPayload();
	CopyTileItemsToPayload();

	FCodexInteractionPopupResponse Response;
	Response.RequestId = ActiveRequest.RequestId;
	Response.InteractionRequest = ActiveRequest.InteractionRequest;
	Response.Result = Result;
	Response.bWasClosed = Result == ECodexPopupResult::Closed;
	Response.UIPlaygroundPayload = ActivePayload;
	InteractionSubsystem->SubmitInteractionPopupResult(Response);
}

void UCodexInteractionUIPlaygroundPopupWidget::CopyInputControlsToPayload()
{
	if (ActivePayload == nullptr)
	{
		return;
	}

	if (ETB_InputText != nullptr)
	{
		ActivePayload->InputText = ETB_InputText->GetText();
	}

	if (CHK_Toggle != nullptr)
	{
		ActivePayload->bToggleValue = CHK_Toggle->IsChecked();
	}

	if (SLD_Value != nullptr)
	{
		ActivePayload->SliderValue = SLD_Value->GetValue();
	}

	if (SPN_Value != nullptr)
	{
		ActivePayload->SpinValue = FMath::RoundToInt(SPN_Value->GetValue());
	}

	if (CB_Preset != nullptr)
	{
		ActivePayload->SelectedPreset = CB_Preset->GetSelectedOption();
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::CopyListItemsToPayload()
{
	if (ActivePayload == nullptr)
	{
		return;
	}

	ActivePayload->ListEntries.Reset();
	for (const UCodexInteractionUIPlaygroundListItem* Item : ListItems)
	{
		if (Item == nullptr)
		{
			continue;
		}

		FCodexUIPlaygroundListEntryData Data;
		Data.Title = Item->Title;
		Data.Description = Item->Description;
		Data.StateText = Item->StateText;
		Data.Section = Item->Section;
		Data.TintColor = Item->TintColor;
		Data.bIsEnabled = Item->bIsEnabled;
		ActivePayload->ListEntries.Add(Data);
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::CopyTileItemsToPayload()
{
	if (ActivePayload == nullptr)
	{
		return;
	}

	ActivePayload->TileSlots.Reset();
	for (const UCodexInteractionUIPlaygroundTileItem* Item : TileItems)
	{
		if (Item == nullptr)
		{
			continue;
		}

		FCodexUIPlaygroundTileSlotData Data;
		Data.SlotIndex = Item->SlotIndex;
		Data.Label = Item->Label;
		Data.Value = Item->Value;
		Data.TintColor = Item->TintColor;
		Data.bIsEmpty = Item->bIsEmpty;
		Data.Section = Item->Section;
		ActivePayload->TileSlots.Add(Data);
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::EnsurePayloadContent()
{
	if (ActivePayload == nullptr)
	{
		return;
	}

	if (ActivePayload->PresetOptions.Num() == 0)
	{
		ActivePayload->PresetOptions = { TEXT("Preset A"), TEXT("Preset B"), TEXT("Preset C") };
	}

	if (ActivePayload->ListEntries.Num() == 0 || ActivePayload->TileSlots.Num() == 0)
	{
		UCodexInteractionUIPlaygroundPayload* Fallback = BuildDefaultPayload(this);
		if (ActivePayload->ListEntries.Num() == 0)
		{
			ActivePayload->ListEntries = Fallback->ListEntries;
		}

		if (ActivePayload->TileSlots.Num() == 0)
		{
			ActivePayload->TileSlots = Fallback->TileSlots;
		}
	}
}

void UCodexInteractionUIPlaygroundPopupWidget::BuildItemsFromPayload()
{
	if (bIsRefreshingItems)
	{
		return;
	}

	bIsRefreshingItems = true;
	SelectedListItem = nullptr;
	SelectedTileItem = nullptr;
	ListItems.Reset();
	TileItems.Reset();

	if (ActivePayload != nullptr)
	{
		for (const FCodexUIPlaygroundListEntryData& Data : ActivePayload->ListEntries)
		{
			ListItems.Add(CreateListItemFromData(Data));
		}

		for (const FCodexUIPlaygroundTileSlotData& Data : ActivePayload->TileSlots)
		{
			TileItems.Add(CreateTileItemFromData(Data));
		}
	}

	RefreshListItems();
	RefreshTileItems();
	bIsRefreshingItems = false;
}

UCodexInteractionUIPlaygroundListItem* UCodexInteractionUIPlaygroundPopupWidget::CreateListItemFromData(const FCodexUIPlaygroundListEntryData& Data)
{
	UCodexInteractionUIPlaygroundListItem* Item = NewObject<UCodexInteractionUIPlaygroundListItem>(this);
	Item->ApplyData(Data);
	return Item;
}

UCodexInteractionUIPlaygroundTileItem* UCodexInteractionUIPlaygroundPopupWidget::CreateTileItemFromData(const FCodexUIPlaygroundTileSlotData& Data)
{
	UCodexInteractionUIPlaygroundTileItem* Item = NewObject<UCodexInteractionUIPlaygroundTileItem>(this);
	Item->ApplyData(Data);
	return Item;
}

void UCodexInteractionUIPlaygroundPopupWidget::SetStatusMessage(const FText& Message)
{
	if (ActivePayload != nullptr)
	{
		ActivePayload->StatusText = Message;
	}

	RefreshTitleAndStatus();
}

TSubclassOf<UUserWidget> UCodexInteractionUIPlaygroundPopupWidget::ResolveListEntryWidgetClass() const
{
	const TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(
		nullptr,
		*CodexInteractionAssetPaths::MakeGeneratedClassObjectPath(CodexInteractionAssetPaths::UIPlaygroundListEntryWidgetObjectPath));
	if (WidgetClass != nullptr)
	{
		return WidgetClass;
	}

	return UCodexInteractionUIPlaygroundListEntryWidget::StaticClass();
}

TSubclassOf<UUserWidget> UCodexInteractionUIPlaygroundPopupWidget::ResolveTileEntryWidgetClass() const
{
	const TSubclassOf<UUserWidget> WidgetClass = LoadClass<UUserWidget>(
		nullptr,
		*CodexInteractionAssetPaths::MakeGeneratedClassObjectPath(CodexInteractionAssetPaths::UIPlaygroundTileEntryWidgetObjectPath));
	if (WidgetClass != nullptr)
	{
		return WidgetClass;
	}

	return UCodexInteractionUIPlaygroundTileEntryWidget::StaticClass();
}

int32 UCodexInteractionUIPlaygroundPopupWidget::ResolveSectionIndex(ECodexUIPlaygroundSection Section)
{
	switch (Section)
	{
	case ECodexUIPlaygroundSection::Basic:
		return 0;
	case ECodexUIPlaygroundSection::Input:
		return 1;
	case ECodexUIPlaygroundSection::Collection:
		return 2;
	case ECodexUIPlaygroundSection::Advanced:
	default:
		return 3;
	}
}

FString UCodexInteractionUIPlaygroundPopupWidget::ResolveSectionLabel(ECodexUIPlaygroundSection Section)
{
	switch (Section)
	{
	case ECodexUIPlaygroundSection::Basic:
		return TEXT("Basic");
	case ECodexUIPlaygroundSection::Input:
		return TEXT("Input");
	case ECodexUIPlaygroundSection::Collection:
		return TEXT("Collection");
	case ECodexUIPlaygroundSection::Advanced:
	default:
		return TEXT("Advanced");
	}
}
