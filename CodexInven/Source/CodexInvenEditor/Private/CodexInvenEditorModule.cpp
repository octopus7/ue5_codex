#include "CodexInvenEditorModule.h"

#include "CodexInvenPickupAssetGenerator.h"
#include "Containers/Ticker.h"
#include "HAL/IConsoleManager.h"
#include "Logging/LogMacros.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Modules/ModuleManager.h"
#include "Misc/CoreMisc.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenEditor, Log, All);

void FCodexInvenEditorModule::StartupModule()
{
	GeneratePickupAssetsCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("GeneratePickupAssets"),
		TEXT("Generate stored pickup materials, meshes, and icon assets under /Game/Art/Pickups."),
		FConsoleCommandDelegate::CreateRaw(this, &FCodexInvenEditorModule::HandleGeneratePickupAssets),
		ECVF_Default);

	const bool bAutoGenerate =
		FParse::Param(FCommandLine::Get(), TEXT("GeneratePickupAssetsOnStartup")) ||
		FParse::Param(FCommandLine::Get(), TEXT("GeneratePickupAssetsAndExit"));
	bExitAfterAutoGenerate = FParse::Param(FCommandLine::Get(), TEXT("GeneratePickupAssetsAndExit"));
	if (bAutoGenerate)
	{
		AutoGenerateTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateRaw(this, &FCodexInvenEditorModule::RunAutoGeneratePickupAssets),
			0.0f);
	}
}

void FCodexInvenEditorModule::ShutdownModule()
{
	if (AutoGenerateTickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(AutoGenerateTickerHandle);
		AutoGenerateTickerHandle.Reset();
	}

	if (GeneratePickupAssetsCommand != nullptr)
	{
		IConsoleManager::Get().UnregisterConsoleObject(GeneratePickupAssetsCommand);
		GeneratePickupAssetsCommand = nullptr;
	}
}

void FCodexInvenEditorModule::HandleGeneratePickupAssets()
{
	FString ResultMessage;
	if (FCodexInvenPickupAssetGenerator::GenerateAssets(ResultMessage))
	{
		UE_LOG(LogCodexInvenEditor, Display, TEXT("%s"), *ResultMessage);
		return;
	}

	UE_LOG(LogCodexInvenEditor, Error, TEXT("%s"), *ResultMessage);
}

bool FCodexInvenEditorModule::RunAutoGeneratePickupAssets(const float InDeltaTime)
{
	static_cast<void>(InDeltaTime);

	AutoGenerateTickerHandle.Reset();
	HandleGeneratePickupAssets();
	if (bExitAfterAutoGenerate)
	{
		RequestEngineExit(TEXT("GeneratePickupAssetsAndExit"));
	}

	return false;
}

IMPLEMENT_MODULE(FCodexInvenEditorModule, CodexInvenEditor)
