#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexTopDownBootstrapCommandlet.generated.h"

UCLASS()
class CODEXUMGBOOTSTRAPEDITOR_API UCodexTopDownBootstrapCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexTopDownBootstrapCommandlet();

	virtual int32 Main(const FString& Params) override;
};
