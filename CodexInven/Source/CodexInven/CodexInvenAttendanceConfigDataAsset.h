#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "CodexInvenAttendanceTypes.h"
#include "CodexInvenAttendanceConfigDataAsset.generated.h"

UCLASS(BlueprintType)
class CODEXINVEN_API UCodexInvenAttendanceConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance", meta = (ClampMin = "0.0"))
	float PopupInitialDelaySeconds = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attendance")
	TArray<FCodexInvenAttendanceEventDefinition> ActiveEvents;
};
