#include "Import/UiTextureImportService.h"

#include "AssetImportTask.h"
#include "AssetToolsModule.h"
#include "Dom/JsonObject.h"
#include "Engine/Texture2D.h"
#include "Factories/TextureFactory.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "ObjectTools.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Settings/UiTextureBridgeSettings.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

DEFINE_LOG_CATEGORY_STATIC(LogUiTextureImportService, Log, All);

namespace
{
	const TCHAR* JsonFieldSourceFile = TEXT("sourceFile");
	const TCHAR* JsonFieldDestinationPath = TEXT("destinationPath");
	const TCHAR* JsonFieldAssetName = TEXT("assetName");
	const TCHAR* JsonFieldOverwrite = TEXT("overwrite");
	const TCHAR* JsonFieldPreset = TEXT("preset");

	FString SerializeJson(const TSharedRef<FJsonObject>& InObject)
	{
		FString Output;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
		FJsonSerializer::Serialize(InObject, Writer);
		return Output;
	}

	void SetFailure(
		FUiTextureBridgeImportResult& OutResult,
		const int32 InHttpStatusCode,
		const FString& InSourceFile,
		const FString& InAssetPath,
		const FString& InError)
	{
		OutResult.bSuccess = false;
		OutResult.HttpStatusCode = InHttpStatusCode;
		OutResult.SourceFile = InSourceFile;
		OutResult.AssetPath = InAssetPath;
		OutResult.Error = InError;
	}

	bool DeserializeObject(const FString& InJsonText, TSharedPtr<FJsonObject>& OutObject)
	{
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(InJsonText);
		return FJsonSerializer::Deserialize(Reader, OutObject) && OutObject.IsValid();
	}

	FString CanonicalizeDestinationPath(FString InDestinationPath)
	{
		InDestinationPath.TrimStartAndEndInline();
		while (InDestinationPath.EndsWith(TEXT("/")))
		{
			InDestinationPath.LeftChopInline(1);
		}
		return InDestinationPath;
	}

	FString SanitizeAssetName(const FString& InCandidate)
	{
		FString Sanitized = ObjectTools::SanitizeObjectName(InCandidate);
		if (Sanitized.IsEmpty())
		{
			Sanitized = TEXT("Texture");
		}

		if (!Sanitized.StartsWith(TEXT("T_"), ESearchCase::IgnoreCase))
		{
			Sanitized = FString::Printf(TEXT("T_%s"), *Sanitized);
		}

		return Sanitized;
	}

	bool IsSupportedExtension(const FString& InSourceFile)
	{
		const FString Extension = FPaths::GetExtension(InSourceFile, true).ToLower();
		return Extension == TEXT(".png")
			|| Extension == TEXT(".jpg")
			|| Extension == TEXT(".jpeg")
			|| Extension == TEXT(".bmp")
			|| Extension == TEXT(".tga");
	}

	FString MakeObjectPath(const FString& InPackagePath)
	{
		return InPackagePath + TEXT(".") + FPackageName::GetLongPackageAssetName(InPackagePath);
	}

	bool SaveImportedObject(UObject& InObject, FString& OutError)
	{
		UPackage* const Package = InObject.GetPackage();
		if (Package == nullptr)
		{
			OutError = FString::Printf(TEXT("Object '%s' has no package to save."), *InObject.GetName());
			return false;
		}

		const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		SaveArgs.Error = GError;

		if (!UPackage::SavePackage(Package, &InObject, *Filename, SaveArgs))
		{
			OutError = FString::Printf(TEXT("Failed to save imported package '%s'."), *Package->GetName());
			return false;
		}

		return true;
	}
}

FString FUiTextureImportService::GetDefaultPresetName()
{
	return TEXT("ui_default");
}

TArray<FString> FUiTextureImportService::GetSupportedPresets()
{
	return { GetDefaultPresetName() };
}

