#include "CodexInvenAttendanceWidgetBase.h"

#include "Blueprint/WidgetTree.h"
#include "CodexInvenAttendanceDayEntryWidget.h"
#include "CodexInvenAttendanceSubsystem.h"
#include "CodexInvenOwnershipComponent.h"
#include "CodexInvenTopDownCharacter.h"
#include "Components/Button.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

namespace
{
	const TCHAR* ClaimCompleteText = TEXT("\uBAA8\uB4E0 \uCD9C\uC11D \uBCF4\uC0C1 \uC218\uB839 \uC644\uB8CC");
	const TCHAR* ClaimedTodayText = TEXT("\uC624\uB298 \uBCF4\uC0C1\uC740 \uC774\uBBF8 \uC218\uB839\uD588\uC2B5\uB2C8\uB2E4");
	const TCHAR* ClaimReadyText = TEXT("\uC624\uB298 \uBCF4\uC0C1\uC744 \uC218\uB839\uD560 \uC218 \uC788\uC2B5\uB2C8\uB2E4");
	const TCHAR* InventoryFullText = TEXT("\uC778\uBCA4\uD1A0\uB9AC \uACF5\uAC04\uC774 \uBD80\uC871\uD574 \uBCF4\uC0C1\uC744 \uC218\uB839\uD560 \uC218 \uC5C6\uC2B5\uB2C8\uB2E4");
	const TCHAR* ClaimFailedText = TEXT("\uBCF4\uC0C1 \uC218\uB839\uC5D0 \uC2E4\uD328\uD588\uC2B5\uB2C8\uB2E4");
}

void UCodexInvenAttendanceWidgetBase::InitializeForEvent(const FName InEventId)
{
	EventId = InEventId;
	bHasStatusOverrideText = false;
	RefreshFromSubsystem();
}

FName UCodexInvenAttendanceWidgetBase::GetEventId() const
{
	return EventId;
}

bool UCodexInvenAttendanceWidgetBase::SupportsConfiguredDayCount(const int32 InTotalDays) const
{
	return ExpectedTotalDays <= 0 || ExpectedTotalDays == InTotalDays;
}

FOnCodexInvenAttendanceWidgetClosed& UCodexInvenAttendanceWidgetBase::OnAttendanceWidgetClosed()
{
	return AttendanceWidgetClosed;
}

void UCodexInvenAttendanceWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(true);

	if (ClaimButton != nullptr)
	{
		ClaimButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleClaimButtonClicked);
	}

	if (CloseButton != nullptr)
	{
		CloseButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleCloseButtonClicked);
	}
}

void UCodexInvenAttendanceWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	bHasClosed = false;
	RefreshFromSubsystem();
}

UCodexInvenAttendanceSubsystem* UCodexInvenAttendanceWidgetBase::GetAttendanceSubsystem() const
{
	if (RuntimeAttendanceSubsystem != nullptr)
	{
		return RuntimeAttendanceSubsystem;
	}

	if (const UGameInstance* const GameInstance = GetGameInstance())
	{
		return const_cast<UGameInstance*>(GameInstance)->GetSubsystem<UCodexInvenAttendanceSubsystem>();
	}

	return nullptr;
}

UCodexInvenOwnershipComponent* UCodexInvenAttendanceWidgetBase::GetOwnershipComponent() const
{
	const APlayerController* const OwningPlayerController = GetOwningPlayer();
	if (OwningPlayerController == nullptr)
	{
		return nullptr;
	}

	const ACodexInvenTopDownCharacter* const TopDownCharacter = Cast<ACodexInvenTopDownCharacter>(OwningPlayerController->GetPawn());
	return TopDownCharacter != nullptr ? TopDownCharacter->GetOwnershipComponent() : nullptr;
}

void UCodexInvenAttendanceWidgetBase::RefreshFromSubsystem()
{
	if (EventId.IsNone())
	{
		return;
	}

	RuntimeAttendanceSubsystem = GetAttendanceSubsystem();
	if (RuntimeAttendanceSubsystem == nullptr)
	{
		return;
	}

	FCodexInvenAttendanceEventPresentationData PresentationData;
	if (!RuntimeAttendanceSubsystem->GetEventPresentationData(EventId, PresentationData))
	{
		return;
	}

	const bool bCanFitReward = CanCurrentRewardFitInventory(PresentationData);

	if (EventTitleTextBlock != nullptr)
	{
		EventTitleTextBlock->SetText(PresentationData.Title);
	}

	if (StatusTextBlock != nullptr)
	{
		StatusTextBlock->SetText(BuildStatusText(PresentationData, bCanFitReward));
	}

	if (ClaimButton != nullptr)
	{
		ClaimButton->SetIsEnabled(PresentationData.bCanClaimToday && bCanFitReward);
	}

	RebuildDayEntries(PresentationData);
}

