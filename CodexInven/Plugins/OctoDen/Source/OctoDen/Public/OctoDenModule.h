#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "UObject/StrongObjectPtr.h"

class IDetailsView;
class SDockTab;
class STextBlock;
class UOctoDenBootstrapperSettings;
class UOctoDenInputBuilderSettings;

class FOctoDenModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static bool ShouldShowBootstrapperCodeGenerationUI(const UOctoDenBootstrapperSettings* InSettings);

	bool CreateManagedMap(UOctoDenBootstrapperSettings* InSettings);
	bool OpenManagedMap(UOctoDenBootstrapperSettings* InSettings);
	bool GenerateNativeCode(UOctoDenBootstrapperSettings* InSettings);
	bool CreateBlueprintsAndApply(UOctoDenBootstrapperSettings* InSettings);
	bool BuildInputAssets(UOctoDenInputBuilderSettings* InSettings);
	bool CreateDefaultInputMappingContext(UOctoDenInputBuilderSettings* InSettings);
	bool LinkInputConfigDataAsset(UOctoDenInputBuilderSettings* InSettings);

private:
	void RegisterMenus();
	void RegisterTabSpawners();
	void OpenBootstrapperTab();
	void OpenInputBuilderTab();
	void RefreshBootstrapperDetails();
	void RefreshInputBuilderDetails();
	void UpdateInputBuilderResult(const FText& InResultText);
	void ResetInputBuilderSettings();
	void ShowNotification(const FText& InText, bool bWasSuccessful) const;
	TSharedRef<SDockTab> SpawnBootstrapperTab(const class FSpawnTabArgs& SpawnTabArgs);
	TSharedRef<SDockTab> SpawnInputBuilderTab(const class FSpawnTabArgs& SpawnTabArgs);
	void HandleBootstrapperTabClosed(TSharedRef<SDockTab> InTab);
	void HandleInputBuilderTabClosed(TSharedRef<SDockTab> InTab);

	TSharedPtr<SDockTab> BootstrapperTab;
	TSharedPtr<IDetailsView> BootstrapperDetailsView;
	TStrongObjectPtr<UOctoDenBootstrapperSettings> BootstrapperSettings;
	TSharedPtr<SDockTab> InputBuilderTab;
	TSharedPtr<IDetailsView> InputBuilderDetailsView;
	TStrongObjectPtr<UOctoDenInputBuilderSettings> InputBuilderSettings;
	TSharedPtr<STextBlock> InputBuilderResultTextBlock;
	FDelegateHandle ObjectPropertyChangedHandle;
	FDelegateHandle ReloadCompleteHandle;
	bool bCanCreateBlueprintsAndApply = false;
};
