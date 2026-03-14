#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ProjectBootstrapperHelpSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings)
class UProjectBootstrapperHelpSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config)
	FString HelpLanguageCode;
};