bool FUiTextureImportService::TryParseRequest(
	const FString& InJsonText,
	const FUiTextureBridgeSettingsSnapshot& InSettings,
	FUiTextureBridgeImportRequest& OutRequest,
	FUiTextureBridgeImportResult& OutFailure)
{
	TSharedPtr<FJsonObject> JsonObject;
	if (!DeserializeObject(InJsonText, JsonObject))
	{
		SetFailure(OutFailure, 400, FString(), FString(), TEXT("Request body must be a JSON object."));
		return false;
	}

	if (!JsonObject->TryGetStringField(JsonFieldSourceFile, OutRequest.SourceFile) || OutRequest.SourceFile.TrimStartAndEnd().IsEmpty())
	{
		SetFailure(OutFailure, 400, FString(), FString(), TEXT("Field 'sourceFile' is required."));
		return false;
	}

	OutRequest.SourceFile = FPaths::ConvertRelativePathToFull(OutRequest.SourceFile);
	if (!FPaths::FileExists(OutRequest.SourceFile))
	{
		SetFailure(OutFailure, 404, OutRequest.SourceFile, FString(), FString::Printf(TEXT("Source file does not exist: %s"), *OutRequest.SourceFile));
		return false;
	}

	if (!IsSupportedExtension(OutRequest.SourceFile))
	{
		SetFailure(
			OutFailure,
			400,
			OutRequest.SourceFile,
			FString(),
			FString::Printf(TEXT("Unsupported texture source extension: %s"), *FPaths::GetExtension(OutRequest.SourceFile, true)));
		return false;
	}

	OutRequest.DestinationPath = InSettings.DefaultDestinationPath;
	JsonObject->TryGetStringField(JsonFieldDestinationPath, OutRequest.DestinationPath);
	OutRequest.DestinationPath = CanonicalizeDestinationPath(OutRequest.DestinationPath);
	if (OutRequest.DestinationPath.IsEmpty())
	{
		SetFailure(OutFailure, 400, OutRequest.SourceFile, FString(), TEXT("Destination path must not be empty."));
		return false;
	}

	OutRequest.AssetName = FPaths::GetBaseFilename(OutRequest.SourceFile);
	JsonObject->TryGetStringField(JsonFieldAssetName, OutRequest.AssetName);
	OutRequest.AssetName = SanitizeAssetName(OutRequest.AssetName);

	OutRequest.bOverwrite = true;
	JsonObject->TryGetBoolField(JsonFieldOverwrite, OutRequest.bOverwrite);

	OutRequest.Preset = InSettings.DefaultPreset;
	JsonObject->TryGetStringField(JsonFieldPreset, OutRequest.Preset);
	if (!OutRequest.Preset.Equals(GetDefaultPresetName(), ESearchCase::IgnoreCase))
	{
		SetFailure(
			OutFailure,
			400,
			OutRequest.SourceFile,
			FString(),
			FString::Printf(TEXT("Unsupported preset '%s'."), *OutRequest.Preset));
		return false;
	}
	OutRequest.Preset = GetDefaultPresetName();

	OutRequest.PackagePath = OutRequest.DestinationPath / OutRequest.AssetName;
	FText ValidationError;
	if (!FPackageName::IsValidLongPackageName(OutRequest.PackagePath, true, &ValidationError))
	{
		SetFailure(
			OutFailure,
			400,
			OutRequest.SourceFile,
			OutRequest.PackagePath,
			FString::Printf(TEXT("Invalid destination package path '%s': %s"), *OutRequest.PackagePath, *ValidationError.ToString()));
		return false;
	}

	if (!OutRequest.bOverwrite && FPackageName::DoesPackageExist(OutRequest.PackagePath))
	{
		SetFailure(
			OutFailure,
			400,
			OutRequest.SourceFile,
			OutRequest.PackagePath,
			FString::Printf(TEXT("Asset already exists at '%s' and overwrite is disabled."), *OutRequest.PackagePath));
		return false;
	}

	return true;
}

