#include "Shared/OctoDenNotifications.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"

void OctoDenNotifications::ShowFailureDialog(const FText& Title, const FText& Message)
{
	FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
}

void OctoDenNotifications::ShowSuccessToast(const FText& Message)
{
	FNotificationInfo Info(Message);
	Info.ExpireDuration = 3.5f;
	Info.bFireAndForget = true;
	FSlateNotificationManager::Get().AddNotification(Info);
}
