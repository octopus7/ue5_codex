#pragma once

#include "CoreMinimal.h"
#include "InputBuilder/OctoDenInputBuilderSettings.h"

class UInputAction;
class UInputMappingContext;

namespace OctoDenInputMappingRules
{
	void ConfigureInputAction(UInputAction& InputAction, EOctoDenManagedInputAction ManagedAction);
	bool ApplyMappings(UInputMappingContext& InputMappingContext, UInputAction& InputAction, EOctoDenManagedInputAction ManagedAction, const FOctoDenKeyDraft& Draft, FOctoDenManagedActionBuildResult& OutResult, FText& OutFailure);
}
