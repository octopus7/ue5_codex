#include "CodexInvenClockMvvmWidget.h"

#include "CodexInvenClockMvvmViewModel.h"
#include "INotifyFieldValueChanged.h"
#include "MVVMSubsystem.h"
#include "View/MVVMView.h"

namespace
{
	const FName ClockViewModelName(TEXT("ClockViewModel"));
}

void UCodexInvenClockMvvmWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(false);
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	LastRenderedUnixSecond = INDEX_NONE;
	RuntimeClockViewModel = ResolveClockViewModel();
	RefreshClockViewModel();
}

void UCodexInvenClockMvvmWidget::NativeTick(const FGeometry& InGeometry, const float InDeltaTime)
{
	Super::NativeTick(InGeometry, InDeltaTime);

	RefreshClockViewModel();
}

UCodexInvenClockMvvmViewModel* UCodexInvenClockMvvmWidget::ResolveClockViewModel()
{
	if (RuntimeClockViewModel != nullptr)
	{
		return RuntimeClockViewModel;
	}

	UMVVMView* const View = UMVVMSubsystem::GetViewFromUserWidget(this);
	if (View == nullptr)
	{
		return nullptr;
	}

	const TScriptInterface<INotifyFieldValueChanged> ExistingViewModel = View->GetViewModel(ClockViewModelName);
	if (UCodexInvenClockMvvmViewModel* const ExistingClockViewModel = Cast<UCodexInvenClockMvvmViewModel>(ExistingViewModel.GetObject()))
	{
		RuntimeClockViewModel = ExistingClockViewModel;
		return RuntimeClockViewModel;
	}

	UCodexInvenClockMvvmViewModel* const NewClockViewModel = NewObject<UCodexInvenClockMvvmViewModel>(this);
	TScriptInterface<INotifyFieldValueChanged> ClockViewModelInterface;
	ClockViewModelInterface.SetObject(NewClockViewModel);
	ClockViewModelInterface.SetInterface(Cast<INotifyFieldValueChanged>(NewClockViewModel));

	if (View->SetViewModel(ClockViewModelName, ClockViewModelInterface))
	{
		RuntimeClockViewModel = NewClockViewModel;
	}

	return RuntimeClockViewModel;
}

void UCodexInvenClockMvvmWidget::RefreshClockViewModel()
{
	UCodexInvenClockMvvmViewModel* const ClockViewModel = ResolveClockViewModel();
	if (ClockViewModel == nullptr)
	{
		return;
	}

	const FDateTime LocalTime = FDateTime::Now();
	const double FractionalSecond = static_cast<double>(LocalTime.GetMillisecond()) / 1000.0;
	const double SecondValue = static_cast<double>(LocalTime.GetSecond()) + FractionalSecond;
	const double MinuteValue = static_cast<double>(LocalTime.GetMinute()) + (SecondValue / 60.0);
	const double HourValue = static_cast<double>(LocalTime.GetHour() % 12) + (MinuteValue / 60.0);

	ClockViewModel->SetHourHandAngle(static_cast<float>(HourValue * 30.0));
	ClockViewModel->SetMinuteHandAngle(static_cast<float>(MinuteValue * 6.0));
	ClockViewModel->SetSecondHandAngle(static_cast<float>(SecondValue * 6.0));

	const int64 CurrentUnixSecond = LocalTime.ToUnixTimestamp();
	if (LastRenderedUnixSecond == CurrentUnixSecond)
	{
		return;
	}

	LastRenderedUnixSecond = CurrentUnixSecond;
	ClockViewModel->SetDigitalClockText(FText::FromString(LocalTime.ToString(TEXT("%Y-%m-%d %H:%M:%S"))));
}
