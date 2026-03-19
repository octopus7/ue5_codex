#pragma once

#include "Modules/ModuleManager.h"

class FCodexInvenMediumEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
