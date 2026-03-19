#include "CodexInvenAttendanceScrollableWidget.h"

#include "Components/ScrollBox.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Templates/SharedPointer.h"

UCodexInvenAttendanceScrollableWidget::UCodexInvenAttendanceScrollableWidget()
{
	ExpectedTotalDays = 14;
	DayEntryWidth = 132.0f;
	DayEntryHeight = 176.0f;
	DayEntrySpacing = 12.0f;
}

void UCodexInvenAttendanceScrollableWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ResetScrollDragState();

	if (UScrollBox* const DayScrollBox = GetDayScrollBox())
	{
		DayScrollBox->SetAllowRightClickDragScrolling(false);
		DayScrollBox->SetConsumeMouseWheel(EConsumeMouseWheel::Never);
	}
}

void UCodexInvenAttendanceScrollableWidget::NativeDestruct()
{
	ResetScrollDragState();
	Super::NativeDestruct();
}

FReply UCodexInvenAttendanceScrollableWidget::NativeOnPreviewMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	}

	UScrollBox* const DayScrollBox = GetDayScrollBox();
	if (DayScrollBox == nullptr || !IsPointerOverDayScrollBox(InMouseEvent.GetScreenSpacePosition()))
	{
		return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	}

	bIsDraggingScroll = true;
	DragStartScreenPosition = InMouseEvent.GetScreenSpacePosition();
	DragStartScrollOffset = DayScrollBox->GetScrollOffset();

	if (const TSharedPtr<SWidget> CachedWidget = GetCachedWidget(); CachedWidget.IsValid())
	{
		return FReply::Handled().CaptureMouse(CachedWidget.ToSharedRef());
	}

	return FReply::Handled();
}

FReply UCodexInvenAttendanceScrollableWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (!bIsDraggingScroll || InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	}

	ResetScrollDragState();
	return FReply::Handled().ReleaseMouseCapture();
}

FReply UCodexInvenAttendanceScrollableWidget::NativeOnMouseMove(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (!bIsDraggingScroll)
	{
		return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
	}

	UScrollBox* const DayScrollBox = GetDayScrollBox();
	if (DayScrollBox == nullptr || !InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		ResetScrollDragState();
		return FReply::Handled().ReleaseMouseCapture();
	}

	const float DragDeltaX = InMouseEvent.GetScreenSpacePosition().X - DragStartScreenPosition.X;
	DayScrollBox->SetScrollOffset(ClampScrollOffset(DragStartScrollOffset - DragDeltaX));
	return FReply::Handled();
}

FReply UCodexInvenAttendanceScrollableWidget::NativeOnMouseWheel(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	UScrollBox* const DayScrollBox = GetDayScrollBox();
	if (DayScrollBox == nullptr || !IsPointerOverDayScrollBox(InMouseEvent.GetScreenSpacePosition()))
	{
		return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
	}

	const float MaxScrollOffset = FMath::Max(0.0f, DayScrollBox->GetScrollOffsetOfEnd());
	if (MaxScrollOffset <= 0.0f)
	{
		return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
	}

	const float NextScrollOffset = ClampScrollOffset(
		DayScrollBox->GetScrollOffset() - (InMouseEvent.GetWheelDelta() * MouseWheelScrollAmount));
	DayScrollBox->SetScrollOffset(NextScrollOffset);
	return FReply::Handled();
}

void UCodexInvenAttendanceScrollableWidget::NativeOnMouseCaptureLost(const FCaptureLostEvent& CaptureLostEvent)
{
	ResetScrollDragState();
	Super::NativeOnMouseCaptureLost(CaptureLostEvent);
}

void UCodexInvenAttendanceScrollableWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	ResetScrollDragState();
	Super::NativeOnFocusLost(InFocusEvent);
}

UScrollBox* UCodexInvenAttendanceScrollableWidget::GetDayScrollBox() const
{
	return Cast<UScrollBox>(DayEntryPanel);
}

bool UCodexInvenAttendanceScrollableWidget::IsPointerOverDayScrollBox(const FVector2D& InScreenSpacePosition) const
{
	const UScrollBox* const DayScrollBox = GetDayScrollBox();
	return DayScrollBox != nullptr && DayScrollBox->GetCachedGeometry().IsUnderLocation(InScreenSpacePosition);
}

float UCodexInvenAttendanceScrollableWidget::ClampScrollOffset(const float InScrollOffset) const
{
	const UScrollBox* const DayScrollBox = GetDayScrollBox();
	const float MaxScrollOffset = DayScrollBox != nullptr
		? FMath::Max(0.0f, DayScrollBox->GetScrollOffsetOfEnd())
		: 0.0f;
	return FMath::Clamp(InScrollOffset, 0.0f, MaxScrollOffset);
}

void UCodexInvenAttendanceScrollableWidget::ResetScrollDragState()
{
	bIsDraggingScroll = false;
	DragStartScreenPosition = FVector2D::ZeroVector;
	DragStartScrollOffset = 0.0f;
}
