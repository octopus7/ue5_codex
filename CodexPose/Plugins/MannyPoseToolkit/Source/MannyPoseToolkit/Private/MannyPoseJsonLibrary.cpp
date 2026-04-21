#include "MannyPoseJsonLibrary.h"

#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace MannyPoseJsonLibrary
{
    TSharedPtr<FJsonValue> FindFieldValueCaseInsensitive(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName)
    {
        if (!JsonObject.IsValid())
        {
            return nullptr;
        }

        for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : JsonObject->Values)
        {
            if (Pair.Key.Equals(FieldName, ESearchCase::IgnoreCase))
            {
                return Pair.Value;
            }
        }

        return nullptr;
    }

    bool TryGetStringFieldCaseInsensitive(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, FString& OutString)
    {
        const TSharedPtr<FJsonValue> Value = FindFieldValueCaseInsensitive(JsonObject, FieldName);
        if (!Value.IsValid() || Value->Type != EJson::String)
        {
            return false;
        }

        OutString = Value->AsString();
        return true;
    }

    bool TryGetObjectFieldCaseInsensitive(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, TSharedPtr<FJsonObject>& OutObject)
    {
        const TSharedPtr<FJsonValue> Value = FindFieldValueCaseInsensitive(JsonObject, FieldName);
        if (!Value.IsValid() || Value->Type != EJson::Object)
        {
            return false;
        }

        OutObject = Value->AsObject();
        return OutObject.IsValid();
    }

    bool TryGetNumberFieldCaseInsensitive(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, double& OutNumber)
    {
        const TSharedPtr<FJsonValue> Value = FindFieldValueCaseInsensitive(JsonObject, FieldName);
        return Value.IsValid() && Value->TryGetNumber(OutNumber);
    }

    bool ParseNumberTriplet(const TArray<TSharedPtr<FJsonValue>>& ArrayValue, double& OutA, double& OutB, double& OutC)
    {
        return ArrayValue.Num() == 3
            && ArrayValue[0].IsValid() && ArrayValue[0]->TryGetNumber(OutA)
            && ArrayValue[1].IsValid() && ArrayValue[1]->TryGetNumber(OutB)
            && ArrayValue[2].IsValid() && ArrayValue[2]->TryGetNumber(OutC);
    }

    bool ParseVectorValue(const TSharedPtr<FJsonValue>& Value, FVector& OutVector, FString& OutError, const FString& Context)
    {
        if (!Value.IsValid())
        {
            OutError = FString::Printf(TEXT("Missing vector value for %s."), *Context);
            return false;
        }

        if (Value->Type == EJson::Array)
        {
            double X = 0.0;
            double Y = 0.0;
            double Z = 0.0;
            if (!ParseNumberTriplet(Value->AsArray(), X, Y, Z))
            {
                OutError = FString::Printf(TEXT("Expected 3 numeric values for %s."), *Context);
                return false;
            }

            OutVector = FVector(X, Y, Z);
            return true;
        }

        if (Value->Type == EJson::Object)
        {
            const TSharedPtr<FJsonObject> ObjectValue = Value->AsObject();
            double X = 0.0;
            double Y = 0.0;
            double Z = 0.0;
            if (!TryGetNumberFieldCaseInsensitive(ObjectValue, TEXT("X"), X)
                || !TryGetNumberFieldCaseInsensitive(ObjectValue, TEXT("Y"), Y)
                || !TryGetNumberFieldCaseInsensitive(ObjectValue, TEXT("Z"), Z))
            {
                OutError = FString::Printf(TEXT("Expected X/Y/Z numeric fields for %s."), *Context);
                return false;
            }

            OutVector = FVector(X, Y, Z);
            return true;
        }

        OutError = FString::Printf(TEXT("Unsupported vector format for %s."), *Context);
        return false;
    }

    bool ParseRotatorValue(const TSharedPtr<FJsonValue>& Value, FRotator& OutRotator, FString& OutError, const FString& Context)
    {
        if (!Value.IsValid())
        {
            OutError = FString::Printf(TEXT("Missing rotator value for %s."), *Context);
            return false;
        }

        if (Value->Type == EJson::Array)
        {
            double Pitch = 0.0;
            double Yaw = 0.0;
            double Roll = 0.0;
            if (!ParseNumberTriplet(Value->AsArray(), Pitch, Yaw, Roll))
            {
                OutError = FString::Printf(TEXT("Expected 3 numeric values for %s."), *Context);
                return false;
            }

            OutRotator = FRotator(Pitch, Yaw, Roll);
            return true;
        }

        if (Value->Type == EJson::Object)
        {
            const TSharedPtr<FJsonObject> ObjectValue = Value->AsObject();
            double Pitch = 0.0;
            double Yaw = 0.0;
            double Roll = 0.0;
            if (!TryGetNumberFieldCaseInsensitive(ObjectValue, TEXT("Pitch"), Pitch)
                || !TryGetNumberFieldCaseInsensitive(ObjectValue, TEXT("Yaw"), Yaw)
                || !TryGetNumberFieldCaseInsensitive(ObjectValue, TEXT("Roll"), Roll))
            {
                OutError = FString::Printf(TEXT("Expected Pitch/Yaw/Roll numeric fields for %s."), *Context);
                return false;
            }

            OutRotator = FRotator(Pitch, Yaw, Roll);
            return true;
        }

        OutError = FString::Printf(TEXT("Unsupported rotator format for %s."), *Context);
        return false;
    }

    bool ParseRotationMap(const TSharedPtr<FJsonObject>& JsonObject, TMap<FName, FRotator>& OutMap, FString& OutError, const FString& Context)
    {
        OutMap.Reset();
        if (!JsonObject.IsValid())
        {
            return true;
        }

        for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : JsonObject->Values)
        {
            FRotator Rotation = FRotator::ZeroRotator;
            if (!ParseRotatorValue(Pair.Value, Rotation, OutError, FString::Printf(TEXT("%s.%s"), *Context, *Pair.Key)))
            {
                return false;
            }

            OutMap.Add(FName(*Pair.Key), Rotation);
        }

        return true;
    }

    bool ParseBoneRotationMap(const TSharedPtr<FJsonObject>& JsonObject, TMap<FName, FMannyPoseBoneRotation>& OutMap, FString& OutError, const FString& Context)
    {
        OutMap.Reset();
        if (!JsonObject.IsValid())
        {
            return true;
        }

        for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : JsonObject->Values)
        {
            FMannyPoseBoneRotation BoneRotation;
            TSharedPtr<FJsonValue> RotationValue = Pair.Value;

            if (Pair.Value.IsValid() && Pair.Value->Type == EJson::Object)
            {
                const TSharedPtr<FJsonObject> BoneObject = Pair.Value->AsObject();
                if (const TSharedPtr<FJsonValue> NestedRotation = FindFieldValueCaseInsensitive(BoneObject, TEXT("Rotation")))
                {
                    RotationValue = NestedRotation;
                }
            }

            if (!ParseRotatorValue(RotationValue, BoneRotation.Rotation, OutError, FString::Printf(TEXT("%s.%s"), *Context, *Pair.Key)))
            {
                return false;
            }

            OutMap.Add(FName(*Pair.Key), BoneRotation);
        }

        return true;
    }

    bool ParseControlTransformMap(const TSharedPtr<FJsonObject>& JsonObject, TMap<FName, FMannyPoseControlTransform>& OutMap, FString& OutError, const FString& Context)
    {
        OutMap.Reset();
        if (!JsonObject.IsValid())
        {
            return true;
        }

        for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : JsonObject->Values)
        {
            if (!Pair.Value.IsValid() || Pair.Value->Type != EJson::Object)
            {
                OutError = FString::Printf(TEXT("Expected object for %s.%s."), *Context, *Pair.Key);
                return false;
            }

            const TSharedPtr<FJsonObject> TransformObject = Pair.Value->AsObject();
            FMannyPoseControlTransform ControlTransform;

            if (const TSharedPtr<FJsonValue> LocationValue = FindFieldValueCaseInsensitive(TransformObject, TEXT("Location")))
            {
                if (!ParseVectorValue(LocationValue, ControlTransform.Location, OutError, FString::Printf(TEXT("%s.%s.Location"), *Context, *Pair.Key)))
                {
                    return false;
                }
            }

            if (const TSharedPtr<FJsonValue> RotationValue = FindFieldValueCaseInsensitive(TransformObject, TEXT("Rotation")))
            {
                if (!ParseRotatorValue(RotationValue, ControlTransform.Rotation, OutError, FString::Printf(TEXT("%s.%s.Rotation"), *Context, *Pair.Key)))
                {
                    return false;
                }
            }

            if (const TSharedPtr<FJsonValue> ScaleValue = FindFieldValueCaseInsensitive(TransformObject, TEXT("Scale")))
            {
                if (!ParseVectorValue(ScaleValue, ControlTransform.Scale, OutError, FString::Printf(TEXT("%s.%s.Scale"), *Context, *Pair.Key)))
                {
                    return false;
                }
            }

            OutMap.Add(FName(*Pair.Key), ControlTransform);
        }

        return true;
    }

    bool LoadJsonObjectFromString(const FString& JsonString, TSharedPtr<FJsonObject>& OutJsonObject, FString& OutError)
    {
        const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
        if (!FJsonSerializer::Deserialize(Reader, OutJsonObject) || !OutJsonObject.IsValid())
        {
            OutError = TEXT("Failed to deserialize JSON.");
            return false;
        }

        return true;
    }

    bool LoadJsonObjectFromFile(const FString& AbsoluteFilePath, TSharedPtr<FJsonObject>& OutJsonObject, FString& OutError)
    {
        FString JsonString;
        if (!FFileHelper::LoadFileToString(JsonString, *AbsoluteFilePath))
        {
            OutError = FString::Printf(TEXT("Unable to read file: %s"), *AbsoluteFilePath);
            return false;
        }

        return LoadJsonObjectFromString(JsonString, OutJsonObject, OutError);
    }

    bool ParsePoseData(const TSharedPtr<FJsonObject>& JsonObject, FMannyPoseData& OutPose, FString& OutError)
    {
        FMannyPoseData ParsedPose;

        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("Version"), ParsedPose.Version);
        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("Skeleton"), ParsedPose.Skeleton);
        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("ReferencePose"), ParsedPose.ReferencePose);
        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("Name"), ParsedPose.Name);
        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("Category"), ParsedPose.Category);
        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("Notes"), ParsedPose.Notes);
        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("LeftHandPreset"), ParsedPose.LeftHandPreset);
        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("RightHandPreset"), ParsedPose.RightHandPreset);

        TSharedPtr<FJsonObject> ControlObject;
        if (TryGetObjectFieldCaseInsensitive(JsonObject, TEXT("IKControls"), ControlObject)
            && !ParseControlTransformMap(ControlObject, ParsedPose.IKControls, OutError, TEXT("IKControls")))
        {
            return false;
        }

        TSharedPtr<FJsonObject> BoneObject;
        if (TryGetObjectFieldCaseInsensitive(JsonObject, TEXT("FKBones"), BoneObject)
            && !ParseBoneRotationMap(BoneObject, ParsedPose.FKBones, OutError, TEXT("FKBones")))
        {
            return false;
        }

        TSharedPtr<FJsonObject> FingerOffsetObject;
        if (TryGetObjectFieldCaseInsensitive(JsonObject, TEXT("FingerOffsets"), FingerOffsetObject)
            && !ParseRotationMap(FingerOffsetObject, ParsedPose.FingerOffsets, OutError, TEXT("FingerOffsets")))
        {
            return false;
        }

        OutPose = ParsedPose;
        return true;
    }

    bool ParseHandPresetData(const TSharedPtr<FJsonObject>& JsonObject, FMannyHandPresetData& OutPreset, FString& OutError)
    {
        FMannyHandPresetData ParsedPreset;

        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("Version"), ParsedPreset.Version);
        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("Skeleton"), ParsedPreset.Skeleton);
        TryGetStringFieldCaseInsensitive(JsonObject, TEXT("Name"), ParsedPreset.Name);

        TSharedPtr<FJsonObject> LeftHandObject;
        if (TryGetObjectFieldCaseInsensitive(JsonObject, TEXT("LeftHandRotations"), LeftHandObject)
            && !ParseRotationMap(LeftHandObject, ParsedPreset.LeftHandRotations, OutError, TEXT("LeftHandRotations")))
        {
            return false;
        }

        TSharedPtr<FJsonObject> RightHandObject;
        if (TryGetObjectFieldCaseInsensitive(JsonObject, TEXT("RightHandRotations"), RightHandObject)
            && !ParseRotationMap(RightHandObject, ParsedPreset.RightHandRotations, OutError, TEXT("RightHandRotations")))
        {
            return false;
        }

        OutPreset = ParsedPreset;
        return true;
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
    TSharedPtr<FJsonObject> JsonObject;
    return MannyPoseJsonLibrary::LoadJsonObjectFromString(JsonString, JsonObject, OutError)
        && MannyPoseJsonLibrary::ParsePoseData(JsonObject, OutPose, OutError);
}

