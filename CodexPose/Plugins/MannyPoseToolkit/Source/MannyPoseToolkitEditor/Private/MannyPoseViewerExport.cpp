#include "MannyPoseViewerExport.h"

#include "Dom/JsonObject.h"
#include "Engine/SkeletalMesh.h"
#include "HAL/FileManager.h"
#include "MannyPoseJsonLibrary.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
    struct FPreviewBoneStyle
    {
        FName Name;
        FLinearColor Color;
        float Thickness;
    };

    const TArray<FPreviewBoneStyle>& GetPreviewBoneStyles()
    {
        static const TArray<FPreviewBoneStyle> BoneStyles =
        {
            { TEXT("pelvis"), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("spine_01"), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("spine_02"), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("spine_03"), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("spine_04"), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("spine_05"), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("neck_01"), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("head"), FLinearColor(0.80f, 0.84f, 0.90f), 3.0f },

            { TEXT("clavicle_l"), FLinearColor(0.35f, 0.80f, 1.00f), 2.0f },
            { TEXT("upperarm_l"), FLinearColor(0.35f, 0.80f, 1.00f), 2.0f },
            { TEXT("lowerarm_l"), FLinearColor(0.35f, 0.80f, 1.00f), 2.0f },
            { TEXT("hand_l"), FLinearColor(0.35f, 0.80f, 1.00f), 2.0f },

            { TEXT("clavicle_r"), FLinearColor(1.00f, 0.72f, 0.35f), 2.0f },
            { TEXT("upperarm_r"), FLinearColor(1.00f, 0.72f, 0.35f), 2.0f },
            { TEXT("lowerarm_r"), FLinearColor(1.00f, 0.72f, 0.35f), 2.0f },
            { TEXT("hand_r"), FLinearColor(1.00f, 0.72f, 0.35f), 2.0f },

            { TEXT("thigh_l"), FLinearColor(0.35f, 0.80f, 1.00f), 2.5f },
            { TEXT("calf_l"), FLinearColor(0.35f, 0.80f, 1.00f), 2.5f },
            { TEXT("foot_l"), FLinearColor(0.35f, 0.80f, 1.00f), 2.5f },
            { TEXT("ball_l"), FLinearColor(0.35f, 0.80f, 1.00f), 2.5f },

            { TEXT("thigh_r"), FLinearColor(1.00f, 0.72f, 0.35f), 2.5f },
            { TEXT("calf_r"), FLinearColor(1.00f, 0.72f, 0.35f), 2.5f },
            { TEXT("foot_r"), FLinearColor(1.00f, 0.72f, 0.35f), 2.5f },
            { TEXT("ball_r"), FLinearColor(1.00f, 0.72f, 0.35f), 2.5f }
        };

        return BoneStyles;
    }

    const FPreviewBoneStyle* FindPreviewBoneStyle(const FName BoneName)
    {
        for (const FPreviewBoneStyle& BoneStyle : GetPreviewBoneStyles())
        {
            if (BoneStyle.Name == BoneName)
            {
                return &BoneStyle;
            }
        }

        return nullptr;
    }

    TArray<TSharedPtr<FJsonValue>> MakeNumberArray(const std::initializer_list<double> Values)
    {
        TArray<TSharedPtr<FJsonValue>> Result;
        Result.Reserve(static_cast<int32>(Values.size()));

        for (const double Value : Values)
        {
            Result.Add(MakeShared<FJsonValueNumber>(Value));
        }

        return Result;
    }

    FString GetRelativeOrAbsolutePath(const FString& AbsolutePath)
    {
        FString RelativePath = AbsolutePath;
        if (FPaths::MakePathRelativeTo(RelativePath, *FPaths::ProjectDir()))
        {
            return RelativePath;
        }

        return AbsolutePath;
    }

    void CollectExportBoneIndices(const FReferenceSkeleton& RefSkeleton, TArray<int32>& OutBoneIndices)
    {
        TSet<int32> BoneIndexSet;

        for (const FPreviewBoneStyle& BoneStyle : GetPreviewBoneStyles())
        {
            int32 BoneIndex = RefSkeleton.FindBoneIndex(BoneStyle.Name);
            while (BoneIndex != INDEX_NONE)
            {
                BoneIndexSet.Add(BoneIndex);
                BoneIndex = RefSkeleton.GetParentIndex(BoneIndex);
            }
        }

        OutBoneIndices = BoneIndexSet.Array();
        OutBoneIndices.Sort();
    }

    TSet<FName> BuildAllowedBoneNameSet(const FReferenceSkeleton& RefSkeleton, const TArray<int32>& BoneIndices)
    {
        TSet<FName> AllowedBoneNames;
        AllowedBoneNames.Reserve(BoneIndices.Num());

        for (const int32 BoneIndex : BoneIndices)
        {
            AllowedBoneNames.Add(RefSkeleton.GetBoneName(BoneIndex));
        }

        return AllowedBoneNames;
    }

    TSharedRef<FJsonObject> BuildSkeletonJson(const USkeletalMesh& SkeletalMesh, const TArray<int32>& BoneIndices)
    {
        const FReferenceSkeleton& RefSkeleton = SkeletalMesh.GetRefSkeleton();
        const TArray<FTransform>& RefBonePose = RefSkeleton.GetRefBonePose();

        TArray<TSharedPtr<FJsonValue>> BoneArray;
        BoneArray.Reserve(BoneIndices.Num());

        for (const int32 BoneIndex : BoneIndices)
        {
            if (!RefBonePose.IsValidIndex(BoneIndex))
            {
                continue;
            }

            const FTransform& LocalTransform = RefBonePose[BoneIndex];
            const FQuat LocalRotation = LocalTransform.GetRotation().GetNormalized();
            const FVector LocalTranslation = LocalTransform.GetLocation();
            const FVector LocalScale = LocalTransform.GetScale3D();
            const FName BoneName = RefSkeleton.GetBoneName(BoneIndex);
            const int32 ParentIndex = RefSkeleton.GetParentIndex(BoneIndex);
            const FPreviewBoneStyle* PreviewStyle = FindPreviewBoneStyle(BoneName);

            TSharedRef<FJsonObject> BoneObject = MakeShared<FJsonObject>();
            BoneObject->SetStringField(TEXT("name"), BoneName.ToString());

            if (ParentIndex == INDEX_NONE)
            {
                BoneObject->SetField(TEXT("parent"), MakeShared<FJsonValueNull>());
            }
            else
            {
                BoneObject->SetStringField(TEXT("parent"), RefSkeleton.GetBoneName(ParentIndex).ToString());
            }

            BoneObject->SetArrayField(TEXT("localTranslation"), MakeNumberArray(
            {
                LocalTranslation.X,
                LocalTranslation.Y,
                LocalTranslation.Z
            }));
            BoneObject->SetArrayField(TEXT("localRotationQuaternion"), MakeNumberArray(
            {
                LocalRotation.X,
                LocalRotation.Y,
                LocalRotation.Z,
                LocalRotation.W
            }));
            BoneObject->SetArrayField(TEXT("localScale"), MakeNumberArray(
            {
                LocalScale.X,
                LocalScale.Y,
                LocalScale.Z
            }));
            BoneObject->SetBoolField(TEXT("showInPreview"), PreviewStyle != nullptr);
            BoneObject->SetArrayField(TEXT("previewColor"), MakeNumberArray(
            {
                PreviewStyle ? PreviewStyle->Color.R : 0.18,
                PreviewStyle ? PreviewStyle->Color.G : 0.20,
                PreviewStyle ? PreviewStyle->Color.B : 0.24,
                PreviewStyle ? PreviewStyle->Color.A : 0.55
            }));
            BoneObject->SetNumberField(TEXT("previewThickness"), PreviewStyle ? PreviewStyle->Thickness : 1.0);

            BoneArray.Add(MakeShared<FJsonValueObject>(BoneObject));
        }

        TSharedRef<FJsonObject> SkeletonObject = MakeShared<FJsonObject>();
        SkeletonObject->SetStringField(TEXT("name"), SkeletalMesh.GetSkeleton() ? SkeletalMesh.GetSkeleton()->GetName() : TEXT("UE5_Manny"));
        SkeletonObject->SetStringField(TEXT("sourceSkeletalMesh"), SkeletalMesh.GetPathName());
        SkeletonObject->SetStringField(TEXT("sourcePoseDirectory"), TEXT("Plugins/MannyPoseToolkit/Content/Poses/Bodies"));
        SkeletonObject->SetStringField(TEXT("units"), TEXT("cm"));
        SkeletonObject->SetStringField(TEXT("exportedAtUtc"), FDateTime::UtcNow().ToIso8601());
        SkeletonObject->SetArrayField(TEXT("previewViewForward"), MakeNumberArray({ -1.0, -1.3, -0.45 }));
        SkeletonObject->SetArrayField(TEXT("previewUp"), MakeNumberArray({ 0.0, 0.0, 1.0 }));
        SkeletonObject->SetArrayField(TEXT("bones"), BoneArray);
        return SkeletonObject;
    }

    TSharedRef<FJsonObject> BuildPoseJson(const FMannyPoseData& PoseData, const FString& SourceFile, const TSet<FName>& AllowedBoneNames)
    {
        TSharedRef<FJsonObject> PoseObject = MakeShared<FJsonObject>();
        PoseObject->SetStringField(TEXT("name"), PoseData.Name);
        PoseObject->SetStringField(TEXT("category"), PoseData.Category);
        PoseObject->SetStringField(TEXT("notes"), PoseData.Notes);
        PoseObject->SetStringField(TEXT("sourceFile"), GetRelativeOrAbsolutePath(SourceFile));
        PoseObject->SetStringField(TEXT("leftHandPreset"), PoseData.LeftHandPreset);
        PoseObject->SetStringField(TEXT("rightHandPreset"), PoseData.RightHandPreset);

        TSharedRef<FJsonObject> BoneOffsetsObject = MakeShared<FJsonObject>();

        TArray<FName> PoseBoneNames;
        PoseData.FKBones.GetKeys(PoseBoneNames);
        PoseBoneNames.Sort(FNameLexicalLess());

        for (const FName BoneName : PoseBoneNames)
        {
            if (!AllowedBoneNames.Contains(BoneName))
            {
                continue;
            }

            const FMannyPoseBoneRotation* BoneOffset = PoseData.FKBones.Find(BoneName);
            if (!BoneOffset)
            {
                continue;
            }

            const FQuat RotationOffsetQuat = FQuat(BoneOffset->Rotation).GetNormalized();

            TSharedRef<FJsonObject> BoneObject = MakeShared<FJsonObject>();
            BoneObject->SetArrayField(TEXT("location"), MakeNumberArray(
            {
                BoneOffset->Location.X,
                BoneOffset->Location.Y,
                BoneOffset->Location.Z
            }));
            BoneObject->SetArrayField(TEXT("rotationQuaternion"), MakeNumberArray(
            {
                RotationOffsetQuat.X,
                RotationOffsetQuat.Y,
                RotationOffsetQuat.Z,
                RotationOffsetQuat.W
            }));

            BoneOffsetsObject->SetObjectField(BoneName.ToString(), BoneObject);
        }

        PoseObject->SetObjectField(TEXT("boneOffsets"), BoneOffsetsObject);
        return PoseObject;
    }
}

