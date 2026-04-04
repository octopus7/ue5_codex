#pragma once

#include "Commandlets/Commandlet.h"
#include "CMWImportVoxCommandlet.generated.h"

UCLASS()
class UCMWImportVoxCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCMWImportVoxCommandlet();

	virtual int32 Main(const FString& Params) override;
};