FUiTextureBridgeImportResult FUiTextureImportService::ExecuteImport(const FUiTextureBridgeImportRequest& InRequest)
{
	check(IsInGameThread());

	FUiTextureBridgeImportResult Result;
	Result.SourceFile = InRequest.SourceFile;
	Result.AssetPath = InRequest.PackagePath;
	Result.HttpStatusCode = 200;

	UAssetImportTask* const ImportTask = NewObject<UAssetImportTask>(GetTransientPackage());
	UTextureFactory* const TextureFactory = ImportTask != nullptr ? NewObject<UTextureFactory>(ImportTask) : nullptr;
	if (ImportTask == nullptr || TextureFactory == nullptr)
	{
		SetFailure(Result, 500, InRequest.SourceFile, InRequest.PackagePath, TEXT("Failed to allocate Unreal import helpers."));
		return Result;
	}

	ImportTask->Filename = InRequest.SourceFile;
	ImportTask->DestinationPath = InRequest.DestinationPath;
	ImportTask->DestinationName = InRequest.AssetName;
	ImportTask->bReplaceExisting = true;
	ImportTask->bReplaceExistingSettings = true;
	ImportTask->bAutomated = true;
	ImportTask->bSave = false;
	ImportTask->bAsync = false;
	ImportTask->Factory = TextureFactory;

	TArray<UAssetImportTask*> ImportTasks;
	ImportTasks.Add(ImportTask);

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().ImportAssetTasks(ImportTasks);

	UTexture2D* ImportedTexture = nullptr;
	for (UObject* const ImportedObject : ImportTask->GetObjects())
	{
		ImportedTexture = Cast<UTexture2D>(ImportedObject);
		if (ImportedTexture != nullptr)
		{
			break;
		}
	}

	if (ImportedTexture == nullptr)
	{
		ImportedTexture = LoadObject<UTexture2D>(nullptr, *MakeObjectPath(InRequest.PackagePath));
	}

	if (ImportedTexture == nullptr)
	{
		SetFailure(
			Result,
			500,
			InRequest.SourceFile,
			InRequest.PackagePath,
			FString::Printf(TEXT("Import did not produce a texture asset at '%s'."), *InRequest.PackagePath));
		return Result;
	}

	ImportedTexture->Modify();
	ImportedTexture->LODGroup = TEXTUREGROUP_UI;
	ImportedTexture->MipGenSettings = TMGS_NoMipmaps;
	ImportedTexture->SRGB = true;
	ImportedTexture->NeverStream = true;
	ImportedTexture->MarkPackageDirty();
	ImportedTexture->PostEditChange();
	ImportedTexture->UpdateResource();

	UPackage* const TexturePackage = ImportedTexture->GetPackage();
	if (TexturePackage == nullptr)
	{
		SetFailure(Result, 500, InRequest.SourceFile, InRequest.PackagePath, TEXT("Imported texture has no package to save."));
		return Result;
	}

	FString SaveError;
	if (!SaveImportedObject(*ImportedTexture, SaveError))
	{
		SetFailure(
			Result,
			500,
			InRequest.SourceFile,
			InRequest.PackagePath,
			SaveError);
		return Result;
	}

	Result.bSuccess = true;
	return Result;
}

FString FUiTextureImportService::BuildResponseJson(const FUiTextureBridgeImportResult& InResult)
{
	const TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
	RootObject->SetBoolField(TEXT("success"), InResult.bSuccess);
	RootObject->SetStringField(TEXT("assetPath"), InResult.AssetPath);
	RootObject->SetStringField(TEXT("sourceFile"), InResult.SourceFile);
	RootObject->SetStringField(TEXT("error"), InResult.Error);

	TArray<TSharedPtr<FJsonValue>> WarningValues;
	for (const FString& Warning : InResult.Warnings)
	{
		WarningValues.Add(MakeShared<FJsonValueString>(Warning));
	}
	RootObject->SetArrayField(TEXT("warnings"), WarningValues);

	return SerializeJson(RootObject);
}
