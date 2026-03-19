#pragma once

#include "CoreMinimal.h"
#include "Framework/Docking/TabManager.h"
#include "Input/Reply.h"
#include "Modules/ModuleManager.h"

class IDetailsView;
class SDockTab;
class SWidget;
class UOctoDenBootstrapperSettings;
class UOctoDenInputBuilderSettings;

class FOctoDenModule : public IModuleInterface
{
public:
	static const FName BootstrapperTabId;
	static const FName InputBuilderTabId;

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus();
	void RegisterTabs();
	void UnregisterTabs();
	void ResetBootstrapper();
	void ResetInputBuilder();
	void RefreshBootstrapperReadiness();

	TSharedRef<SDockTab> SpawnBootstrapperTab(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnInputBuilderTab(const FSpawnTabArgs& Args);
	TSharedRef<SWidget> BuildBootstrapperContent();
	TSharedRef<SWidget> BuildInputBuilderContent();

	FReply HandleCreateManagedMap();
	FReply HandleOpenManagedMap();
	FReply HandleGenerateCode();
	FReply HandleCreateBlueprintsAndApply();
	FReply HandleBuildManagedAction();
	FReply HandleLinkRuntimeInputConfig();

	TStrongObjectPtr<UOctoDenBootstrapperSettings> BootstrapperSettings;
	TStrongObjectPtr<UOctoDenInputBuilderSettings> InputBuilderSettings;
	TSharedPtr<IDetailsView> BootstrapperDetailsView;
	TSharedPtr<IDetailsView> InputBuilderDetailsView;
	bool bCanApplyBlueprints = false;
};