bool UMannyPoseJsonLibrary::LoadPoseFromJsonFile(const FString& AbsoluteFilePath, FMannyPoseData& OutPose, FString& OutError)
{
    TSharedPtr<FJsonObject> JsonObject;
    return MannyPoseJsonLibrary::LoadJsonObjectFromFile(AbsoluteFilePath, JsonObject, OutError)
        && MannyPoseJsonLibrary::ParsePoseData(JsonObject, OutPose, OutError);
}

bool UMannyPoseJsonLibrary::LoadHandPresetFromJsonString(const FString& JsonString, FMannyHandPresetData& OutPreset, FString& OutError)
{
    TSharedPtr<FJsonObject> JsonObject;
    return MannyPoseJsonLibrary::LoadJsonObjectFromString(JsonString, JsonObject, OutError)
        && MannyPoseJsonLibrary::ParseHandPresetData(JsonObject, OutPreset, OutError);
}

bool UMannyPoseJsonLibrary::LoadHandPresetFromJsonFile(const FString& AbsoluteFilePath, FMannyHandPresetData& OutPreset, FString& OutError)
{
    TSharedPtr<FJsonObject> JsonObject;
    return MannyPoseJsonLibrary::LoadJsonObjectFromFile(AbsoluteFilePath, JsonObject, OutError)
        && MannyPoseJsonLibrary::ParseHandPresetData(JsonObject, OutPreset, OutError);
}

FString UMannyPoseJsonLibrary::ToPrettyJson(const FMannyPoseData& Pose)
{
    return MannyPoseJsonLibrary::StructToPrettyJson(Pose);
}

FString UMannyPoseJsonLibrary::HandPresetToPrettyJson(const FMannyHandPresetData& Preset)
{
    return MannyPoseJsonLibrary::StructToPrettyJson(Preset);
}
