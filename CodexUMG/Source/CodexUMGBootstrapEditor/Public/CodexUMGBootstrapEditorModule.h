#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "Modules/ModuleInterface.h"

class FCodexUMGBootstrapEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void RunBootstrap();

private:
	void HandlePostEngineInit();
	void RegisterMenus();

	FDelegateHandle PostEngineInitHandle;
};
