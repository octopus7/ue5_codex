#pragma once

#include "Commandlets/Commandlet.h"
#include "CMWBootstrapContentCommandlet.generated.h"

UCLASS()
class UCMWBootstrapContentCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCMWBootstrapContentCommandlet();

	virtual int32 Main(const FString& Params) override;
};
