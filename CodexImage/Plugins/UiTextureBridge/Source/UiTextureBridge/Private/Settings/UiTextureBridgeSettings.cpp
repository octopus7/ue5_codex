#include "Settings/UiTextureBridgeSettings.h"

#define LOCTEXT_NAMESPACE "UiTextureBridgeSettings"

FUiTextureBridgeSettingsSnapshot FUiTextureBridgeSettingsSnapshot::FromSettings(const UUiTextureBridgeSettings& InSettings)
{
	FUiTextureBridgeSettingsSnapshot Snapshot;
	Snapshot.Port = InSettings.Port;
	Snapshot.BindAddress = InSettings.BindAddress;
	Snapshot.DefaultDestinationPath = InSettings.DefaultDestinationPath;
	Snapshot.DefaultPreset = InSettings.DefaultPreset;
	Snapshot.RequestTimeoutSeconds = InSettings.RequestTimeoutSeconds;
	Snapshot.bAutoStart = InSettings.bAutoStart;
	return Snapshot;
}

UUiTextureBridgeSettings::UUiTextureBridgeSettings()
{
	CategoryName = TEXT("Plugins");
	SectionName = TEXT("UiTextureBridge");
}

FName UUiTextureBridgeSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName UUiTextureBridgeSettings::GetCategoryName() const
{
	return TEXT("Plugins");
}

FName UUiTextureBridgeSettings::GetSectionName() const
{
	return TEXT("UiTextureBridge");
}

#if WITH_EDITOR
FText UUiTextureBridgeSettings::GetSectionText() const
{
	return LOCTEXT("SectionText", "UI Texture Bridge");
}

FText UUiTextureBridgeSettings::GetSectionDescription() const
{
	return LOCTEXT("SectionDescription", "Configure the local HTTP bridge used to import UI textures into this project.");
}
#endif

#undef LOCTEXT_NAMESPACE
