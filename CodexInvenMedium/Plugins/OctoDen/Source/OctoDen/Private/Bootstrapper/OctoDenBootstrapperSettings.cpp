#include "Bootstrapper/OctoDenBootstrapperSettings.h"

#include "Misc/App.h"
#include "Shared/OctoDenAssetNaming.h"

namespace
{
	const FString ManagedMapsFolder = TEXT("/Game/Maps");
}

void UOctoDenBootstrapperSettings::ResetToDefaults()
{
	DeriveDefaultsFromProject();
	LastStatus = FText::GetEmpty();
}

void UOctoDenBootstrapperSettings::DeriveDefaultsFromProject()
{
	const FString ProjectName = OctoDenAssetNaming::ToPascalIdentifier(FApp::GetProjectName(), TEXT("Project"));
	const FString SafeMapName = OctoDenAssetNaming::SanitizeAssetName(ManagedMapName, TEXT("Main"));

	RuntimeModuleName = FApp::GetProjectName();
	ManagedMapName = SafeMapName;
	GameInstanceClassName = TEXT("CodexInvenMediumGameInstance");
	GameInstanceBlueprintFolder = TEXT("/Game/Blueprints/Core");
	GameInstanceBlueprintName = FString::Printf(TEXT("BP_%sGameInstance"), *ProjectName);
	GameModeClassName = FString::Printf(TEXT("%s%sGameMode"), *ProjectName, *OctoDenAssetNaming::ToPascalIdentifier(SafeMapName, TEXT("Main")));
	GameModeBlueprintFolder = TEXT("/Game/Blueprints/Maps");
	GameModeBlueprintName = FString::Printf(TEXT("BP_%s"), *GameModeClassName);

	const FString MapObjectPath = OctoDenAssetNaming::BuildObjectPath(ManagedMapsFolder, SafeMapName);
	TargetMap = FSoftObjectPath(MapObjectPath);
	EditorStartupMap = TargetMap;
	GameDefaultMap = TargetMap;
}

FString UOctoDenBootstrapperSettings::GetManagedMapPackagePath() const
{
	return OctoDenAssetNaming::BuildPackagePath(TEXT("/Game/Maps"), ManagedMapName);
}
