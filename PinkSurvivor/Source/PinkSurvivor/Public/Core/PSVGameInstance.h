#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PSVGameInstance.generated.h"

class UPSVSaveGame;

/**
 * Game instance responsible for loading and storing persistent progression data.
 * Gold balance survives across runs through the bound SaveGame object.
 */
UCLASS()
class PINKSURVIVOR_API UPSVGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPSVGameInstance();

    virtual void Init() override;

    UFUNCTION(BlueprintCallable, Category="Progression")
    int32 GetPersistentGold() const;

    UFUNCTION(BlueprintCallable, Category="Progression")
    void AddPersistentGold(int32 Amount);

    UFUNCTION(BlueprintCallable, Category="Progression")
    void SavePersistentData();

protected:
    void LoadOrCreateSaveGame();
    void InternalSaveGame();

    UPROPERTY()
    TObjectPtr<UPSVSaveGame> ActiveSave;

    UPROPERTY(EditDefaultsOnly, Category="Progression")
    FName SaveSlotName;

    UPROPERTY(EditDefaultsOnly, Category="Progression")
    int32 SaveUserIndex;
};
