#include "CodexHarnessGameInstance.h"

#include "InputAction.h"
#include "InputMappingContext.h"

const UCodexHarnessInputConfigDataAsset* UCodexHarnessGameInstance::GetInputConfig() const
{
	return DefaultInputConfig;
}

const UInputMappingContext* UCodexHarnessGameInstance::GetInputMappingContext() const
{
	return DefaultInputConfig != nullptr ? DefaultInputConfig->InputMappingContext : nullptr;
}

const UInputAction* UCodexHarnessGameInstance::GetInputAction(const ECodexHarnessConfiguredInputAction InAction) const
{
	return DefaultInputConfig != nullptr ? DefaultInputConfig->GetInputAction(InAction) : nullptr;
}
