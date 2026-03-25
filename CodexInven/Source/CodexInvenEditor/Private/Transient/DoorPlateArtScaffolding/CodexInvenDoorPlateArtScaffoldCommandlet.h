#pragma once

#include "Commandlets/Commandlet.h"
#include "CodexInvenDoorPlateArtScaffoldCommandlet.generated.h"

UCLASS()
class UCodexInvenDoorPlateArtScaffoldCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UCodexInvenDoorPlateArtScaffoldCommandlet();

	virtual int32 Main(const FString& InParams) override;
};
