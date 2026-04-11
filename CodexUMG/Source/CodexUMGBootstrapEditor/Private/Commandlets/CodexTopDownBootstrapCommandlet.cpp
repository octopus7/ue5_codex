#include "Commandlets/CodexTopDownBootstrapCommandlet.h"

#include "CodexUMGBootstrapEditorModule.h"
#include "Modules/ModuleManager.h"

UCodexTopDownBootstrapCommandlet::UCodexTopDownBootstrapCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
	ShowErrorCount = true;
}

int32 UCodexTopDownBootstrapCommandlet::Main(const FString& Params)
{
	(void)Params;

	FCodexUMGBootstrapEditorModule& BootstrapModule =
		FModuleManager::LoadModuleChecked<FCodexUMGBootstrapEditorModule>("CodexUMGBootstrapEditor");
	BootstrapModule.RunBootstrap();

	UE_LOG(LogTemp, Display, TEXT("Top-down bootstrap asset build completed successfully."));
	return 0;
}
