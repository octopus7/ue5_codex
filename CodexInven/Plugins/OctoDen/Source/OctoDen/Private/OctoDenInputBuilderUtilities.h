#pragma once

#include "CoreMinimal.h"
#include "OctoDenInputBuilderSettings.h"

class UInputAction;
class UInputMappingContext;

namespace OctoDenInputBuilder
{
	struct FApplyManagedActionMappingsResult
	{
		int32 RemovedNullMappings = 0;
		int32 RemovedExistingMappings = 0;
		int32 AddedMappings = 0;
	};

	void ConfigureManagedInputAction(UInputAction& InInputAction, EOctoDenStandardInputAction InManagedAction);

	bool ApplyManagedActionMappings(
		UInputMappingContext& InInputMappingContext,
		UInputAction& InInputAction,
		EOctoDenStandardInputAction InManagedAction,
		const FOctoDenInputBindingDraft& InBindingDraft,
		FApplyManagedActionMappingsResult& OutResult,
		FText& OutFailReason);
}
