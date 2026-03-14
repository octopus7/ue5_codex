#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "UObject/StrongObjectPtr.h"

class SWindow;
class IDetailsView;
class FSlateStyleSet;
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

private:
	void RegisterStyle();
	void UnregisterStyle();
	void RegisterMenus();
	void OpenBootstrapperWindow();
	void HandleDialogWindowClosed(const TSharedRef<SWindow>& Window);

	TSharedPtr<FSlateStyleSet> StyleSet;
	TSharedPtr<SWindow> DialogWindow;
	TSharedPtr<IDetailsView> DialogDetailsView;
	TStrongObjectPtr<UProjectBootstrapperDialogSettings> DialogSettings;
};
