#include "CodexHarnessGameInstance.h"

#include "InputAction.h"
#include "InputMappingContext.h"
#include "NiagaraSystem.h"

const UCodexHarnessInputConfigDataAsset* UCodexHarnessGameInstance::GetInputConfig() const
{
	return DefaultInputConfig;
}

const UCodexHarnessEffectsConfigDataAsset* UCodexHarnessGameInstance::GetEffectsConfig() const
{
	return DefaultEffectsConfig;
}

const UInputMappingContext* UCodexHarnessGameInstance::GetInputMappingContext() const
{
	return DefaultInputConfig != nullptr ? DefaultInputConfig->InputMappingContext : nullptr;
}

const UInputAction* UCodexHarnessGameInstance::GetInputAction(const ECodexHarnessConfiguredInputAction InAction) const
{
	return DefaultInputConfig != nullptr ? DefaultInputConfig->GetInputAction(InAction) : nullptr;
}

const UNiagaraSystem* UCodexHarnessGameInstance::GetPlayerHitReactionSystem() const
{
	return DefaultEffectsConfig != nullptr ? DefaultEffectsConfig->PlayerHitReactionSystem : nullptr;
}
