#include "Import/UiTextureImportService.h"
#include "Misc/AutomationTest.h"
#include "Misc/Base64.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Settings/UiTextureBridgeSettings.h"
#include "ToolMenus.h"
#include "UiTextureBridgeModule.h"
#include "Engine/Texture2D.h"
#include "UObject/GarbageCollection.h"
#include "UObject/Package.h"
#include "FileHelpers.h"
#include "SocketSubsystem.h"
#include "Sockets.h"

namespace
{
	const TCHAR* TestPngBase64 = TEXT("iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAANSURBVBhXY/jPwPAfAAUAAf+mXJtdAAAAAElFTkSuQmCC");

	FString MakeAutomationDirectory()
	{
		const FString Directory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Automation"), TEXT("UiTextureBridge"));
		IFileManager::Get().MakeDirectory(*Directory, true);
		return Directory;
	}

	FString MakePngFile(const FString& InStem)
	{
		TArray<uint8> Bytes;
		FBase64::Decode(TestPngBase64, Bytes);

		const FString Filename = FPaths::Combine(MakeAutomationDirectory(), InStem + TEXT(".png"));
		FFileHelper::SaveArrayToFile(Bytes, *Filename);
		return Filename;
	}

	FString MakeTextFile(const FString& InStem)
	{
		const FString Filename = FPaths::Combine(MakeAutomationDirectory(), InStem + TEXT(".txt"));
		FFileHelper::SaveStringToFile(TEXT("not a texture"), *Filename);
		return Filename;
	}

	FString MakeImportJson(const FString& InSourceFile, const FString& InDestinationPath = FString(), const FString& InAssetName = FString())
	{
		FString Json = FString::Printf(TEXT("{\"sourceFile\":\"%s\""), *InSourceFile.Replace(TEXT("\\"), TEXT("/")));
		if (!InDestinationPath.IsEmpty())
		{
			Json += FString::Printf(TEXT(",\"destinationPath\":\"%s\""), *InDestinationPath);
		}
		if (!InAssetName.IsEmpty())
		{
			Json += FString::Printf(TEXT(",\"assetName\":\"%s\""), *InAssetName);
		}
		Json += TEXT("}");
		return Json;
	}

	void CleanupImportedAsset(const FString& InPackagePath)
	{
		UPackage* const Package = FindPackage(nullptr, *InPackagePath);
		if (Package != nullptr)
		{
			bool bUnloadedAny = false;
			FText UnloadError;
			TArray<UPackage*> Packages;
			Packages.Add(Package);
			UEditorLoadingAndSavingUtils::UnloadPackages(Packages, bUnloadedAny, UnloadError);
			CollectGarbage(RF_NoFlags);
		}

		const FString PackageFilename = FPackageName::LongPackageNameToFilename(InPackagePath, FPackageName::GetAssetPackageExtension());
		IFileManager::Get().Delete(*PackageFilename);
		IFileManager::Get().Delete(*FPaths::ChangeExtension(PackageFilename, TEXT("uexp")));
		IFileManager::Get().Delete(*FPaths::ChangeExtension(PackageFilename, TEXT("ubulk")));
		IFileManager::Get().Delete(*FPaths::ChangeExtension(PackageFilename, TEXT("uptnl")));
	}

	int32 FindAvailablePort()
	{
		ISocketSubsystem* const SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		if (SocketSubsystem == nullptr)
		{
			return 30181;
		}

		TSharedRef<FInternetAddr> Address = SocketSubsystem->CreateInternetAddr();
		Address->SetLoopbackAddress();
		Address->SetPort(0);

		FSocket* const Socket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("UiTextureBridgeAutomationPortProbe"), false);
		if (Socket == nullptr)
		{
			return 30181;
		}

		int32 ResultPort = 30181;
		if (Socket->Bind(*Address))
		{
			Socket->GetAddress(*Address);
			ResultPort = Address->GetPort();
		}

