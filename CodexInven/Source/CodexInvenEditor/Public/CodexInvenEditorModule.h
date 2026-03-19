#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Containers/Ticker.h"
#include "Modules/ModuleInterface.h"

enum class ECodexInvenEditorAutoGenerateTask : uint8
{
	None,
	PickupAssets,
	TestFarmAnimals
};

class FCodexInvenEditorModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void HandleGeneratePickupAssets();
	void HandleGenerateTestFarmAnimals();
	bool ExecuteAutoGenerateTask(ECodexInvenEditorAutoGenerateTask InTask, FString& OutMessage) const;
	bool RunAutoGenerateTasks(float InDeltaTime);

	class IConsoleObject* GeneratePickupAssetsCommand = nullptr;
	class IConsoleObject* GenerateTestFarmAnimalsCommand = nullptr;
	bool bExitAfterAutoGenerate = false;
	TArray<ECodexInvenEditorAutoGenerateTask> PendingAutoGenerateTasks;
	FTSTicker::FDelegateHandle AutoGenerateTickerHandle;
};
