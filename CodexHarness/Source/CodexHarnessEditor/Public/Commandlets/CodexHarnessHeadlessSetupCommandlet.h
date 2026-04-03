#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexHarnessHeadlessSetupCommandlet.generated.h"

UCLASS()
class CODEXHARNESSEDITOR_API UCodexHarnessHeadlessSetupCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexHarnessHeadlessSetupCommandlet();

	virtual int32 Main(const FString& Params) override;
};
