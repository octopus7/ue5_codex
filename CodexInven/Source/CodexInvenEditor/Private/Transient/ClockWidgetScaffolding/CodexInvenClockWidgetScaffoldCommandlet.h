#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexInvenClockWidgetScaffoldCommandlet.generated.h"

UCLASS()
class UCodexInvenClockWidgetScaffoldCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexInvenClockWidgetScaffoldCommandlet();

	virtual int32 Main(const FString& InParams) override;
};
