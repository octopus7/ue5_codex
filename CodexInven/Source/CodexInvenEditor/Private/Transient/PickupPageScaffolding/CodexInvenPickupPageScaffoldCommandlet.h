#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexInvenPickupPageScaffoldCommandlet.generated.h"

UCLASS()
class UCodexInvenPickupPageScaffoldCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexInvenPickupPageScaffoldCommandlet();

	virtual int32 Main(const FString& InParams) override;
};
