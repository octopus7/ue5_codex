#include "Transient/DoorPlateArtScaffolding/CodexInvenDoorPlateArtScaffoldCommandlet.h"

#include "Transient/DoorPlateArtScaffolding/CodexInvenDoorPlateAssetGenerator.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenDoorPlateArtScaffoldCommandlet, Log, All);

UCodexInvenDoorPlateArtScaffoldCommandlet::UCodexInvenDoorPlateArtScaffoldCommandlet()
{
	IsServer = false;
	IsClient = false;
	IsEditor = true;
	LogToConsole = true;
	ShowErrorCount = true;
	HelpDescription = TEXT("Create or update door/pressure plate prototype meshes and vertex color materials under /Game/Art/Interactables/DoorPlate.");
}

int32 UCodexInvenDoorPlateArtScaffoldCommandlet::Main(const FString& InParams)
{
	static_cast<void>(InParams);

	FString ResultMessage;
	if (!FCodexInvenDoorPlateAssetGenerator::GenerateAssets(ResultMessage))
	{
		UE_LOG(LogCodexInvenDoorPlateArtScaffoldCommandlet, Error, TEXT("%s"), *ResultMessage);
		return 1;
	}

	UE_LOG(LogCodexInvenDoorPlateArtScaffoldCommandlet, Display, TEXT("%s"), *ResultMessage);
	return 0;
}
