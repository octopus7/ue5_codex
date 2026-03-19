#pragma once

#include "CoreMinimal.h"

namespace OctoDenNotifications
{
	void ShowFailureDialog(const FText& Title, const FText& Message);
	void ShowSuccessToast(const FText& Message);
}
