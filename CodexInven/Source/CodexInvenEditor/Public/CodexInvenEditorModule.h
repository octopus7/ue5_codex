#pragma once

#include "Containers/Ticker.h"
#include "Modules/ModuleInterface.h"

class FCodexInvenEditorModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void HandleGeneratePickupAssets();
	bool RunAutoGeneratePickupAssets(float InDeltaTime);

	class IConsoleObject* GeneratePickupAssetsCommand = nullptr;
	bool bExitAfterAutoGenerate = false;
	FTSTicker::FDelegateHandle AutoGenerateTickerHandle;
};
