#include "Systems/Game/CMWGameInstance.h"

#include "Engine/World.h"
#include "Systems/Game/CMWGameDataAsset.h"

void UCMWGameInstance::Init()
{
	Super::Init();

	if (!GameDataAsset.IsNull())
	{
		LoadedGameData = Cast<UCMWGameDataAsset>(GameDataAsset.TryLoad());
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