bool MannyPoseViewerExport::ExportViewerDataset(
    const USkeletalMesh& SkeletalMesh,
    const FString& BodyPoseDirectory,
    const FString& OutputPath,
    FExportSummary& OutSummary,
    FString& OutError)
{
    OutSummary = FExportSummary();
    OutError.Reset();

    const FReferenceSkeleton& RefSkeleton = SkeletalMesh.GetRefSkeleton();
    if (RefSkeleton.GetNum() == 0)
    {
        OutError = TEXT("The selected skeletal mesh does not have a reference skeleton.");
        return false;
    }

    TArray<int32> BoneIndices;
    CollectExportBoneIndices(RefSkeleton, BoneIndices);
    if (BoneIndices.IsEmpty())
    {
        OutError = TEXT("No preview bones were found on the selected skeletal mesh.");
        return false;
    }

    TArray<FString> BodyPoseFiles;
    IFileManager::Get().FindFiles(BodyPoseFiles, *(BodyPoseDirectory / TEXT("*.json")), true, false);
    BodyPoseFiles.Sort();

    if (BodyPoseFiles.IsEmpty())
    {
        OutError = FString::Printf(TEXT("No body pose JSON files were found in %s."), *BodyPoseDirectory);
        return false;
    }

    const TSet<FName> AllowedBoneNames = BuildAllowedBoneNameSet(RefSkeleton, BoneIndices);

    TArray<TSharedPtr<FJsonValue>> PoseArray;
    PoseArray.Reserve(BodyPoseFiles.Num());

    for (const FString& BodyPoseFile : BodyPoseFiles)
    {
        const FString AbsolutePosePath = BodyPoseDirectory / BodyPoseFile;
        FMannyPoseData PoseData;
        FString PoseError;
        if (!UMannyPoseJsonLibrary::LoadPoseFromJsonFile(AbsolutePosePath, PoseData, PoseError))
        {
            OutError = FString::Printf(TEXT("Failed to load pose %s: %s"), *BodyPoseFile, *PoseError);
            return false;
        }

        PoseArray.Add(MakeShared<FJsonValueObject>(BuildPoseJson(PoseData, AbsolutePosePath, AllowedBoneNames)));
    }

    TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
    RootObject->SetStringField(TEXT("version"), TEXT("1.0"));
    RootObject->SetStringField(TEXT("datasetType"), TEXT("MannyPoseViewerDataset"));
    RootObject->SetObjectField(TEXT("skeleton"), BuildSkeletonJson(SkeletalMesh, BoneIndices));
    RootObject->SetArrayField(TEXT("poses"), PoseArray);

    const FString OutputDirectory = FPaths::GetPath(OutputPath);
    if (!OutputDirectory.IsEmpty() && !IFileManager::Get().MakeDirectory(*OutputDirectory, true))
    {
        OutError = FString::Printf(TEXT("Failed to create export directory: %s"), *OutputDirectory);
        return false;
    }

    FString JsonText;
    const TSharedRef<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>> Writer =
        TJsonWriterFactory<TCHAR, TPrettyJsonPrintPolicy<TCHAR>>::Create(&JsonText);

    if (!FJsonSerializer::Serialize(RootObject, Writer))
    {
        OutError = TEXT("Failed to serialize the viewer dataset.");
        return false;
    }

    if (!FFileHelper::SaveStringToFile(JsonText, *OutputPath))
    {
        OutError = FString::Printf(TEXT("Failed to save the viewer dataset to %s."), *OutputPath);
        return false;
    }

    OutSummary.BoneCount = BoneIndices.Num();
    OutSummary.PoseCount = PoseArray.Num();
    return true;
}
