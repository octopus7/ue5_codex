#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexInvenMediumTopDownSetupCommandlet.generated.h"

UCLASS()
class UCodexInvenMediumTopDownSetupCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexInvenMediumTopDownSetupCommandlet();

	virtual int32 Main(const FString& Params) override;
};
