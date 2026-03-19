#pragma once

#include "Commandlets/Commandlet.h"

#include "CodexInvenClockMvvmWidgetScaffoldCommandlet.generated.h"

UCLASS()
class UCodexInvenClockMvvmWidgetScaffoldCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexInvenClockMvvmWidgetScaffoldCommandlet();

	virtual int32 Main(const FString& InParams) override;
};
