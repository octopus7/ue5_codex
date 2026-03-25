#include "CodexInvenClockWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

namespace
{
	constexpr float ClockContainerRenderScale = 0.5f;
	const FVector2D ClockContainerPivot(0.0f, 0.5f);
	const FVector2D ClockContainerViewportPosition(108.0f, 0.0f);
}

void UCodexInvenClockWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWidget* const ClockContainerWidget = GetWidgetFromName(TEXT("ClockContainerBorder")))
	{
		ClockContainerWidget->SetRenderTransformPivot(ClockContainerPivot);
		ClockContainerWidget->SetRenderScale(FVector2D(ClockContainerRenderScale, ClockContainerRenderScale));

		if (UCanvasPanelSlot* const ClockContainerSlot = Cast<UCanvasPanelSlot>(ClockContainerWidget->Slot))
		{
			ClockContainerSlot->SetAnchors(FAnchors(0.0f, 0.5f, 0.0f, 0.5f));
			ClockContainerSlot->SetAlignment(ClockContainerPivot);
			ClockContainerSlot->SetPosition(ClockContainerViewportPosition);
			ClockContainerSlot->SetAutoSize(true);
		}
	}

	SetIsFocusable(false);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	LastRenderedUnixSecond = INDEX_NONE;
	RefreshClockVisuals();
}

void UCodexInvenClockWidget::NativeTick(const FGeometry& InGeometry, const float InDeltaTime)
{
	Super::NativeTick(InGeometry, InDeltaTime);

	RefreshClockVisuals();
}

void UCodexInvenClockWidget::RefreshClockVisuals()
{
	const FDateTime LocalTime = FDateTime::Now();
	const double FractionalSecond = static_cast<double>(LocalTime.GetMillisecond()) / 1000.0;
	const double SecondValue = static_cast<double>(LocalTime.GetSecond()) + FractionalSecond;
	const double MinuteValue = static_cast<double>(LocalTime.GetMinute()) + (SecondValue / 60.0);
	const double HourValue = static_cast<double>(LocalTime.GetHour() % 12) + (MinuteValue / 60.0);

	if (HourHand != nullptr)
	{
		HourHand->SetRenderTransformAngle(static_cast<float>(HourValue * 30.0));
	}

	if (MinuteHand != nullptr)
	{
		MinuteHand->SetRenderTransformAngle(static_cast<float>(MinuteValue * 6.0));
	}

	if (SecondHand != nullptr)
	{
		SecondHand->SetRenderTransformAngle(static_cast<float>(SecondValue * 6.0));
	}

	RefreshDigitalClockText(LocalTime);
}

void UCodexInvenClockWidget::RefreshDigitalClockText(const FDateTime& InLocalTime)
{
	const int64 CurrentUnixSecond = InLocalTime.ToUnixTimestamp();
	if (ClockTextBlock == nullptr || LastRenderedUnixSecond == CurrentUnixSecond)
	{
		return;
	}

	LastRenderedUnixSecond = CurrentUnixSecond;
	ClockTextBlock->SetText(FText::FromString(InLocalTime.ToString(TEXT("%Y-%m-%d %H:%M:%S"))));
}
