#include "MannyPoseJsonLibrary.h"

#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace MannyPoseJsonLibrary
{
    template<typename TStructType>
    bool LoadStructFromJsonString(const FString& JsonString, TStructType& OutStruct, FString& OutError)
    {
        TSharedPtr<FJsonObject> JsonObject;
        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
        if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
        {
            OutError = TEXT("Failed to deserialize JSON.");
            return false;
        }

        if (!FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &OutStruct))
        {
            OutError = TEXT("Failed to convert JSON object to target struct.");
            return false;
        }

        return true;
    }

    template<typename TStructType>
    bool LoadStructFromJsonFile(const FString& AbsoluteFilePath, TStructType& OutStruct, FString& OutError)
    {
        FString JsonString;
        if (!FFileHelper::LoadFileToString(JsonString, *AbsoluteFilePath))
        {
            OutError = FString::Printf(TEXT("Unable to read file: %s"), *AbsoluteFilePath);
            return false;
        }

        return LoadStructFromJsonString(JsonString, OutStruct, OutError);
    }

    template<typename TStructType>
    FString StructToPrettyJson(const TStructType& Value)
    {
        FString OutJson;
        FJsonObjectConverter::UStructToJsonObjectString(Value, OutJson, 0, 0, 0, nullptr, true);
        return OutJson;
    }
}

bool UMannyPoseJsonLibrary::LoadPoseFromJsonString(const FString& JsonString, FMannyPoseData& OutPose, FString& OutError)
{
    return MannyPoseJsonLibrary::LoadStructFromJsonString(JsonString, OutPose, OutError);
}

bool UMannyPoseJsonLibrary::LoadPoseFromJsonFile(const FString& AbsoluteFilePath, FMannyPoseData& OutPose, FString& OutError)
{
    return MannyPoseJsonLibrary::LoadStructFromJsonFile(AbsoluteFilePath, OutPose, OutError);
}

bool UMannyPoseJsonLibrary::LoadHandPresetFromJsonString(const FString& JsonString, FMannyHandPresetData& OutPreset, FString& OutError)
{
    return MannyPoseJsonLibrary::LoadStructFromJsonString(JsonString, OutPreset, OutError);
}

bool UMannyPoseJsonLibrary::LoadHandPresetFromJsonFile(const FString& AbsoluteFilePath, FMannyHandPresetData& OutPreset, FString& OutError)
{
    return MannyPoseJsonLibrary::LoadStructFromJsonFile(AbsoluteFilePath, OutPreset, OutError);
}

FString UMannyPoseJsonLibrary::ToPrettyJson(const FMannyPoseData& Pose)
{
    return MannyPoseJsonLibrary::StructToPrettyJson(Pose);
}

FString UMannyPoseJsonLibrary::HandPresetToPrettyJson(const FMannyHandPresetData& Preset)
{
    return MannyPoseJsonLibrary::StructToPrettyJson(Preset);
}
