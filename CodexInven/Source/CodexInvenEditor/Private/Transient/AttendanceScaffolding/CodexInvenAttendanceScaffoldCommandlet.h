#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexInvenAttendanceScaffoldCommandlet.generated.h"

UCLASS()
class UCodexInvenAttendanceScaffoldCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexInvenAttendanceScaffoldCommandlet();

	virtual int32 Main(const FString& InParams) override;
};
