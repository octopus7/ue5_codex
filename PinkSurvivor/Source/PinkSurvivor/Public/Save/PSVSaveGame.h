#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PSVSaveGame.generated.h"

/**
 * Basic save-game container for long-term player progression data.
 * Currently stores the total amount of gold coins collected across runs.
 */
UCLASS()
class PINKSURVIVOR_API UPSVSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPSVSaveGame();

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Progression")
    int32 PersistentGold;
};
