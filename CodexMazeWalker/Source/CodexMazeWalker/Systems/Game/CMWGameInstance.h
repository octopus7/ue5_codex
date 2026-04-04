#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CMWGameInstance.generated.h"

class UCMWGameDataAsset;

UCLASS(Config = Game)
class CODEXMAZEWALKER_API UCMWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION(BlueprintPure, Category = "CodexMazeWalker|GameData")
	UCMWGameDataAsset* GetGameData() const;

	UFUNCTION(BlueprintPure, Category = "CodexMazeWalker|GameData")
	static UCMWGameInstance* Get(const UObject* WorldContextObject);

protected:
	UPROPERTY(Config, EditDefaultsOnly, Category = "Bootstrap")
	FSoftObjectPath GameDataAsset;

	UPROPERTY(Transient)
	TObjectPtr<UCMWGameDataAsset> LoadedGameData;
};
