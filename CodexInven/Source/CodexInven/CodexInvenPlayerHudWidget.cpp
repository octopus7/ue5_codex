// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenPlayerHudWidget.h"

#include "Blueprint/WidgetTree.h"
#include "CodexInvenInventoryIconSubsystem.h"
#include "CodexInvenInventoryTileEntryWidget.h"
#include "CodexInvenInventoryTileItemObject.h"
#include "CodexInvenOwnershipComponent.h"
#include "CodexInvenOwnershipDebugWidget.h"
#include "CodexInvenTopDownCharacter.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Engine/GameInstance.h"
#include "Styling/SlateColor.h"
#include "Widgets/Layout/Anchors.h"

namespace
{
	constexpr float HudMargin = 24.0f;
	constexpr float InventoryPanelWidth = 640.0f;
	constexpr float InventoryPanelHeight = 480.0f;
	constexpr float InventoryTileEntrySize = 111.0f;
	constexpr float InventoryTileSpacing = 8.0f;
	constexpr float InventoryPanelInnerPadding = 16.0f;
	const TCHAR* InventoryLabelText = TEXT("\uC18C\uC720\uBB3C");
	const TCHAR* DebugLabelText = TEXT("\uB514\uBC84\uADF8");
	const TCHAR* PickupPageLabelText = TEXT("\uD53D\uC5C5 \uD398\uC774\uC9C0");
	const TCHAR* LightColorLabelText = TEXT("Light Color");

	UButton* CreateHudButton(UWidgetTree& InWidgetTree, const FName InButtonName, const FText& InLabel)
	{
		UButton* Button = InWidgetTree.ConstructWidget<UButton>(UButton::StaticClass(), InButtonName);
		Button->SetBackgroundColor(FLinearColor(0.10f, 0.14f, 0.18f, 0.96f));

		UTextBlock* LabelText = InWidgetTree.ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), *FString::Printf(TEXT("%sLabel"), *InButtonName.ToString()));
		LabelText->SetText(InLabel);
		LabelText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		LabelText->SetJustification(ETextJustify::Center);
		Button->AddChild(LabelText);

		if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(LabelText->Slot))
		{
			ButtonSlot->SetPadding(FMargin(14.0f, 8.0f));
			ButtonSlot->SetHorizontalAlignment(HAlign_Center);
			ButtonSlot->SetVerticalAlignment(VAlign_Center);
		}

		return Button;
	}
}

void UCodexInvenPlayerHudWidget::SetObservedOwnershipComponent(UCodexInvenOwnershipComponent* InComponent)
{
	BuildWidgetTreeIfNeeded();

	if (ObservedOwnershipComponent == InComponent)
	{
		RefreshInventoryItems();
		if (OwnershipDebugWidget != nullptr)
		{
			OwnershipDebugWidget->SetObservedOwnershipComponent(InComponent);
		}
		return;
	}

	EndInventoryDrag();
	UnbindObservedOwnershipComponent();
	ObservedOwnershipComponent = InComponent;

	if (ObservedOwnershipComponent != nullptr)
	{
		ObservedOwnershipComponent->OnInventoryChanged.AddUObject(this, &ThisClass::HandleInventoryChanged);
	}

	if (OwnershipDebugWidget != nullptr)
	{
		OwnershipDebugWidget->SetObservedOwnershipComponent(ObservedOwnershipComponent);
	}

	RefreshInventoryItems();
}

void UCodexInvenPlayerHudWidget::BeginInventoryDrag(const int32 InSourceSlotIndex)
{
	ActiveDragSourceSlotIndex = InSourceSlotIndex;
	HoveredDropTargetSlotIndex = INDEX_NONE;
	RefreshInventoryEntryVisualStates();
}

void UCodexInvenPlayerHudWidget::SetHoveredInventoryDropTarget(const int32 InTargetSlotIndex)
{
	if (HoveredDropTargetSlotIndex == InTargetSlotIndex)
	{
		return;
	}

	HoveredDropTargetSlotIndex = InTargetSlotIndex;
	RefreshInventoryEntryVisualStates();
}

void UCodexInvenPlayerHudWidget::ClearHoveredInventoryDropTarget(const int32 InExpectedTargetSlotIndex)
{
	if (InExpectedTargetSlotIndex != INDEX_NONE && HoveredDropTargetSlotIndex != InExpectedTargetSlotIndex)
	{
		return;
	}

	if (HoveredDropTargetSlotIndex == INDEX_NONE)
	{
		return;
	}

	HoveredDropTargetSlotIndex = INDEX_NONE;
	RefreshInventoryEntryVisualStates();
}

