#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "ProjectBootstrapperDialogSettings.generated.h"

UENUM()
enum class EProjectBootstrapperManagedMapTemplate : uint8
{
	Basic,
	Blank,
	TimeOfDay
};

UCLASS(Transient)
class UProjectBootstrapperDialogSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Code")
	FString TargetRuntimeModule;

	UPROPERTY(EditAnywhere, Category = "Maps", meta = (DisplayName = "Managed Map Name", ToolTip = "Used by the Create and Open buttons to manage /Game/Maps/<Name>."))
	FString ManagedMapName = TEXT("BasicMap");

	UPROPERTY(EditAnywhere, Category = "Maps", meta = (DisplayName = "Managed Map Template"))
	EProjectBootstrapperManagedMapTemplate ManagedMapTemplate = EProjectBootstrapperManagedMapTemplate::Basic;

	UPROPERTY(EditAnywhere, Category = "Maps")
	TSoftObjectPtr<UWorld> TargetMap;

	UPROPERTY(EditAnywhere, Category = "Maps")
	TSoftObjectPtr<UWorld> EditorStartupMap;

	UPROPERTY(EditAnywhere, Category = "Maps")
	TSoftObjectPtr<UWorld> GameDefaultMap;

	UPROPERTY(EditAnywhere, Category = "Maps")
	bool bSetAsGlobalDefaultGameMode = false;

	UPROPERTY(EditAnywhere, Category = "GameInstance")
	FString GameInstanceClassName;

	UPROPERTY(EditAnywhere, Category = "GameInstance", meta = (ToolTip = "Content browser folder path. Example: /Game/Blueprints/Core"))
	FString GameInstanceBlueprintFolder = TEXT("/Game/Blueprints/Core");

	UPROPERTY(EditAnywhere, Category = "GameInstance")
	FString GameInstanceBlueprintName;

	UPROPERTY(EditAnywhere, Category = "GameMode")
	FString GameModeClassName;

	UPROPERTY(EditAnywhere, Category = "GameMode", meta = (ToolTip = "Content browser folder path. Example: /Game/Blueprints/GameModes"))
	FString GameModeBlueprintFolder = TEXT("/Game/Blueprints/GameModes");

	UPROPERTY(EditAnywhere, Category = "GameMode")
	FString GameModeBlueprintName;
};
