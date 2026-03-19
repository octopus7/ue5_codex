#include "CodexInvenEditorModule.h"

#include "FarmAnimals/CodexInvenFarmAnimalAssetGenerator.h"
#include "CodexInvenPickupAssetGenerator.h"
#include "Containers/Ticker.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMisc.h"
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

	GenerateTestFarmAnimalsCommand = IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("GenerateTestFarmAnimals"),
		TEXT("Generate test-only farm animal meshes under /Game/TestGenerated/FarmAnimals."),
		FConsoleCommandDelegate::CreateRaw(this, &FCodexInvenEditorModule::HandleGenerateTestFarmAnimals),
		ECVF_Default);

	const bool bAutoGeneratePickupAssets =
		FParse::Param(FCommandLine::Get(), TEXT("GeneratePickupAssetsOnStartup")) ||
		FParse::Param(FCommandLine::Get(), TEXT("GeneratePickupAssetsAndExit"));
	const bool bAutoGenerateTestFarmAnimals =
		FParse::Param(FCommandLine::Get(), TEXT("GenerateTestFarmAnimalsOnStartup")) ||
		FParse::Param(FCommandLine::Get(), TEXT("GenerateTestFarmAnimalsAndExit"));

	if (bAutoGeneratePickupAssets)
	{
		PendingAutoGenerateTasks.Add(ECodexInvenEditorAutoGenerateTask::PickupAssets);
	}

	if (bAutoGenerateTestFarmAnimals)
	{
		PendingAutoGenerateTasks.Add(ECodexInvenEditorAutoGenerateTask::TestFarmAnimals);
	}

	bExitAfterAutoGenerate =
		FParse::Param(FCommandLine::Get(), TEXT("GeneratePickupAssetsAndExit")) ||
		FParse::Param(FCommandLine::Get(), TEXT("GenerateTestFarmAnimalsAndExit"));

	if (PendingAutoGenerateTasks.Num() > 0)
	{
		AutoGenerateTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateRaw(this, &FCodexInvenEditorModule::RunAutoGenerateTasks),
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

	if (GenerateTestFarmAnimalsCommand != nullptr)
	{
		IConsoleManager::Get().UnregisterConsoleObject(GenerateTestFarmAnimalsCommand);
		GenerateTestFarmAnimalsCommand = nullptr;
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

void FCodexInvenEditorModule::HandleGenerateTestFarmAnimals()
{
	FString ResultMessage;
	if (FCodexInvenFarmAnimalAssetGenerator::GenerateAssets(ResultMessage))
	{
		UE_LOG(LogCodexInvenEditor, Display, TEXT("%s"), *ResultMessage);
		return;
	}

	UE_LOG(LogCodexInvenEditor, Error, TEXT("%s"), *ResultMessage);
}

bool FCodexInvenEditorModule::ExecuteAutoGenerateTask(const ECodexInvenEditorAutoGenerateTask InTask, FString& OutMessage) const
{
	switch (InTask)
	{
	case ECodexInvenEditorAutoGenerateTask::PickupAssets:
		return FCodexInvenPickupAssetGenerator::GenerateAssets(OutMessage);

	case ECodexInvenEditorAutoGenerateTask::TestFarmAnimals:
		return FCodexInvenFarmAnimalAssetGenerator::GenerateAssets(OutMessage);

	case ECodexInvenEditorAutoGenerateTask::None:
		break;
	}

	OutMessage = TEXT("No auto-generate task was specified.");
	return true;
}

bool FCodexInvenEditorModule::RunAutoGenerateTasks(const float InDeltaTime)
{
	static_cast<void>(InDeltaTime);

	AutoGenerateTickerHandle.Reset();

	bool bSucceeded = true;
	for (const ECodexInvenEditorAutoGenerateTask Task : PendingAutoGenerateTasks)
	{
		FString ResultMessage;
		const bool bTaskSucceeded = ExecuteAutoGenerateTask(Task, ResultMessage);
		bSucceeded &= bTaskSucceeded;

		if (bTaskSucceeded)
		{
			UE_LOG(LogCodexInvenEditor, Display, TEXT("%s"), *ResultMessage);
		}
		else
		{
			UE_LOG(LogCodexInvenEditor, Error, TEXT("%s"), *ResultMessage);
		}
	}

	PendingAutoGenerateTasks.Reset();
	if (bExitAfterAutoGenerate)
	{
		FPlatformMisc::RequestExitWithStatus(false, bSucceeded ? 0 : 1);
	}

	return false;
}

IMPLEMENT_MODULE(FCodexInvenEditorModule, CodexInvenEditor)
