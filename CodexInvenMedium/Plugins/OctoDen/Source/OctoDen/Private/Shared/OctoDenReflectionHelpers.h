#pragma once

#include "CoreMinimal.h"

class FObjectProperty;

namespace OctoDenReflectionHelpers
{
	FObjectProperty* FindRequiredObjectProperty(UObject& TargetObject, const TCHAR* PropertyName, UClass* ExpectedBaseClass, FText& OutFailure);
	bool SetObjectProperty(UObject& TargetObject, const TCHAR* PropertyName, UObject* Value, UClass* ExpectedBaseClass, FText& OutFailure);
}
