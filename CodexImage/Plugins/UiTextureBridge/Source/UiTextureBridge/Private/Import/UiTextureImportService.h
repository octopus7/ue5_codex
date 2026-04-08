#pragma once

#include "CoreMinimal.h"

struct FUiTextureBridgeSettingsSnapshot;

struct FUiTextureBridgeImportRequest
{
	FString SourceFile;
	FString DestinationPath;
	FString AssetName;
	FString PackagePath;
	FString Preset;
	bool bOverwrite = true;
};

struct FUiTextureBridgeImportResult
{
	bool bSuccess = false;
	int32 HttpStatusCode = 200;
	FString AssetPath;
	FString SourceFile;
	TArray<FString> Warnings;
	FString Error;
};

class FUiTextureImportService
{
public:
	static FString GetDefaultPresetName();
	static TArray<FString> GetSupportedPresets();

	static bool TryParseRequest(
		const FString& InJsonText,
		const FUiTextureBridgeSettingsSnapshot& InSettings,
		FUiTextureBridgeImportRequest& OutRequest,
		FUiTextureBridgeImportResult& OutFailure);

	static FUiTextureBridgeImportResult ExecuteImport(const FUiTextureBridgeImportRequest& InRequest);
	static FString BuildResponseJson(const FUiTextureBridgeImportResult& InResult);
};
