#pragma once

#include "CoreMinimal.h"

class FUiTextureBridgeMenu
{
public:
	FUiTextureBridgeMenu(FSimpleDelegate InStartServer, FSimpleDelegate InStopServer, FSimpleDelegate InOpenSettings);

	void Register();
	void Unregister();

private:
	void RegisterMenus();

	FSimpleDelegate StartServerDelegate;
	FSimpleDelegate StopServerDelegate;
	FSimpleDelegate OpenSettingsDelegate;
};
