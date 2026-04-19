#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MannyPoseTypes.h"
#include "MannyPoseJsonLibrary.generated.h"

UCLASS()
class MANNYPOSETOOLKIT_API UMannyPoseJsonLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="MannyPose")
    static bool LoadPoseFromJsonString(const FString& JsonString, FMannyPoseData& OutPose, FString& OutError);

    UFUNCTION(BlueprintCallable, Category="MannyPose")
    static bool LoadPoseFromJsonFile(const FString& AbsoluteFilePath, FMannyPoseData& OutPose, FString& OutError);

    UFUNCTION(BlueprintCallable, Category="MannyPose")
    static bool LoadHandPresetFromJsonString(const FString& JsonString, FMannyHandPresetData& OutPreset, FString& OutError);

    UFUNCTION(BlueprintCallable, Category="MannyPose")
    static bool LoadHandPresetFromJsonFile(const FString& AbsoluteFilePath, FMannyHandPresetData& OutPreset, FString& OutError);

    UFUNCTION(BlueprintCallable, Category="MannyPose")
    static FString ToPrettyJson(const FMannyPoseData& Pose);

    UFUNCTION(BlueprintCallable, Category="MannyPose")
    static FString HandPresetToPrettyJson(const FMannyHandPresetData& Preset);
};