void UCodexInvenAttendanceWidgetBase::RebuildDayEntries(const FCodexInvenAttendanceEventPresentationData& InPresentationData)
{
	if (DayEntryPanel == nullptr || WidgetTree == nullptr)
	{
		return;
	}

	RuntimeDayEntryWidgets.Reset();
	DayEntryPanel->ClearChildren();

	for (const FCodexInvenAttendanceDayPresentationData& DayEntryData : InPresentationData.DayEntries)
	{
		UCodexInvenAttendanceDayEntryWidget* const DayEntryWidget =
			WidgetTree->ConstructWidget<UCodexInvenAttendanceDayEntryWidget>(
				UCodexInvenAttendanceDayEntryWidget::StaticClass(),
				MakeUniqueObjectName(this, UCodexInvenAttendanceDayEntryWidget::StaticClass(), TEXT("AttendanceDayEntry")));
		if (DayEntryWidget == nullptr)
		{
			continue;
		}

		DayEntryWidget->SetPresentationData(DayEntryData);
		RuntimeDayEntryWidgets.Add(DayEntryWidget);

		USizeBox* const EntrySizeBox =
			WidgetTree->ConstructWidget<USizeBox>(
				USizeBox::StaticClass(),
				MakeUniqueObjectName(this, USizeBox::StaticClass(), TEXT("AttendanceDayEntrySizeBox")));
		EntrySizeBox->SetWidthOverride(DayEntryWidth);
		EntrySizeBox->SetHeightOverride(DayEntryHeight);
		EntrySizeBox->SetContent(DayEntryWidget);

		UPanelSlot* const PanelSlot = DayEntryPanel->AddChild(EntrySizeBox);
		if (UHorizontalBoxSlot* const HorizontalBoxSlot = Cast<UHorizontalBoxSlot>(PanelSlot))
		{
			HorizontalBoxSlot->SetPadding(FMargin(0.0f, 0.0f, DayEntrySpacing, 0.0f));
		}
		else if (UScrollBoxSlot* const ScrollBoxSlot = Cast<UScrollBoxSlot>(PanelSlot))
		{
			ScrollBoxSlot->SetPadding(FMargin(0.0f, 0.0f, DayEntrySpacing, 0.0f));
		}
	}
}

bool UCodexInvenAttendanceWidgetBase::CanCurrentRewardFitInventory(
	const FCodexInvenAttendanceEventPresentationData& InPresentationData) const
{
	if (!InPresentationData.bCanClaimToday || InPresentationData.NextClaimDayNumber == INDEX_NONE)
	{
		return false;
	}

	const UCodexInvenOwnershipComponent* const OwnershipComponent = GetOwnershipComponent();
	return OwnershipComponent != nullptr && OwnershipComponent->CanAddPickup(
		InPresentationData.NextReward.PickupType,
		InPresentationData.NextReward.Quantity);
}

FText UCodexInvenAttendanceWidgetBase::BuildStatusText(
	const FCodexInvenAttendanceEventPresentationData& InPresentationData,
	const bool bInCanFitReward) const
{
	if (bHasStatusOverrideText)
	{
		return StatusOverrideText;
	}

	if (InPresentationData.bIsCompleted)
	{
		return FText::FromString(FString(ClaimCompleteText));
	}

	if (InPresentationData.bClaimedToday)
	{
		return FText::FromString(FString(ClaimedTodayText));
	}

	if (!bInCanFitReward)
	{
		return FText::FromString(FString(InventoryFullText));
	}

	return FText::Format(
		FText::FromString(TEXT("{0} / {1} - {2}")),
		FText::AsNumber(InPresentationData.ClaimedDaysCount),
		FText::AsNumber(InPresentationData.TotalDays),
		FText::FromString(FString(ClaimReadyText)));
}

void UCodexInvenAttendanceWidgetBase::CloseAttendanceWidget()
{
	if (bHasClosed)
	{
		return;
	}

	bHasClosed = true;
	AttendanceWidgetClosed.Broadcast(EventId);
	RemoveFromParent();
}

void UCodexInvenAttendanceWidgetBase::HandleClaimButtonClicked()
{
	RuntimeAttendanceSubsystem = GetAttendanceSubsystem();
	UCodexInvenOwnershipComponent* const OwnershipComponent = GetOwnershipComponent();
	if (RuntimeAttendanceSubsystem == nullptr || OwnershipComponent == nullptr || EventId.IsNone())
	{
		return;
	}

	FCodexInvenAttendanceEventPresentationData PresentationData;
	if (!RuntimeAttendanceSubsystem->GetEventPresentationData(EventId, PresentationData))
	{
		bHasStatusOverrideText = true;
		StatusOverrideText = FText::FromString(FString(ClaimFailedText));
		RefreshFromSubsystem();
		return;
	}

	if (!CanCurrentRewardFitInventory(PresentationData))
	{
		bHasStatusOverrideText = true;
		StatusOverrideText = FText::FromString(FString(InventoryFullText));
		RefreshFromSubsystem();
		return;
	}

	if (!RuntimeAttendanceSubsystem->TryClaimToday(EventId, *OwnershipComponent))
	{
		bHasStatusOverrideText = true;
		StatusOverrideText = FText::FromString(FString(ClaimFailedText));
		RefreshFromSubsystem();
		return;
	}

	bHasStatusOverrideText = false;
	RefreshFromSubsystem();
	CloseAttendanceWidget();
}

void UCodexInvenAttendanceWidgetBase::HandleCloseButtonClicked()
{
	CloseAttendanceWidget();
}
