// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CodexEditorAssetLibrary.generated.h"

UCLASS()
class CODEXPLANET_API UCodexEditorAssetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CodexPlanet|Editor")
	static FString CreateOrbitControlsWidgetBlueprint(const FString& AssetPath = TEXT("/Game/UI/WBP_PlanetOrbitControls"));
};
