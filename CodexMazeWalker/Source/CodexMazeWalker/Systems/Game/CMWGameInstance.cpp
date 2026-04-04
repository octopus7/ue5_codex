#include "Systems/Game/CMWGameInstance.h"

#include "Engine/World.h"
#include "Logging/LogMacros.h"
#include "Systems/Game/CMWGameDataAsset.h"

void UCMWGameInstance::Init()
{
	Super::Init();

	if (GameDataAsset.IsNull())
	{
		GameDataAsset = FSoftObjectPath(TEXT("/Game/Data/DA_CMWGameData.DA_CMWGameData"));
	}

	if (!GameDataAsset.IsNull())
	{
		LoadedGameData = Cast<UCMWGameDataAsset>(GameDataAsset.TryLoad());
	}

	if (!LoadedGameData)
	{
		UE_LOG(LogTemp, Warning, TEXT("CMWGameInstance failed to load GameData asset '%s'."), *GameDataAsset.ToString());
	}
}

UCMWGameDataAsset* UCMWGameInstance::GetGameData() const
{
	return LoadedGameData;
}

UCMWGameInstance* UCMWGameInstance::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		return Cast<UCMWGameInstance>(World->GetGameInstance());
	}

	return nullptr;
}