void UCodexInvenPlayerHudWidget::EndInventoryDrag()
{
	if (ActiveDragSourceSlotIndex == INDEX_NONE && HoveredDropTargetSlotIndex == INDEX_NONE)
	{
		return;
	}

	ActiveDragSourceSlotIndex = INDEX_NONE;
	HoveredDropTargetSlotIndex = INDEX_NONE;
	RefreshInventoryEntryVisualStates();
}

bool UCodexInvenPlayerHudWidget::HandleInventorySlotDrop(const int32 InSourceSlotIndex, const int32 InTargetSlotIndex)
{
	if (ObservedOwnershipComponent == nullptr)
	{
		EndInventoryDrag();
		return false;
	}

	if (InSourceSlotIndex == InTargetSlotIndex)
	{
		EndInventoryDrag();
		return true;
	}

	const TArray<FCodexInvenInventorySlotData> Snapshot = ObservedOwnershipComponent->BuildInventorySnapshot();
	if (!Snapshot.IsValidIndex(InSourceSlotIndex) || !Snapshot.IsValidIndex(InTargetSlotIndex) || Snapshot[InSourceSlotIndex].bIsEmpty)
	{
		EndInventoryDrag();
		return false;
	}

	EndInventoryDrag();
	return Snapshot[InTargetSlotIndex].bIsEmpty
		? ObservedOwnershipComponent->MoveInventorySlot(InSourceSlotIndex, InTargetSlotIndex)
		: ObservedOwnershipComponent->SwapInventorySlots(InSourceSlotIndex, InTargetSlotIndex);
}

bool UCodexInvenPlayerHudWidget::ShouldBlockFireInput() const
{
	return IsInventoryPanelVisible() ||
		ActiveDragSourceSlotIndex != INDEX_NONE ||
		IsButtonHovered(InventoryToggleButton) ||
		IsButtonHovered(PickupPageToggleButton) ||
		IsButtonHovered(LightColorToggleButton) ||
		IsButtonHovered(DebugToggleButton);
}

bool UCodexInvenPlayerHudWidget::IsInventoryPanelVisible() const
{
	return InventoryPanelSizeBox != nullptr && InventoryPanelSizeBox->GetVisibility() == ESlateVisibility::Visible;
}

FOnCodexInvenPickupPageToggleRequested& UCodexInvenPlayerHudWidget::OnPickupPageToggleRequested()
{
	return PickupPageToggleRequested;
}

void UCodexInvenPlayerHudWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(false);
	BuildWidgetTreeIfNeeded();

	if (InventoryToggleButton != nullptr)
	{
		InventoryToggleButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleInventoryToggleClicked);
	}

	if (DebugToggleButton != nullptr)
	{
		DebugToggleButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleDebugToggleClicked);
	}

	if (PickupPageToggleButton != nullptr)
	{
		PickupPageToggleButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandlePickupPageToggleClicked);
	}

	if (LightColorToggleButton != nullptr)
	{
		LightColorToggleButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleLightColorToggleClicked);
	}

	if (IncreaseCapacityButton != nullptr)
	{
		IncreaseCapacityButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleIncreaseCapacityClicked);
	}

	SetInventoryPanelVisible(bIsInventoryVisible);
	SetDebugWidgetVisible(bIsDebugVisible);
	RefreshInventoryItems();
}

void UCodexInvenPlayerHudWidget::NativeDestruct()
{
	EndInventoryDrag();
	UnbindObservedOwnershipComponent();

	Super::NativeDestruct();
}