		Socket->Close();
		SocketSubsystem->DestroySocket(Socket);
		return ResultPort;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUiTextureBridgeMenuRegistrationTest, "UiTextureBridge.Menu.Registration", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FUiTextureBridgeMenuRegistrationTest::RunTest(const FString& Parameters)
{
	TestTrue(TEXT("UiTextureBridge module loads"), FModuleManager::Get().LoadModulePtr<IModuleInterface>(TEXT("UiTextureBridge")) != nullptr);

	UToolMenus* const ToolMenus = UToolMenus::Get();
	TestNotNull(TEXT("ToolMenus is available"), ToolMenus);
	if (ToolMenus == nullptr)
	{
		return false;
	}

	UToolMenu* const ToolsMenu = ToolMenus->FindMenu(TEXT("LevelEditor.MainMenu.Tools"));
	TestNotNull(TEXT("Tools menu exists"), ToolsMenu);
	if (ToolsMenu == nullptr)
	{
		return false;
	}

	TestTrue(TEXT("UI Texture Bridge section exists"), ToolsMenu->FindSection(TEXT("UiTextureBridge")) != nullptr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUiTextureBridgeSettingsDefaultsTest, "UiTextureBridge.Settings.Defaults", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FUiTextureBridgeSettingsDefaultsTest::RunTest(const FString& Parameters)
{
	const UUiTextureBridgeSettings* const Settings = GetDefault<UUiTextureBridgeSettings>();
	TestEqual(TEXT("Default port"), Settings->Port, 30081);
	TestEqual(TEXT("Default bind address"), Settings->BindAddress, FString(TEXT("127.0.0.1")));
	TestEqual(TEXT("Default destination path"), Settings->DefaultDestinationPath, FString(TEXT("/Game/UI/Textures")));
	TestEqual(TEXT("Default preset"), Settings->DefaultPreset, FString(TEXT("ui_default")));
	TestFalse(TEXT("Auto start defaults to false"), Settings->bAutoStart);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUiTextureBridgeRequestDefaultsTest, "UiTextureBridge.Import.RequestDefaults", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FUiTextureBridgeRequestDefaultsTest::RunTest(const FString& Parameters)
{
	const FString SourceFile = MakePngFile(TEXT("RequestDefaults"));
	FUiTextureBridgeImportRequest Request;
	FUiTextureBridgeImportResult Failure;
	const FUiTextureBridgeSettingsSnapshot Snapshot = FUiTextureBridgeSettingsSnapshot::FromSettings(*GetDefault<UUiTextureBridgeSettings>());

	const bool bParsed = FUiTextureImportService::TryParseRequest(MakeImportJson(SourceFile), Snapshot, Request, Failure);
	TestTrue(TEXT("Minimal request parses"), bParsed);
	if (!bParsed)
	{
		IFileManager::Get().Delete(*SourceFile);
		return false;
	}

	TestEqual(TEXT("Default destination path applied"), Request.DestinationPath, Snapshot.DefaultDestinationPath);
	TestEqual(TEXT("Default preset applied"), Request.Preset, FString(TEXT("ui_default")));
	TestTrue(TEXT("Auto-generated asset name is prefixed"), Request.AssetName.StartsWith(TEXT("T_")));
	TestTrue(TEXT("Package path uses destination path"), Request.PackagePath.StartsWith(TEXT("/Game/UI/Textures/")));
	IFileManager::Get().Delete(*SourceFile);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUiTextureBridgeInvalidExtensionTest, "UiTextureBridge.Import.InvalidExtension", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FUiTextureBridgeInvalidExtensionTest::RunTest(const FString& Parameters)
{
	const FString SourceFile = MakeTextFile(TEXT("InvalidExtension"));
	FUiTextureBridgeImportRequest Request;
	FUiTextureBridgeImportResult Failure;
	const FUiTextureBridgeSettingsSnapshot Snapshot = FUiTextureBridgeSettingsSnapshot::FromSettings(*GetDefault<UUiTextureBridgeSettings>());

	const bool bParsed = FUiTextureImportService::TryParseRequest(MakeImportJson(SourceFile), Snapshot, Request, Failure);
	TestFalse(TEXT("Unsupported extension is rejected"), bParsed);
	TestEqual(TEXT("Unsupported extension returns bad request"), Failure.HttpStatusCode, 400);
	TestTrue(TEXT("Failure mentions unsupported extension"), Failure.Error.Contains(TEXT("Unsupported texture source extension")));
	IFileManager::Get().Delete(*SourceFile);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUiTextureBridgeImportRoundTripTest, "UiTextureBridge.Import.RoundTrip", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FUiTextureBridgeImportRoundTripTest::RunTest(const FString& Parameters)
{
	const FString SourceFile = MakePngFile(TEXT("RoundTrip"));
	const FString DestinationPath = TEXT("/Game/UiTextureBridgeTests");
	const FString AssetName = TEXT("T_AutomationUiTextureBridge");
	const FString PackagePath = DestinationPath / AssetName;

	CleanupImportedAsset(PackagePath);

	FUiTextureBridgeImportRequest Request;
	FUiTextureBridgeImportResult Failure;
	const FUiTextureBridgeSettingsSnapshot Snapshot = FUiTextureBridgeSettingsSnapshot::FromSettings(*GetDefault<UUiTextureBridgeSettings>());
	const bool bParsed = FUiTextureImportService::TryParseRequest(MakeImportJson(SourceFile, DestinationPath, AssetName), Snapshot, Request, Failure);
	TestTrue(TEXT("Round-trip request parses"), bParsed);
	if (!bParsed)
	{
		IFileManager::Get().Delete(*SourceFile);
		return false;
	}

	const FUiTextureBridgeImportResult FirstImport = FUiTextureImportService::ExecuteImport(Request);
	TestTrue(TEXT("First import succeeds"), FirstImport.bSuccess);
	if (!FirstImport.bSuccess)
	{
		CleanupImportedAsset(PackagePath);
		IFileManager::Get().Delete(*SourceFile);
		return false;
	}

	UTexture2D* const Texture = LoadObject<UTexture2D>(nullptr, *(PackagePath + TEXT(".") + AssetName));
	TestNotNull(TEXT("Imported texture loads"), Texture);
	if (Texture != nullptr)
	{
		TestEqual(TEXT("Texture group set to UI"), static_cast<int32>(Texture->LODGroup), static_cast<int32>(TEXTUREGROUP_UI));
		TestEqual(TEXT("MipGenSettings disabled"), static_cast<int32>(Texture->MipGenSettings), static_cast<int32>(TMGS_NoMipmaps));
		TestTrue(TEXT("Texture uses sRGB"), Texture->SRGB);
		TestTrue(TEXT("Texture never streams"), Texture->NeverStream);
	}

	const FUiTextureBridgeImportResult SecondImport = FUiTextureImportService::ExecuteImport(Request);
	TestTrue(TEXT("Overwrite import also succeeds"), SecondImport.bSuccess);

	CleanupImportedAsset(PackagePath);
	IFileManager::Get().Delete(*SourceFile);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUiTextureBridgeServerLifecycleTest, "UiTextureBridge.Server.Lifecycle", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FUiTextureBridgeServerLifecycleTest::RunTest(const FString& Parameters)
{
	FUiTextureBridgeModule& Module = FUiTextureBridgeModule::Get();
	UUiTextureBridgeSettings* const Settings = GetMutableDefault<UUiTextureBridgeSettings>();

	const int32 OriginalPort = Settings->Port;
	const FString OriginalBindAddress = Settings->BindAddress;
	const bool bOriginalAutoStart = Settings->bAutoStart;

	Settings->Port = FindAvailablePort();
	Settings->BindAddress = TEXT("127.0.0.1");
	Settings->bAutoStart = false;

	Module.StopServer();
	TestFalse(TEXT("Server starts stopped"), Module.IsServerRunning());

	Module.StartServer();
	TestTrue(TEXT("Server starts successfully"), Module.IsServerRunning());

	Module.StartServer();
	TestTrue(TEXT("Repeated start stays running"), Module.IsServerRunning());

	Module.StopServer();
	TestFalse(TEXT("Server stops cleanly"), Module.IsServerRunning());

	Settings->Port = OriginalPort;
	Settings->BindAddress = OriginalBindAddress;
	Settings->bAutoStart = bOriginalAutoStart;
	return true;
}
