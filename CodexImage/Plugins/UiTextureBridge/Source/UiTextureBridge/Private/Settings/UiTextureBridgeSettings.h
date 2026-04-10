#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UiTextureBridgeSettings.generated.h"

class UUiTextureBridgeSettings;

struct FUiTextureBridgeSettingsSnapshot
{
	int32 Port = 30081;
	FString BindAddress = TEXT("127.0.0.1");
	FString DefaultDestinationPath = TEXT("/Game/UI/Textures");
	FString DefaultPreset = TEXT("ui_default");
	float RequestTimeoutSeconds = 10.0f;
	bool bAutoStart = false;

	static FUiTextureBridgeSettingsSnapshot FromSettings(const UUiTextureBridgeSettings& InSettings);
};

UCLASS(Config = Editor, defaultconfig, meta = (DisplayName = "UI Texture Bridge"))
class UUiTextureBridgeSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UUiTextureBridgeSettings();

	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif

	UPROPERTY(EditAnywhere, Config, Category = "Server", meta = (ClampMin = "1024", ClampMax = "65535"))
	int32 Port = 30081;

	UPROPERTY(EditAnywhere, Config, Category = "Server")
	FString BindAddress = TEXT("127.0.0.1");

	UPROPERTY(EditAnywhere, Config, Category = "Import")
	FString DefaultDestinationPath = TEXT("/Game/UI/Textures");

	UPROPERTY(EditAnywhere, Config, Category = "Import")
	FString DefaultPreset = TEXT("ui_default");

	UPROPERTY(EditAnywhere, Config, Category = "Server", meta = (ClampMin = "0.1", ClampMax = "120.0"))
	float RequestTimeoutSeconds = 10.0f;

	UPROPERTY(EditAnywhere, Config, Category = "Server")
	bool bAutoStart = true;
};
