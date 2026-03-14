#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "UObject/StrongObjectPtr.h"

class SWindow;
class IDetailsView;
class FSlateStyleSet;
class STextBlock;
class UProjectBootstrapperDialogSettings;

class FProjectBootstrapperModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool CreateManagedMap(UProjectBootstrapperDialogSettings* InDialogSettings);
	bool OpenManagedMap(UProjectBootstrapperDialogSettings* InDialogSettings);
	bool GenerateNativeCode(UProjectBootstrapperDialogSettings* InDialogSettings);
	bool CreateBlueprintsAndApply(UProjectBootstrapperDialogSettings* InDialogSettings);
	void RefreshDialogDetails() const;
	void OpenHelpWindow();

private:
	void RegisterStyle();
	void UnregisterStyle();
	void RegisterMenus();
	void OpenBootstrapperWindow();
	void HandleDialogWindowClosed(const TSharedRef<SWindow>& Window);
	void HandleHelpWindowClosed(const TSharedRef<SWindow>& Window);
	void EnsureHelpLanguageOptions();
	FString GetSelectedHelpLanguageCode();
	FString DetectDefaultHelpLanguageCode() const;
	void SaveHelpLanguageCode(const FString& InLanguageCode);
	FText GetHelpLanguageDisplayText(const FString& InLanguageCode) const;
	FString LoadHelpTextForLanguage(const FString& InLanguageCode) const;
	void RefreshHelpWindowContent();

	TSharedPtr<FSlateStyleSet> StyleSet;
	TSharedPtr<SWindow> DialogWindow;
	TSharedPtr<IDetailsView> DialogDetailsView;
	TStrongObjectPtr<UProjectBootstrapperDialogSettings> DialogSettings;
	TSharedPtr<SWindow> HelpWindow;
	TSharedPtr<STextBlock> HelpTextBlock;
	TArray<TSharedPtr<FString>> HelpLanguageOptions;
};
