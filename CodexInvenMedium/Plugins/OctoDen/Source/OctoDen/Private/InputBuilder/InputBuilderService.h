#pragma once

#include "CoreMinimal.h"
#include "InputBuilder/OctoDenInputBuilderSettings.h"

class UInputAction;
class UInputMappingContext;

namespace OctoDenInputBuilder
{
	void AnalyzeInputContext(const UOctoDenInputBuilderSettings& Settings, FOctoDenInputContextAnalysis& OutAnalysis);
	bool BuildManagedAction(UOctoDenInputBuilderSettings& Settings, FOctoDenManagedActionBuildResult& OutResult, FText& OutFailure);
	bool LinkRuntimeInputConfig(UOctoDenInputBuilderSettings& Settings, FText& OutFailure);
	bool CanLinkRuntimeInputConfig(const UOctoDenInputBuilderSettings& Settings, FText* OutReason = nullptr);
}
