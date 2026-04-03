#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "VoxImportedAssetUserData.generated.h"

UCLASS()
class VOXIMPORTERRUNTIME_API UVoxImportedAssetUserData : public UAssetUserData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Vox")
	int32 MarkerVersion = 1;

	UPROPERTY(VisibleAnywhere, Category = "Vox")
	FString SourceFilename;

	UPROPERTY(VisibleAnywhere, Category = "Vox")
	bool bIsSmoothReconstruction = false;

	UPROPERTY(VisibleAnywhere, Category = "Vox")
	float ReconstructionResolutionScale = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Vox")
	FString GeneratedFromAssetPath;
};