void UCodexInvenPlayerHudWidget::BuildWidgetTreeIfNeeded()
{
	if (WidgetTree == nullptr)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("PlayerHudWidgetTree"));
	}

	if (WidgetTree->RootWidget == nullptr)
	{
		RootCanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvasPanel"));
		WidgetTree->RootWidget = RootCanvasPanel;
	}
	else if (RootCanvasPanel == nullptr)
	{
		RootCanvasPanel = Cast<UCanvasPanel>(WidgetTree->RootWidget);
	}

	if (RootCanvasPanel == nullptr)
	{
		return;
	}

	if (InventoryToggleButton == nullptr || DebugToggleButton == nullptr || PickupPageToggleButton == nullptr || LightColorToggleButton == nullptr)
	{
		UHorizontalBox* ActionBarBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ActionBarBox"));
		InventoryToggleButton = CreateHudButton(*WidgetTree, TEXT("InventoryToggleButton"), FText::FromString(FString(InventoryLabelText)));
		PickupPageToggleButton = CreateHudButton(*WidgetTree, TEXT("PickupPageToggleButton"), FText::FromString(FString(PickupPageLabelText)));
		LightColorToggleButton = CreateHudButton(*WidgetTree, TEXT("LightColorToggleButton"), FText::FromString(FString(LightColorLabelText)));
		DebugToggleButton = CreateHudButton(*WidgetTree, TEXT("DebugToggleButton"), FText::FromString(FString(DebugLabelText)));

		if (UHorizontalBoxSlot* InventoryButtonSlot = ActionBarBox->AddChildToHorizontalBox(InventoryToggleButton))
		{
			InventoryButtonSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
		}

		if (UHorizontalBoxSlot* PickupPageButtonSlot = ActionBarBox->AddChildToHorizontalBox(PickupPageToggleButton))
		{
			PickupPageButtonSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
		}

		if (UHorizontalBoxSlot* LightColorButtonSlot = ActionBarBox->AddChildToHorizontalBox(LightColorToggleButton))
		{
			LightColorButtonSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
		}

		ActionBarBox->AddChildToHorizontalBox(DebugToggleButton);

		if (UCanvasPanelSlot* ActionBarSlot = RootCanvasPanel->AddChildToCanvas(ActionBarBox))
		{
			ActionBarSlot->SetAutoSize(true);
			ActionBarSlot->SetAnchors(FAnchors(0.0f, 1.0f, 0.0f, 1.0f));
			ActionBarSlot->SetAlignment(FVector2D(0.0f, 1.0f));
			ActionBarSlot->SetPosition(FVector2D(HudMargin, -HudMargin));
		}
	}

	if (InventoryPanelSizeBox == nullptr || InventoryPanelBorder == nullptr || InventoryScrollBox == nullptr || InventoryWrapBox == nullptr || InventorySummaryTextBlock == nullptr || IncreaseCapacityButton == nullptr)
	{
		InventoryPanelSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("InventoryPanelSizeBox"));
		InventoryPanelSizeBox->SetWidthOverride(InventoryPanelWidth);
		InventoryPanelSizeBox->SetHeightOverride(InventoryPanelHeight);

		InventoryPanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("InventoryPanelBorder"));
		InventoryPanelBorder->SetPadding(FMargin(InventoryPanelInnerPadding));
		InventoryPanelBorder->SetBrushColor(FLinearColor(0.02f, 0.04f, 0.06f, 0.92f));
		InventoryPanelSizeBox->SetContent(InventoryPanelBorder);

		UVerticalBox* InventoryContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("InventoryContentBox"));
		InventoryPanelBorder->SetContent(InventoryContentBox);

		UHorizontalBox* InventoryHeaderBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("InventoryHeaderBox"));
		if (UVerticalBoxSlot* HeaderSlot = InventoryContentBox->AddChildToVerticalBox(InventoryHeaderBox))
		{
			HeaderSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 12.0f));
			HeaderSlot->SetHorizontalAlignment(HAlign_Fill);
		}

		UTextBlock* TitleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("InventoryTitleText"));
		TitleText->SetText(FText::FromString(FString(InventoryLabelText)));
		TitleText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		TitleText->SetJustification(ETextJustify::Left);
		if (UHorizontalBoxSlot* TitleSlot = InventoryHeaderBox->AddChildToHorizontalBox(TitleText))
		{
			TitleSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			TitleSlot->SetHorizontalAlignment(HAlign_Left);
			TitleSlot->SetVerticalAlignment(VAlign_Center);
		}

		InventorySummaryTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("InventorySummaryTextBlock"));
		InventorySummaryTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		InventorySummaryTextBlock->SetJustification(ETextJustify::Right);
		if (UHorizontalBoxSlot* SummarySlot = InventoryHeaderBox->AddChildToHorizontalBox(InventorySummaryTextBlock))
		{
			SummarySlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
			SummarySlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
			SummarySlot->SetHorizontalAlignment(HAlign_Right);
			SummarySlot->SetVerticalAlignment(VAlign_Center);
		}

		IncreaseCapacityButton = CreateHudButton(*WidgetTree, TEXT("IncreaseCapacityButton"), FText::FromString(TEXT("+10")));
		if (UHorizontalBoxSlot* IncreaseButtonSlot = InventoryHeaderBox->AddChildToHorizontalBox(IncreaseCapacityButton))
		{
			IncreaseButtonSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
			IncreaseButtonSlot->SetHorizontalAlignment(HAlign_Right);
			IncreaseButtonSlot->SetVerticalAlignment(VAlign_Center);
		}

		InventoryScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("InventoryScrollBox"));
		InventoryScrollBox->SetOrientation(Orient_Vertical);
		if (UVerticalBoxSlot* ScrollBoxSlot = InventoryContentBox->AddChildToVerticalBox(InventoryScrollBox))
		{
			ScrollBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			ScrollBoxSlot->SetHorizontalAlignment(HAlign_Fill);
			ScrollBoxSlot->SetVerticalAlignment(VAlign_Fill);
		}

		InventoryWrapBox = WidgetTree->ConstructWidget<UWrapBox>(UWrapBox::StaticClass(), TEXT("InventoryWrapBox"));
		InventoryWrapBox->SetOrientation(Orient_Horizontal);
		InventoryWrapBox->SetInnerSlotPadding(FVector2D(InventoryTileSpacing, InventoryTileSpacing));
		InventoryWrapBox->SetExplicitWrapSize(true);
		InventoryWrapBox->SetWrapSize(InventoryPanelWidth - (InventoryPanelInnerPadding * 2.0f) - 20.0f);
		InventoryScrollBox->AddChild(InventoryWrapBox);

		if (UCanvasPanelSlot* InventoryPanelSlot = RootCanvasPanel->AddChildToCanvas(InventoryPanelSizeBox))
		{
			InventoryPanelSlot->SetAnchors(FAnchors(0.5f, 0.5f, 0.5f, 0.5f));
			InventoryPanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			InventoryPanelSlot->SetAutoSize(false);
			InventoryPanelSlot->SetSize(FVector2D(InventoryPanelWidth, InventoryPanelHeight));
			InventoryPanelSlot->SetPosition(FVector2D::ZeroVector);
		}
	}

	if (OwnershipDebugWidget == nullptr)
	{
		OwnershipDebugWidget = WidgetTree->ConstructWidget<UCodexInvenOwnershipDebugWidget>(UCodexInvenOwnershipDebugWidget::StaticClass(), TEXT("OwnershipDebugWidget"));
		if (UCanvasPanelSlot* DebugWidgetSlot = RootCanvasPanel->AddChildToCanvas(OwnershipDebugWidget))
		{
			DebugWidgetSlot->SetAutoSize(true);
			DebugWidgetSlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
			DebugWidgetSlot->SetAlignment(FVector2D(1.0f, 0.0f));
			DebugWidgetSlot->SetPosition(FVector2D(-HudMargin, HudMargin));
		}
	}
}

