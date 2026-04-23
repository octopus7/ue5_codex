#pragma once

#include "Commandlets/Commandlet.h"
#include "MannyPoseExportCommandlet.generated.h"

UCLASS()
class UMannyPoseExportCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UMannyPoseExportCommandlet();

    virtual int32 Main(const FString& Params) override;
};
