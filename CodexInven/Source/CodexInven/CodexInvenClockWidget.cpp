#include "CodexInvenClockWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"

void UCodexInvenClockWidget::NativeConstruct()
{
	Super::NativeConstruct();

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
