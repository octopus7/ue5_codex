#include "PopupWidgetBase.h"

#include "InputCoreTypes.h"

FOnPopupWidgetClosed& UPopupWidgetBase::OnPopupClosed()
{
	return PopupClosed;
}

void UPopupWidgetBase::ClosePopupWithResult(const EPopupWidgetResult InResult)
{
	if (bHasClosed)
	{
		return;
	}

	bHasClosed = true;
	PopupClosed.Broadcast(InResult);
	RemoveFromParent();
}

void UPopupWidgetBase::DismissPopup()
{
	ClosePopupWithResult(EPopupWidgetResult::Dismissed);
}

bool UPopupWidgetBase::IsPopupClosed() const
{
	return bHasClosed;
}

void UPopupWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(true);
}

void UPopupWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	bHasClosed = false;
	SetFocus();
}

FReply UPopupWidgetBase::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bCloseOnBack && IsBackCloseKey(InKeyEvent.GetKey()))
	{
		HandleBackRequested();
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UPopupWidgetBase::HandleBackRequested()
{
	DismissPopup();
}

bool UPopupWidgetBase::IsBackCloseKey(const FKey& InKey)
{
	return InKey == EKeys::Escape
		|| InKey == EKeys::Five
		|| InKey == EKeys::Gamepad_Special_Right
		|| InKey == EKeys::Android_Back;
}
