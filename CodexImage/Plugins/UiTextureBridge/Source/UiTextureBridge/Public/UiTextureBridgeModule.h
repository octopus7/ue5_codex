#pragma once

#include "Modules/ModuleManager.h"

class FUiTextureBridgeMenu;
class FUiTextureBridgeServer;

class FUiTextureBridgeModule : public IModuleInterface
{
public:
	static FUiTextureBridgeModule& Get();
	static bool IsAvailable();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void StartServer();
	void StopServer();
	void OpenSettings();
	bool IsServerRunning() const;

private:
	void RegisterSettings();
	void UnregisterSettings();

	TUniquePtr<FUiTextureBridgeServer> Server;
	TUniquePtr<FUiTextureBridgeMenu> Menu;
};
