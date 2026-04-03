#include "CodexHarnessGameInstance.h"

#include "Camera/CameraShakeBase.h"
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

TSubclassOf<UCameraShakeBase> UCodexHarnessGameInstance::GetPlayerHitCameraShakeClass() const
{
	return DefaultEffectsConfig != nullptr ? DefaultEffectsConfig->PlayerHitCameraShakeClass : nullptr;
}

float UCodexHarnessGameInstance::GetPlayerHitCameraShakeScale() const
{
	return DefaultEffectsConfig != nullptr ? DefaultEffectsConfig->PlayerHitCameraShakeScale : 0.0f;
}
