#include "UI/UiTextureBridgeMenu.h"

#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "UiTextureBridgeMenu"

FUiTextureBridgeMenu::FUiTextureBridgeMenu(
	FSimpleDelegate InStartServer,
	FSimpleDelegate InStopServer,
	FSimpleDelegate InOpenSettings)
	: StartServerDelegate(MoveTemp(InStartServer))
	, StopServerDelegate(MoveTemp(InStopServer))
	, OpenSettingsDelegate(MoveTemp(InOpenSettings))
{
}

void FUiTextureBridgeMenu::Register()
{
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUiTextureBridgeMenu::RegisterMenus));
}

void FUiTextureBridgeMenu::Unregister()
{
	if (UToolMenus::TryGet() != nullptr)
	{
		UToolMenus::UnRegisterStartupCallback(this);
		UToolMenus::UnregisterOwner(this);
	}
}

void FUiTextureBridgeMenu::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* const ToolsMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Tools"));
	FToolMenuSection& Section = ToolsMenu->FindOrAddSection(TEXT("UiTextureBridge"));
	Section.AddSubMenu(
		TEXT("UiTextureBridgeRoot"),
		LOCTEXT("RootLabel", "UI Texture Bridge"),
		LOCTEXT("RootTooltip", "Control the local UI texture import bridge."),
		FNewToolMenuDelegate::CreateLambda([this](UToolMenu* SubMenu)
		{
			FToolMenuSection& SubSection = SubMenu->AddSection(TEXT("UiTextureBridgeActions"));
			SubSection.AddMenuEntry(
				TEXT("UiTextureBridgeStartServer"),
				LOCTEXT("StartServerLabel", "Start Server"),
				LOCTEXT("StartServerTooltip", "Start the local HTTP server for importing UI textures."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([this]()
				{
					StartServerDelegate.ExecuteIfBound();
				})));
			SubSection.AddMenuEntry(
				TEXT("UiTextureBridgeStopServer"),
				LOCTEXT("StopServerLabel", "Stop Server"),
				LOCTEXT("StopServerTooltip", "Stop the local HTTP server for importing UI textures."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([this]()
				{
					StopServerDelegate.ExecuteIfBound();
				})));
			SubSection.AddMenuEntry(
				TEXT("UiTextureBridgeOpenSettings"),
				LOCTEXT("OpenSettingsLabel", "Open Settings"),
				LOCTEXT("OpenSettingsTooltip", "Open the project settings for UI Texture Bridge."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([this]()
				{
					OpenSettingsDelegate.ExecuteIfBound();
				})));
		}));
}

#undef LOCTEXT_NAMESPACE
