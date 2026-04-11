// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/ComboBoxString.h"
#include "Interaction/CodexInteractionUIPlaygroundPayload.h"
#include "Interaction/CodexInteractionTypes.h"
#include "CodexInteractionUIPlaygroundPopupWidget.generated.h"

class UButton;
class UCheckBox;
class UComboBoxString;
class UEditableTextBox;
class UListView;
class UMultiLineEditableTextBox;
class UProgressBar;
class USlider;
class USpinBox;
class UTextBlock;
class UTileView;
class UWidgetSwitcher;
class UCodexInteractionSubsystem;
class UCodexInteractionUIPlaygroundListItem;
class UCodexInteractionUIPlaygroundTileItem;
class UCodexUIPlaygroundDragDropOperation;
class UDragDropOperation;
struct FKeyEvent;

UCLASS(Abstract, Blueprintable)
class CODEXUMG_API UCodexInteractionUIPlaygroundPopupWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	void ApplyPopupRequest(const FCodexInteractionPopupRequest& NewRequest, UCodexInteractionSubsystem& InInteractionSubsystem);
	void NotifyTileDragStarted();
	bool CanDropOnSlot(const UCodexInteractionUIPlaygroundTileItem* TargetItem, const UCodexUIPlaygroundDragDropOperation* Operation) const;
	bool TryHandleDropOnSlot(UCodexInteractionUIPlaygroundTileItem* TargetItem, UDragDropOperation* InOperation);
	void SetActiveSection(ECodexUIPlaygroundSection InSection);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Title;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_Status;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_BasicDescription;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_InputSummary;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_CollectionSummary;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_FocusStatus;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TXT_DragStatus;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UEditableTextBox> ETB_InputText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UMultiLineEditableTextBox> MultiLineEditableTextBox_Notes;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UCheckBox> CHK_Toggle;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> ProgressBar_Sample;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<USlider> SLD_Value;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<USpinBox> SPN_Value;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UComboBoxString> CB_Preset;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UListView> ListView_Items;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTileView> TileView_Slots;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidgetSwitcher> Switcher_Sections;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Close;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Reset;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Submit;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Primary;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_Secondary;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_StatusPing;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_TabBasic;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_TabInput;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_TabCollection;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_TabAdvanced;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_ListAdd;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_ListRemove;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BTN_ListToggleSelection;

private:
	UFUNCTION()
	void HandleCloseClicked();

	UFUNCTION()
	void HandleResetClicked();

	UFUNCTION()
	void HandleSubmitClicked();

	UFUNCTION()
	void HandlePrimaryClicked();

	UFUNCTION()
	void HandleSecondaryClicked();

	UFUNCTION()
	void HandleStatusPingClicked();

	UFUNCTION()
	void HandleTabBasicClicked();

	UFUNCTION()
	void HandleTabInputClicked();

	UFUNCTION()
	void HandleTabCollectionClicked();

	UFUNCTION()
	void HandleTabAdvancedClicked();

	UFUNCTION()
	void HandleInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void HandleNotesTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void HandleToggleChanged(bool bIsChecked);

	UFUNCTION()
	void HandleSliderChanged(float Value);

	UFUNCTION()
	void HandleSpinBoxChanged(float Value);

	UFUNCTION()
	void HandlePresetChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void HandleListSelectionChanged(UObject* SelectedItem);

	UFUNCTION()
	void HandleListAddClicked();

	UFUNCTION()
	void HandleListRemoveClicked();

	UFUNCTION()
	void HandleListToggleSelectionClicked();

	UFUNCTION()
	void HandleTileSelectionChanged(UObject* SelectedItem);

	void RefreshAll();
	void RefreshTitleAndStatus() const;
	void RefreshBasicSection() const;
	void RefreshInputSection();
	void RefreshCollectionSection();
	void RefreshAdvancedSection();
	void RefreshSectionVisibility();
	void RefreshListItems();
	void RefreshTileItems();
	void RefreshComboOptions();
	void UpdateListSelectionFlags();
	void UpdateSelectionFlags();
	void ResetPopup();
	void SubmitPopupResult(ECodexPopupResult Result);
	void CopyInputControlsToPayload();
	void CopyListItemsToPayload();
	void CopyTileItemsToPayload();
	void EnsurePayloadContent();
	void BuildItemsFromPayload();
	void SetStatusMessage(const FText& Message);
	UCodexInteractionUIPlaygroundListItem* CreateListItemFromData(const FCodexUIPlaygroundListEntryData& Data);
	UCodexInteractionUIPlaygroundTileItem* CreateTileItemFromData(const FCodexUIPlaygroundTileSlotData& Data);
	TSubclassOf<UUserWidget> ResolveListEntryWidgetClass() const;
	TSubclassOf<UUserWidget> ResolveTileEntryWidgetClass() const;
	static int32 ResolveSectionIndex(ECodexUIPlaygroundSection Section);
	static FString ResolveSectionLabel(ECodexUIPlaygroundSection Section);

	FCodexInteractionPopupRequest ActiveRequest;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionSubsystem> InteractionSubsystem;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionUIPlaygroundPayload> ActivePayload;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionUIPlaygroundPayload> InitialPayloadSnapshot;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCodexInteractionUIPlaygroundListItem>> ListItems;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCodexInteractionUIPlaygroundTileItem>> TileItems;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionUIPlaygroundListItem> SelectedListItem;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionUIPlaygroundTileItem> SelectedTileItem;

	UPROPERTY(Transient)
	ECodexUIPlaygroundSection ActiveSection = ECodexUIPlaygroundSection::Basic;

	bool bIsRefreshingControls = false;
	bool bIsRefreshingItems = false;
	bool bIsApplyingSection = false;
};
