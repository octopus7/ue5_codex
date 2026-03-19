#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OctoDenBootstrapperSettings.generated.h"

UENUM()
enum class EOctoDenManagedMapTemplate : uint8
{
	Basic,
	Blank,
	TimeOfDay
};

UCLASS(Transient)
class UOctoDenBootstrapperSettings : public UObject
{
	GENERATED_BODY()

public:
	void ResetToDefaults();
	void DeriveDefaultsFromProject();
	FString GetManagedMapPackagePath() const;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	FString RuntimeModuleName = TEXT("CodexInvenMedium");

	UPROPERTY(EditAnywhere, Category = "Map")
	FString ManagedMapName = TEXT("Main");

	UPROPERTY(EditAnywhere, Category = "Map")
	EOctoDenManagedMapTemplate ManagedMapTemplate = EOctoDenManagedMapTemplate::Basic;

	UPROPERTY(EditAnywhere, Category = "Map")
	FSoftObjectPath TargetMap;

	UPROPERTY(EditAnywhere, Category = "Map")
	FSoftObjectPath EditorStartupMap;

	UPROPERTY(EditAnywhere, Category = "Map")
	FSoftObjectPath GameDefaultMap;

	UPROPERTY(EditAnywhere, Category = "Map")
	bool bSetGlobalDefaultGameMode = true;

	UPROPERTY(EditAnywhere, Category = "GameInstance")
	FString GameInstanceClassName = TEXT("CodexInvenMediumGameInstance");

	UPROPERTY(EditAnywhere, Category = "GameInstance")
	FString GameInstanceBlueprintFolder = TEXT("/Game/Blueprints/Core");

	UPROPERTY(EditAnywhere, Category = "GameInstance")
	FString GameInstanceBlueprintName = TEXT("BP_CodexInvenMediumGameInstance");

	UPROPERTY(EditAnywhere, Category = "GameMode")
	FString GameModeClassName = TEXT("CodexInvenMediumMainGameMode");

	UPROPERTY(EditAnywhere, Category = "GameMode")
	FString GameModeBlueprintFolder = TEXT("/Game/Blueprints/Maps");

	UPROPERTY(EditAnywhere, Category = "GameMode")
	FString GameModeBlueprintName = TEXT("BP_CodexInvenMediumMainGameMode");

	UPROPERTY(VisibleAnywhere, Category = "Status")
	FText LastStatus;
};
