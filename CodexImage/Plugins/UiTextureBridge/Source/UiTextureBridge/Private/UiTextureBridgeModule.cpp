#include "UiTextureBridgeModule.h"

#include "Framework/Notifications/NotificationManager.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "Server/UiTextureBridgeServer.h"
#include "Settings/UiTextureBridgeSettings.h"
#include "UI/UiTextureBridgeMenu.h"
#include "Widgets/Notifications/SNotificationList.h"

DEFINE_LOG_CATEGORY_STATIC(LogUiTextureBridgeModule, Log, All);

IMPLEMENT_MODULE(FUiTextureBridgeModule, UiTextureBridge);

namespace
{
	const FName SettingsContainerName(TEXT("Project"));
	const FName SettingsCategoryName(TEXT("Plugins"));
	const FName SettingsSectionName(TEXT("UiTextureBridge"));

	void ShowNotification(const FText& InTitle, const FText& InSubText, SNotificationItem::ECompletionState InState)
	{
		FNotificationInfo Info(InTitle);
		Info.SubText = InSubText;
		Info.ExpireDuration = 4.0f;
		Info.FadeOutDuration = 0.2f;
		Info.bFireAndForget = true;

		const TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
		if (Notification.IsValid())
		{
			Notification->SetCompletionState(InState);
		}
	}
}

FUiTextureBridgeModule& FUiTextureBridgeModule::Get()
{
	return FModuleManager::LoadModuleChecked<FUiTextureBridgeModule>(TEXT("UiTextureBridge"));
}

bool FUiTextureBridgeModule::IsAvailable()
{
	return FModuleManager::Get().IsModuleLoaded(TEXT("UiTextureBridge"));
}

void FUiTextureBridgeModule::StartupModule()
{
	if (IsRunningCommandlet())
	{
		return;
	}

	Server = MakeUnique<FUiTextureBridgeServer>();
	Menu = MakeUnique<FUiTextureBridgeMenu>(
		FSimpleDelegate::CreateRaw(this, &FUiTextureBridgeModule::StartServer),
		FSimpleDelegate::CreateRaw(this, &FUiTextureBridgeModule::StopServer),
		FSimpleDelegate::CreateRaw(this, &FUiTextureBridgeModule::OpenSettings));

	RegisterSettings();
	Menu->Register();

	if (GetDefault<UUiTextureBridgeSettings>()->bAutoStart)
	{
		StartServer();
	}
}

void FUiTextureBridgeModule::ShutdownModule()
{
	if (IsRunningCommandlet())
	{
		return;
	}

	if (Menu.IsValid())
	{
		Menu->Unregister();
		Menu.Reset();
	}

	if (Server.IsValid() && Server->IsRunning())
	{
		Server->Stop();
	}
	Server.Reset();
	UnregisterSettings();
}

void FUiTextureBridgeModule::StartServer()
{
	if (!Server.IsValid())
	{
		return;
	}

	if (Server->IsRunning())
	{
		ShowNotification(
			NSLOCTEXT("UiTextureBridgeModule", "AlreadyRunningTitle", "UI Texture Bridge already running"),
			FText::FromString(Server->GetBaseUrl()),
			SNotificationItem::CS_Pending);
		return;
	}

	FString Error;
	if (Server->Start(Error))
	{
		ShowNotification(
			NSLOCTEXT("UiTextureBridgeModule", "StartedTitle", "UI Texture Bridge started"),
			FText::FromString(Server->GetBaseUrl()),
			SNotificationItem::CS_Success);
		UE_LOG(LogUiTextureBridgeModule, Display, TEXT("UI Texture Bridge started at %s"), *Server->GetBaseUrl());
	}
	else
	{
		ShowNotification(
			NSLOCTEXT("UiTextureBridgeModule", "StartFailedTitle", "UI Texture Bridge failed to start"),
			FText::FromString(Error),
			SNotificationItem::CS_Fail);
		UE_LOG(LogUiTextureBridgeModule, Error, TEXT("UI Texture Bridge failed to start: %s"), *Error);
	}
}

void FUiTextureBridgeModule::StopServer()
{
	if (!Server.IsValid())
	{
		return;
	}

	if (!Server->IsRunning())
	{
		ShowNotification(
			NSLOCTEXT("UiTextureBridgeModule", "AlreadyStoppedTitle", "UI Texture Bridge already stopped"),
			FText::GetEmpty(),
			SNotificationItem::CS_Pending);
		return;
	}

	Server->Stop();
	ShowNotification(
		NSLOCTEXT("UiTextureBridgeModule", "StoppedTitle", "UI Texture Bridge stopped"),
		FText::GetEmpty(),
		SNotificationItem::CS_Success);
	UE_LOG(LogUiTextureBridgeModule, Display, TEXT("UI Texture Bridge stopped"));
}

void FUiTextureBridgeModule::OpenSettings()
{
	ISettingsModule* const SettingsModule = FModuleManager::LoadModulePtr<ISettingsModule>(TEXT("Settings"));
	if (SettingsModule == nullptr)
	{
		ShowNotification(
			NSLOCTEXT("UiTextureBridgeModule", "SettingsUnavailableTitle", "Unable to open settings"),
			NSLOCTEXT("UiTextureBridgeModule", "SettingsUnavailableBody", "The Unreal Settings module is not available."),
			SNotificationItem::CS_Fail);
		return;
	}

	SettingsModule->ShowViewer(SettingsContainerName, SettingsCategoryName, SettingsSectionName);
}

bool FUiTextureBridgeModule::IsServerRunning() const
{
	return Server.IsValid() && Server->IsRunning();
}

void FUiTextureBridgeModule::RegisterSettings()
{
	ISettingsModule* const SettingsModule = FModuleManager::LoadModulePtr<ISettingsModule>(TEXT("Settings"));
	if (SettingsModule == nullptr)
	{
		return;
	}

	SettingsModule->RegisterSettings(
		SettingsContainerName,
		SettingsCategoryName,
		SettingsSectionName,
		NSLOCTEXT("UiTextureBridgeModule", "SettingsName", "UI Texture Bridge"),
		NSLOCTEXT("UiTextureBridgeModule", "SettingsDescription", "Configure the local HTTP server used to import UI textures."),
		GetMutableDefault<UUiTextureBridgeSettings>());
}

void FUiTextureBridgeModule::UnregisterSettings()
{
	ISettingsModule* const SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
	if (SettingsModule == nullptr)
	{
		return;
	}

	SettingsModule->UnregisterSettings(SettingsContainerName, SettingsCategoryName, SettingsSectionName);
}
