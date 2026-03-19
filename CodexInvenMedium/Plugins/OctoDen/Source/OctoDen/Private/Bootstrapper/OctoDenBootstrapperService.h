#pragma once

#include "CoreMinimal.h"

class UBlueprint;
class UClass;
class UOctoDenBootstrapperSettings;

namespace OctoDenBootstrapper
{
	void PopulateSuggestedDefaults(UOctoDenBootstrapperSettings& Settings);
	bool CreateManagedMap(UOctoDenBootstrapperSettings& Settings, FText& OutFailure);
	bool OpenManagedMap(UOctoDenBootstrapperSettings& Settings, FText& OutFailure);
	bool GenerateNativeClasses(UOctoDenBootstrapperSettings& Settings, FText& OutFailure);
	bool AreNativeClassesReady(const UOctoDenBootstrapperSettings& Settings);
	bool CreateBlueprintsAndApply(UOctoDenBootstrapperSettings& Settings, FText& OutFailure);
}