void UCodexInvenPlayerHudWidget::RefreshInventoryItems()
{
	if (InventoryWrapBox == nullptr || WidgetTree == nullptr)
	{
		return;
	}

	InventoryTileItems.Reset();
	InventoryEntryWidgets.Reset();
	InventoryWrapBox->ClearChildren();

	if (ObservedOwnershipComponent != nullptr)
	{
		UCodexInvenInventoryIconSubsystem* IconSubsystem = nullptr;
		if (UGameInstance* GameInstance = GetGameInstance())
		{
			IconSubsystem = GameInstance->GetSubsystem<UCodexInvenInventoryIconSubsystem>();
		}

		const TArray<FCodexInvenInventorySlotData> Snapshot = ObservedOwnershipComponent->BuildInventorySnapshot();
		InventoryTileItems.Reserve(Snapshot.Num());
		InventoryEntryWidgets.Reserve(Snapshot.Num());

		for (const FCodexInvenInventorySlotData& SlotData : Snapshot)
		{
			UCodexInvenInventoryTileItemObject* TileItem = NewObject<UCodexInvenInventoryTileItemObject>(this);
			TileItem->InitializeItem(
				SlotData,
				IconSubsystem != nullptr ? IconSubsystem->GetInventoryIcon(SlotData.PickupType) : nullptr,
				IconSubsystem != nullptr ? IconSubsystem->GetInventorySlotBackground(SlotData.Rarity) : nullptr);
			InventoryTileItems.Add(TileItem);

			const FName EntryWidgetName = MakeUniqueObjectName(this, UCodexInvenInventoryTileEntryWidget::StaticClass(), TEXT("InventoryTileEntry"));
			UCodexInvenInventoryTileEntryWidget* EntryWidget = WidgetTree->ConstructWidget<UCodexInvenInventoryTileEntryWidget>(UCodexInvenInventoryTileEntryWidget::StaticClass(), EntryWidgetName);
			EntryWidget->SetOwningHudWidget(this);
			EntryWidget->SetTileItemObject(TileItem);
			InventoryEntryWidgets.Add(EntryWidget);

			const FName EntrySizeBoxName = MakeUniqueObjectName(this, USizeBox::StaticClass(), TEXT("InventoryTileEntrySizeBox"));
			USizeBox* EntrySizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), EntrySizeBoxName);
			EntrySizeBox->SetWidthOverride(InventoryTileEntrySize);
			EntrySizeBox->SetHeightOverride(InventoryTileEntrySize);
			EntrySizeBox->SetContent(EntryWidget);

			if (UWrapBoxSlot* EntrySlot = InventoryWrapBox->AddChildToWrapBox(EntrySizeBox))
			{
				EntrySlot->SetFillEmptySpace(false);
				EntrySlot->SetFillSpanWhenLessThan(0.0f);
				EntrySlot->SetHorizontalAlignment(HAlign_Fill);
				EntrySlot->SetVerticalAlignment(VAlign_Fill);
			}
		}
	}

	RefreshInventorySummaryText();
	RefreshInventoryEntryVisualStates();
}

