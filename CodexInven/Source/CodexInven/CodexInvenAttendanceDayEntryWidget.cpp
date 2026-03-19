#include "CodexInvenAttendanceDayEntryWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Styling/SlateColor.h"

namespace
{
	const TCHAR* ClaimedText = TEXT("\uC218\uB839 \uC644\uB8CC");
	const TCHAR* ClaimableText = TEXT("\uC624\uB298 \uC218\uB839");
	const TCHAR* LockedText = TEXT("\uC7A0\uAE08");

	FLinearColor GetEntryBackgroundColor(const ECodexInvenAttendanceDayState InState)
	{
		switch (InState)
		{
		case ECodexInvenAttendanceDayState::Claimed:
			return FLinearColor(0.10f, 0.26f, 0.22f, 0.96f);

		case ECodexInvenAttendanceDayState::Claimable:
			return FLinearColor(0.48f, 0.36f, 0.10f, 0.98f);

		case ECodexInvenAttendanceDayState::Locked:
		default:
			return FLinearColor(0.08f, 0.10f, 0.13f, 0.94f);
		}
	}

	FLinearColor GetTitleColor(const ECodexInvenAttendanceDayState InState)
	{
		switch (InState)
		{
		case ECodexInvenAttendanceDayState::Claimed:
			return FLinearColor(0.30f, 0.84f, 0.66f, 1.0f);

		case ECodexInvenAttendanceDayState::Claimable:
			return FLinearColor(0.96f, 0.82f, 0.36f, 1.0f);

		case ECodexInvenAttendanceDayState::Locked:
		default:
			return FLinearColor(0.62f, 0.68f, 0.76f, 1.0f);
		}
	}

	FText GetStateText(const ECodexInvenAttendanceDayState InState)
	{
		switch (InState)
		{
		case ECodexInvenAttendanceDayState::Claimed:
			return FText::FromString(FString(ClaimedText));

		case ECodexInvenAttendanceDayState::Claimable:
			return FText::FromString(FString(ClaimableText));

		case ECodexInvenAttendanceDayState::Locked:
		default:
			return FText::FromString(FString(LockedText));
		}
	}
}

void UCodexInvenAttendanceDayEntryWidget::SetPresentationData(const FCodexInvenAttendanceDayPresentationData& InPresentationData)
{
	PresentationData = InPresentationData;
	bHasPresentationData = true;
	BuildWidgetTreeIfNeeded();
	RefreshVisualState();
}

void UCodexInvenAttendanceDayEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(false);
	BuildWidgetTreeIfNeeded();
	RefreshVisualState();
}

void UCodexInvenAttendanceDayEntryWidget::BuildWidgetTreeIfNeeded()
{
	if (WidgetTree == nullptr)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("AttendanceDayEntryWidgetTree"));
	}

	if (RootBorder != nullptr)
	{
		return;
	}

	RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("RootBorder"));
	RootBorder->SetPadding(FMargin(12.0f));
	WidgetTree->RootWidget = RootBorder;

	UVerticalBox* const ContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentBox"));
	RootBorder->SetContent(ContentBox);

	DayNumberTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DayNumberTextBlock"));
	FSlateFontInfo DayNumberFont = DayNumberTextBlock->GetFont();
	DayNumberFont.Size = 18;
	DayNumberTextBlock->SetFont(DayNumberFont);
	if (UVerticalBoxSlot* const DaySlot = ContentBox->AddChildToVerticalBox(DayNumberTextBlock))
	{
		DaySlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
	}

	RewardTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RewardTextBlock"));
	RewardTextBlock->SetAutoWrapText(true);
	RewardTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.94f, 0.96f, 1.0f, 1.0f)));
	if (UVerticalBoxSlot* const RewardSlot = ContentBox->AddChildToVerticalBox(RewardTextBlock))
	{
		RewardSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	StateTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StateTextBlock"));
	FSlateFontInfo StateFont = StateTextBlock->GetFont();
	StateFont.Size = 12;
	StateTextBlock->SetFont(StateFont);
	StateTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.90f, 0.92f, 0.98f, 0.92f)));
	if (UVerticalBoxSlot* const StateSlot = ContentBox->AddChildToVerticalBox(StateTextBlock))
	{
		StateSlot->SetPadding(FMargin(0.0f, 10.0f, 0.0f, 0.0f));
	}
}

void UCodexInvenAttendanceDayEntryWidget::RefreshVisualState()
{
	if (!bHasPresentationData || RootBorder == nullptr || DayNumberTextBlock == nullptr || RewardTextBlock == nullptr || StateTextBlock == nullptr)
	{
		return;
	}

	RootBorder->SetBrushColor(GetEntryBackgroundColor(PresentationData.State));
	DayNumberTextBlock->SetText(FText::Format(
		FText::FromString(TEXT("{0}\uC77C\uCC28")),
		FText::AsNumber(PresentationData.DayNumber)));
	DayNumberTextBlock->SetColorAndOpacity(FSlateColor(GetTitleColor(PresentationData.State)));
	RewardTextBlock->SetText(PresentationData.RewardText);
	StateTextBlock->SetText(GetStateText(PresentationData.State));
}
