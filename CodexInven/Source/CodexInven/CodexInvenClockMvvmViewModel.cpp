#include "CodexInvenClockMvvmViewModel.h"

void UCodexInvenClockMvvmViewModel::SetDigitalClockText(const FText& InDigitalClockText)
{
	UE_MVVM_SET_PROPERTY_VALUE(DigitalClockText, InDigitalClockText);
}

void UCodexInvenClockMvvmViewModel::SetHourHandAngle(const float InHourHandAngle)
{
	UE_MVVM_SET_PROPERTY_VALUE(HourHandAngle, InHourHandAngle);
}

void UCodexInvenClockMvvmViewModel::SetMinuteHandAngle(const float InMinuteHandAngle)
{
	UE_MVVM_SET_PROPERTY_VALUE(MinuteHandAngle, InMinuteHandAngle);
}

void UCodexInvenClockMvvmViewModel::SetSecondHandAngle(const float InSecondHandAngle)
{
	UE_MVVM_SET_PROPERTY_VALUE(SecondHandAngle, InSecondHandAngle);
}
