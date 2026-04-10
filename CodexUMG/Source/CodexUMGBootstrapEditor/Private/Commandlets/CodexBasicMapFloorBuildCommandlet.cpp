#include "Commandlets/CodexBasicMapFloorBuildCommandlet.h"

#include "Maps/CodexBasicMapFloorBuilder.h"

UCodexBasicMapFloorBuildCommandlet::UCodexBasicMapFloorBuildCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
	ShowErrorCount = true;
}

int32 UCodexBasicMapFloorBuildCommandlet::Main(const FString& Params)
{
	(void)Params;

	FString ErrorMessage;
	if (!FCodexBasicMapFloorBuilder::RunBuild(ErrorMessage))
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UE_LOG(LogTemp, Display, TEXT("BasicMap floor material build completed successfully."));
	return 0;
}