void UCodexInvenPlayerHudWidget::RefreshInventorySummaryText() const
{
	if (InventorySummaryTextBlock == nullptr)
	{
		return;
	}

	if (ObservedOwnershipComponent == nullptr)
	{
		InventorySummaryTextBlock->SetText(FText::FromString(TEXT("Slots 0 / 0")));
		return;
	}

	InventorySummaryTextBlock->SetText(FText::Format(
		FText::FromString(TEXT("Slots {0} / {1}")),
		FText::AsNumber(ObservedOwnershipComponent->GetOccupiedSlotCount()),
		FText::AsNumber(ObservedOwnershipComponent->GetInventoryCapacity())));
}

void UCodexInvenPlayerHudWidget::RefreshInventoryEntryVisualStates() const
{
	for (UCodexInvenInventoryTileEntryWidget* EntryWidget : InventoryEntryWidgets)
	{
		if (EntryWidget == nullptr)
		{
			continue;
		}

		const int32 SlotIndex = EntryWidget->GetSlotIndex();
		EntryWidget->SetVisualState(
			SlotIndex != INDEX_NONE && SlotIndex == ActiveDragSourceSlotIndex,
			SlotIndex != INDEX_NONE && SlotIndex == HoveredDropTargetSlotIndex);
	}
}

void UCodexInvenPlayerHudWidget::SetInventoryPanelVisible(const bool bInVisible)
{
	bIsInventoryVisible = bInVisible;

	if (InventoryPanelSizeBox != nullptr)
	{
		InventoryPanelSizeBox->SetVisibility(bIsInventoryVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (!bIsInventoryVisible)
	{
		EndInventoryDrag();
	}
}

void UCodexInvenPlayerHudWidget::SetDebugWidgetVisible(const bool bInVisible)
{
	bIsDebugVisible = bInVisible;

	if (OwnershipDebugWidget != nullptr)
	{
		OwnershipDebugWidget->SetVisibility(bIsDebugVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	}
}

void UCodexInvenPlayerHudWidget::UnbindObservedOwnershipComponent()
{
	if (ObservedOwnershipComponent != nullptr)
	{
		ObservedOwnershipComponent->OnInventoryChanged.RemoveAll(this);
		ObservedOwnershipComponent = nullptr;
	}
}

void UCodexInvenPlayerHudWidget::HandleInventoryChanged()
{
	RefreshInventoryItems();
}

bool UCodexInvenPlayerHudWidget::IsButtonHovered(const UButton* InButton) const
{
	return InButton != nullptr && InButton->IsHovered();
}

void UCodexInvenPlayerHudWidget::HandleInventoryToggleClicked()
{
	SetInventoryPanelVisible(!bIsInventoryVisible);
}

void UCodexInvenPlayerHudWidget::HandleDebugToggleClicked()
{
	SetDebugWidgetVisible(!bIsDebugVisible);
}

void UCodexInvenPlayerHudWidget::HandleIncreaseCapacityClicked()
{
	if (ObservedOwnershipComponent != nullptr)
	{
		ObservedOwnershipComponent->IncreaseInventoryCapacity(10);
	}
}

void UCodexInvenPlayerHudWidget::HandlePickupPageToggleClicked()
{
	PickupPageToggleRequested.Broadcast();
}

void UCodexInvenPlayerHudWidget::HandleLightColorToggleClicked()
{
	if (ACodexInvenTopDownCharacter* const TopDownCharacter = Cast<ACodexInvenTopDownCharacter>(GetOwningPlayerPawn()))
	{
		TopDownCharacter->LightColorToggle();
	}
}
