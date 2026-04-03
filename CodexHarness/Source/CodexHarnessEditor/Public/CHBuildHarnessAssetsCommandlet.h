#pragma once

#include "Commandlets/Commandlet.h"

#include "CHBuildHarnessAssetsCommandlet.generated.h"

UCLASS()
class CODEXHARNESSEDITOR_API UCHBuildHarnessAssetsCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCHBuildHarnessAssetsCommandlet();

	virtual int32 Main(const FString& Params) override;
};
