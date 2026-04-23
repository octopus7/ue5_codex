#include "MannyPoseToolkitEditorWidget.h"

#include "Misc/MessageDialog.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "Components/PoseableMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "MannyPoseJsonLibrary.h"
#include "MannyPosePreviewActor.h"
#include "Rendering/DrawElements.h"
#include "Styling/AppStyle.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SLeafWidget.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SMannyPoseToolkitEditorWidget"

namespace MannyPoseToolkitEditorWidget
{
    FString BasenameWithoutExtension(const FString& Path)
    {
        return FPaths::GetBaseFilename(Path);
    }
}

namespace
{
    struct FBodyPreviewBoneDefinition
    {
        FName Name;
        FLinearColor Color;
        float Thickness;
    };

    struct FBodyPreviewLineSegment
    {
        FVector Start = FVector::ZeroVector;
        FVector End = FVector::ZeroVector;
        FLinearColor Color = FLinearColor::White;
        float Thickness = 1.f;
    };

    const TArray<FBodyPreviewBoneDefinition>& GetBodyPreviewBones()
    {
        static const TArray<FBodyPreviewBoneDefinition> Bones =
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

        return Bones;
    }

    const FBodyPreviewBoneDefinition* FindPreviewBoneDefinition(const FName BoneName)
    {
        for (const FBodyPreviewBoneDefinition& Bone : GetBodyPreviewBones())
        {
            if (Bone.Name == BoneName)
            {
                return &Bone;
            }
        }

        return nullptr;
    }

    const USkeletalMesh* GetPreviewSkeletalMesh(const AMannyPosePreviewActor* PreviewActor)
    {
        if (!PreviewActor || !PreviewActor->PoseableMesh)
        {
            return nullptr;
        }

        return Cast<USkeletalMesh>(PreviewActor->PoseableMesh->GetSkinnedAsset());
    }

    void BuildPoseLocalTransforms(const FReferenceSkeleton& RefSkeleton, const TArray<FTransform>& RefBonePose, const FMannyPoseData& PoseData, TArray<FTransform>& OutLocalTransforms)
    {
        OutLocalTransforms = RefBonePose;

        for (const TPair<FName, FMannyPoseBoneRotation>& Pair : PoseData.FKBones)
        {
            const int32 BoneIndex = RefSkeleton.FindBoneIndex(Pair.Key);
            if (!OutLocalTransforms.IsValidIndex(BoneIndex) || !RefBonePose.IsValidIndex(BoneIndex))
            {
                continue;
            }

            const FTransform& ReferenceLocalBoneTransform = RefBonePose[BoneIndex];
            const FQuat NewRotation = (FQuat(Pair.Value.Rotation) * ReferenceLocalBoneTransform.GetRotation()).GetNormalized();
            const FVector NewLocation = ReferenceLocalBoneTransform.GetLocation() + Pair.Value.Location;
            OutLocalTransforms[BoneIndex].SetLocation(NewLocation);
            OutLocalTransforms[BoneIndex].SetRotation(NewRotation);
        }

        for (const TPair<FName, FRotator>& Pair : PoseData.FingerOffsets)
        {
            const int32 BoneIndex = RefSkeleton.FindBoneIndex(Pair.Key);
            if (!OutLocalTransforms.IsValidIndex(BoneIndex))
            {
                continue;
            }

            const FQuat NewRotation = (FQuat(Pair.Value) * OutLocalTransforms[BoneIndex].GetRotation()).GetNormalized();
            OutLocalTransforms[BoneIndex].SetRotation(NewRotation);
        }
    }

    void BuildComponentTransforms(const FReferenceSkeleton& RefSkeleton, const TArray<FTransform>& LocalTransforms, TArray<FTransform>& OutComponentTransforms)
    {
        OutComponentTransforms.SetNum(LocalTransforms.Num());

        for (int32 BoneIndex = 0; BoneIndex < LocalTransforms.Num(); ++BoneIndex)
        {
            const int32 ParentIndex = RefSkeleton.GetParentIndex(BoneIndex);
            if (!OutComponentTransforms.IsValidIndex(BoneIndex))
            {
                continue;
            }

            if (ParentIndex == INDEX_NONE)
            {
                OutComponentTransforms[BoneIndex] = LocalTransforms[BoneIndex];
                continue;
            }

            const FTransform& ParentTransform = OutComponentTransforms[ParentIndex];
            const FTransform& LocalTransform = LocalTransforms[BoneIndex];

            FTransform ComponentTransform;
            ComponentTransform.SetScale3D(ParentTransform.GetScale3D() * LocalTransform.GetScale3D());
            ComponentTransform.SetRotation((ParentTransform.GetRotation() * LocalTransform.GetRotation()).GetNormalized());
            ComponentTransform.SetLocation(ParentTransform.TransformPosition(LocalTransform.GetLocation()));
            OutComponentTransforms[BoneIndex] = ComponentTransform;
        }
    }

    bool BuildPreviewLineSegments(const AMannyPosePreviewActor* PreviewActor, const FMannyPoseData& PoseData, TArray<FBodyPreviewLineSegment>& OutSegments)
    {
        OutSegments.Reset();

        const USkeletalMesh* SkeletalMesh = GetPreviewSkeletalMesh(PreviewActor);
        if (!SkeletalMesh)
        {
            return false;
        }

        const FReferenceSkeleton& RefSkeleton = SkeletalMesh->GetRefSkeleton();
        const TArray<FTransform>& RefBonePose = RefSkeleton.GetRefBonePose();
        if (RefBonePose.IsEmpty())
        {
            return false;
        }

        TArray<FTransform> LocalTransforms;
        BuildPoseLocalTransforms(RefSkeleton, RefBonePose, PoseData, LocalTransforms);

        TArray<FTransform> ComponentTransforms;
        BuildComponentTransforms(RefSkeleton, LocalTransforms, ComponentTransforms);

        for (const FBodyPreviewBoneDefinition& Bone : GetBodyPreviewBones())
        {
            const int32 BoneIndex = RefSkeleton.FindBoneIndex(Bone.Name);
            if (!ComponentTransforms.IsValidIndex(BoneIndex))
            {
                continue;
            }

            int32 ParentIndex = RefSkeleton.GetParentIndex(BoneIndex);
            while (ParentIndex != INDEX_NONE && FindPreviewBoneDefinition(RefSkeleton.GetBoneName(ParentIndex)) == nullptr)
            {
                ParentIndex = RefSkeleton.GetParentIndex(ParentIndex);
            }

            if (!ComponentTransforms.IsValidIndex(ParentIndex))
            {
                continue;
            }

            FBodyPreviewLineSegment Segment;
            Segment.Start = ComponentTransforms[ParentIndex].GetLocation();
            Segment.End = ComponentTransforms[BoneIndex].GetLocation();
            Segment.Color = Bone.Color;
            Segment.Thickness = Bone.Thickness;
            OutSegments.Add(Segment);
        }

        return !OutSegments.IsEmpty();
    }

    TArray<FName> GetDefaultBodyCaptureBoneNames()
    {
        return
        {
            TEXT("pelvis"),
            TEXT("spine_01"),
            TEXT("spine_02"),
            TEXT("spine_03"),
            TEXT("spine_04"),
            TEXT("spine_05"),
            TEXT("clavicle_l"),
            TEXT("clavicle_r"),
            TEXT("upperarm_l"),
            TEXT("upperarm_r"),
            TEXT("lowerarm_l"),
            TEXT("lowerarm_r"),
            TEXT("hand_l"),
            TEXT("hand_r"),
            TEXT("thigh_l"),
            TEXT("thigh_r"),
            TEXT("calf_l"),
            TEXT("calf_r"),
            TEXT("foot_l"),
            TEXT("foot_r"),
            TEXT("ball_l"),
            TEXT("ball_r"),
            TEXT("neck_01"),
            TEXT("head")
        };
    }

    bool CapturePoseFromPreviewActor(const AMannyPosePreviewActor* PreviewActor, FMannyPoseData& OutPose, FString& OutError)
    {
        if (!PreviewActor || !PreviewActor->PoseableMesh)
        {
            OutError = TEXT("Select an AMannyPosePreviewActor in the level first.");
            return false;
        }

        const UPoseableMeshComponent* PoseableMesh = PreviewActor->PoseableMesh;
        const USkeletalMesh* SkeletalMesh = GetPreviewSkeletalMesh(PreviewActor);
        if (!SkeletalMesh)
        {
            OutError = TEXT("Selected preview actor does not have a valid skeletal mesh.");
            return false;
        }

        const FReferenceSkeleton& RefSkeleton = SkeletalMesh->GetRefSkeleton();
        const TArray<FTransform>& RefBonePose = RefSkeleton.GetRefBonePose();
        if (RefBonePose.IsEmpty())
        {
            OutError = TEXT("Selected skeletal mesh has no reference pose data.");
            return false;
        }

        const TArray<FTransform>& CurrentBoneSpaceTransforms = PoseableMesh->BoneSpaceTransforms;
        FMannyPoseData CapturedPose;
        CapturedPose.Version = TEXT("1.2");
        CapturedPose.Skeleton = TEXT("UE5_Manny");
        CapturedPose.ReferencePose = TEXT("A_Pose");
        CapturedPose.Name = PreviewActor->CurrentPose.Name;
        CapturedPose.Category = PreviewActor->CurrentPose.Category;
        CapturedPose.Notes = PreviewActor->CurrentPose.Notes;
        CapturedPose.LeftHandPreset = PreviewActor->CurrentPose.LeftHandPreset;
        CapturedPose.RightHandPreset = PreviewActor->CurrentPose.RightHandPreset;
        CapturedPose.IKControls = PreviewActor->CurrentPose.IKControls;
        CapturedPose.FingerOffsets = PreviewActor->CurrentPose.FingerOffsets;

        const auto IsNearlyZeroRotator = [](const FRotator& Rotator)
        {
            return FMath::Abs(Rotator.Pitch) < 0.01f
                && FMath::Abs(Rotator.Yaw) < 0.01f
                && FMath::Abs(Rotator.Roll) < 0.01f;
        };

        for (const FName BoneName : GetDefaultBodyCaptureBoneNames())
        {
            const int32 BoneIndex = RefSkeleton.FindBoneIndex(BoneName);
            if (!RefBonePose.IsValidIndex(BoneIndex))
            {
                continue;
            }

            const FTransform& ReferenceLocalTransform = RefBonePose[BoneIndex];
            const FTransform& CurrentLocalTransform = CurrentBoneSpaceTransforms.IsValidIndex(BoneIndex)
                ? CurrentBoneSpaceTransforms[BoneIndex]
                : ReferenceLocalTransform;

            const FVector LocationOffset = CurrentLocalTransform.GetLocation() - ReferenceLocalTransform.GetLocation();
            const FQuat RotationOffsetQuat = (CurrentLocalTransform.GetRotation() * ReferenceLocalTransform.GetRotation().Inverse()).GetNormalized();
            FRotator RotationOffset = RotationOffsetQuat.Rotator();
            RotationOffset.Normalize();

            FMannyPoseBoneRotation BoneRotation;
            BoneRotation.Location = LocationOffset.GetAbsMax() < 0.01f ? FVector::ZeroVector : LocationOffset;
            BoneRotation.Rotation = IsNearlyZeroRotator(RotationOffset) ? FRotator::ZeroRotator : RotationOffset;
            CapturedPose.FKBones.Add(BoneName, BoneRotation);
        }

        OutPose = MoveTemp(CapturedPose);
        return true;
    }

    FVector2D ProjectPreviewPoint(const FVector& Point)
    {
        const FVector ViewForward = FVector(-1.0f, -1.3f, -0.45f).GetSafeNormal();
        const FVector ViewRight = FVector::CrossProduct(FVector::UpVector, ViewForward).GetSafeNormal();
        const FVector ViewUp = FVector::CrossProduct(ViewForward, ViewRight).GetSafeNormal();

        return FVector2D(
            FVector::DotProduct(Point, ViewRight),
            FVector::DotProduct(Point, ViewUp));
    }

}

class SMannyPoseBodyPreview : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SMannyPoseBodyPreview) {}
    SLATE_END_ARGS()

    void Construct(const FArguments&)
    {
    }

    void SetPreviewSegments(const TArray<FBodyPreviewLineSegment>* InPreviewSegments)
    {
        PreviewSegments = InPreviewSegments ? *InPreviewSegments : TArray<FBodyPreviewLineSegment>();
        Invalidate(EInvalidateWidget::Paint);
    }

    virtual FVector2D ComputeDesiredSize(float) const override
    {
        return FVector2D(260.f, 220.f);
    }

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
    {
        const FSlateBrush* WhiteBrush = FAppStyle::Get().GetBrush("WhiteBrush");
        const FVector2D Size = AllottedGeometry.GetLocalSize();

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(),
            WhiteBrush,
            ESlateDrawEffect::None,
            FLinearColor(0.07f, 0.08f, 0.10f, 1.f));

        const TArray<FVector2f> FloorLine =
        {
            FVector2f(12.f, Size.Y - 12.f),
            FVector2f(Size.X - 12.f, Size.Y - 12.f)
        };

        FSlateDrawElement::MakeLines(
            OutDrawElements,
            LayerId + 1,
            AllottedGeometry.ToPaintGeometry(),
            FloorLine,
            ESlateDrawEffect::None,
            FLinearColor(0.17f, 0.19f, 0.23f, 1.f),
            true,
            1.0f);

        if (PreviewSegments.IsEmpty())
        {
            return LayerId + 1;
        }

        TArray<FBodyPreviewLineSegment> ProjectedSegments;
        ProjectedSegments.Reserve(PreviewSegments.Num());

        FVector2D MinPoint(FLT_MAX, FLT_MAX);
        FVector2D MaxPoint(-FLT_MAX, -FLT_MAX);

        for (const FBodyPreviewLineSegment& Segment : PreviewSegments)
        {
            FBodyPreviewLineSegment ProjectedSegment = Segment;
            ProjectedSegment.Start = FVector(ProjectPreviewPoint(Segment.Start), 0.f);
            ProjectedSegment.End = FVector(ProjectPreviewPoint(Segment.End), 0.f);
            ProjectedSegments.Add(ProjectedSegment);

            MinPoint.X = FMath::Min(MinPoint.X, FMath::Min(static_cast<double>(ProjectedSegment.Start.X), static_cast<double>(ProjectedSegment.End.X)));
            MinPoint.Y = FMath::Min(MinPoint.Y, FMath::Min(static_cast<double>(ProjectedSegment.Start.Y), static_cast<double>(ProjectedSegment.End.Y)));
            MaxPoint.X = FMath::Max(MaxPoint.X, FMath::Max(static_cast<double>(ProjectedSegment.Start.X), static_cast<double>(ProjectedSegment.End.X)));
            MaxPoint.Y = FMath::Max(MaxPoint.Y, FMath::Max(static_cast<double>(ProjectedSegment.Start.Y), static_cast<double>(ProjectedSegment.End.Y)));
        }

        const FMargin Padding(14.f);
        const FVector2D AvailableSize(
            FMath::Max(Size.X - Padding.GetTotalSpaceAlong<Orient_Horizontal>(), 1.f),
            FMath::Max(Size.Y - Padding.GetTotalSpaceAlong<Orient_Vertical>(), 1.f));
        const FVector2D BoundsSize = MaxPoint - MinPoint;
        const float ScaleX = AvailableSize.X / FMath::Max(BoundsSize.X, 1.f);
        const float ScaleY = AvailableSize.Y / FMath::Max(BoundsSize.Y, 1.f);
        const float Scale = FMath::Min(ScaleX, ScaleY);
        const float ExtraX = (AvailableSize.X - BoundsSize.X * Scale) * 0.5f;
        const float BaselineY = Size.Y - Padding.Bottom;

        for (const FBodyPreviewLineSegment& Segment : ProjectedSegments)
        {
            const FVector2D StartPoint(
                Padding.Left + ExtraX + (Segment.Start.X - MinPoint.X) * Scale,
                BaselineY - (Segment.Start.Y - MinPoint.Y) * Scale);
            const FVector2D EndPoint(
                Padding.Left + ExtraX + (Segment.End.X - MinPoint.X) * Scale,
                BaselineY - (Segment.End.Y - MinPoint.Y) * Scale);

            const TArray<FVector2f> LinePoints =
            {
                FVector2f(static_cast<float>(StartPoint.X), static_cast<float>(StartPoint.Y)),
                FVector2f(static_cast<float>(EndPoint.X), static_cast<float>(EndPoint.Y))
            };

            FSlateDrawElement::MakeLines(
                OutDrawElements,
                LayerId + 2,
                AllottedGeometry.ToPaintGeometry(),
                LinePoints,
                ESlateDrawEffect::None,
                Segment.Color,
                true,
                Segment.Thickness);
        }

        return LayerId + 2;
    }

private:
    TArray<FBodyPreviewLineSegment> PreviewSegments;
};

void SMannyPoseToolkitEditorWidget::Construct(const FArguments& InArgs)
{
    RebuildPresetOptions();
    SetDefaultSelections();

    ChildSlot
    [
        SNew(SBorder)
        .Padding(12.f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 8.f)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("Header", "Select an AMannyPosePreviewActor in the level, assign the UE5 Manny skeletal mesh to its PoseableMesh, then apply bundled body and hand presets."))
                .AutoWrapText(true)
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 10.f)
            [ SNew(SSeparator) ]

            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 6.f)
            [ SNew(STextBlock).Text(LOCTEXT("BodyLabel", "Body Preset")) ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
            [
                SNew(SComboBox<FStringItem>)
                .OptionsSource(&BodyPresetItems)
                .InitiallySelectedItem(SelectedBodyPreset)
                .OnGenerateWidget(this, &SMannyPoseToolkitEditorWidget::GenerateComboItem)
                .OnSelectionChanged_Lambda([this](FStringItem NewItem, ESelectInfo::Type)
                {
                    SelectedBodyPreset = NewItem;
                    RefreshBodyPreview();
                })
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return FText::FromString(SelectedBodyPreset.IsValid() ? *SelectedBodyPreset : TEXT("None"));
                    })
                ]
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f, 0.f, 8.f)
            [
                SNew(SBorder)
                .Padding(8.f)
                .BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 6.f)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("BodyPreviewTitle", "Body Pose Preview"))
                    ]
                    + SVerticalBox::Slot().AutoHeight()
                    [
                        SNew(SBox)
                        .HeightOverride(220.f)
                        [
                            SAssignNew(BodyPreviewWidget, SMannyPoseBodyPreview)
                        ]
                    ]
                    + SVerticalBox::Slot().AutoHeight().Padding(0.f, 6.f, 0.f, 0.f)
                    [
                        SNew(STextBlock)
                        .Text_Lambda([this]()
                        {
                            return FText::FromString(BodyPreviewStatusText);
                        })
                        .ColorAndOpacity(FSlateColor::UseSubduedForeground())
                        .AutoWrapText(true)
                    ]
                ]
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f, 0.f, 10.f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1.f).Padding(0.f, 0.f, 4.f, 0.f)
                [
                    SNew(SButton)
                    .Text(LOCTEXT("ApplyBodyPreset", "Apply Body Preset"))
                    .OnClicked(this, &SMannyPoseToolkitEditorWidget::OnApplyBodyPreset)
                ]
                + SHorizontalBox::Slot().FillWidth(1.f).Padding(4.f, 0.f, 0.f, 0.f)
                [
                    SNew(SButton)
                    .Text(LOCTEXT("SaveBodyPreset", "Overwrite From Actor"))
                    .OnClicked(this, &SMannyPoseToolkitEditorWidget::OnSaveBodyPreset)
                ]
            ]

            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 10.f)
            [ SNew(SSeparator) ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 6.f)
            [ SNew(STextBlock).Text(LOCTEXT("HandLabel", "Hand Presets")) ]

            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
            [
                SNew(STextBlock).Text(LOCTEXT("LeftHandLabel", "Left Hand"))
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
            [
                SNew(SComboBox<FStringItem>)
                .OptionsSource(&HandPresetItems)
                .InitiallySelectedItem(SelectedLeftHandPreset)
                .OnGenerateWidget(this, &SMannyPoseToolkitEditorWidget::GenerateComboItem)
                .OnSelectionChanged_Lambda([this](FStringItem NewItem, ESelectInfo::Type)
                {
                    SelectedLeftHandPreset = NewItem;
                })
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return FText::FromString(SelectedLeftHandPreset.IsValid() ? *SelectedLeftHandPreset : TEXT("None"));
                    })
                ]
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f, 0.f, 8.f)
            [
                SNew(SButton)
                .Text(LOCTEXT("ApplyLeftHandPreset", "Apply Left Hand Preset"))
                .OnClicked(this, &SMannyPoseToolkitEditorWidget::OnApplyLeftHandPreset)
            ]

            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
            [
                SNew(STextBlock).Text(LOCTEXT("RightHandLabel", "Right Hand"))
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
            [
                SNew(SComboBox<FStringItem>)
                .OptionsSource(&HandPresetItems)
                .InitiallySelectedItem(SelectedRightHandPreset)
                .OnGenerateWidget(this, &SMannyPoseToolkitEditorWidget::GenerateComboItem)
                .OnSelectionChanged_Lambda([this](FStringItem NewItem, ESelectInfo::Type)
                {
                    SelectedRightHandPreset = NewItem;
                })
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return FText::FromString(SelectedRightHandPreset.IsValid() ? *SelectedRightHandPreset : TEXT("None"));
                    })
                ]
            ]
            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 4.f, 0.f, 8.f)
            [
                SNew(SButton)
                .Text(LOCTEXT("ApplyRightHandPreset", "Apply Right Hand Preset"))
                .OnClicked(this, &SMannyPoseToolkitEditorWidget::OnApplyRightHandPreset)
            ]

            + SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot().FillWidth(1.f).Padding(0.f, 0.f, 4.f, 0.f)
                [
                    SNew(SButton)
                    .Text(LOCTEXT("ApplyBothHandsPreset", "Apply Left Preset To Both Hands"))
                    .OnClicked(this, &SMannyPoseToolkitEditorWidget::OnApplyBothHandsPreset)
                ]
                + SHorizontalBox::Slot().FillWidth(1.f).Padding(4.f, 0.f, 0.f, 0.f)
                [
                    SNew(SButton)
                    .Text(LOCTEXT("ApplyHandPair", "Apply Left+Right Pair"))
                    .OnClicked(this, &SMannyPoseToolkitEditorWidget::OnApplyHandPair)
                ]
            ]
        ]
    ];

    RefreshBodyPreview();
}

FReply SMannyPoseToolkitEditorWidget::OnApplyBodyPreset()
{
    if (SelectedBodyPreset.IsValid())
    {
        ApplyBodyPresetFile(GetPluginPoseDir() / *SelectedBodyPreset + TEXT(".json"));
    }
    return FReply::Handled();
}

FReply SMannyPoseToolkitEditorWidget::OnSaveBodyPreset()
{
    if (SelectedBodyPreset.IsValid())
    {
        SaveBodyPresetFile(GetPluginPoseDir() / *SelectedBodyPreset + TEXT(".json"));
    }

    return FReply::Handled();
}

FReply SMannyPoseToolkitEditorWidget::OnApplyLeftHandPreset()
{
    if (SelectedLeftHandPreset.IsValid())
    {
        ApplyHandPresetFile(GetPluginHandDir() / *SelectedLeftHandPreset + TEXT(".json"), EMannyHandTargetSide::Left);
    }
    return FReply::Handled();
}

FReply SMannyPoseToolkitEditorWidget::OnApplyRightHandPreset()
{
    if (SelectedRightHandPreset.IsValid())
    {
        ApplyHandPresetFile(GetPluginHandDir() / *SelectedRightHandPreset + TEXT(".json"), EMannyHandTargetSide::Right);
    }
    return FReply::Handled();
}

FReply SMannyPoseToolkitEditorWidget::OnApplyBothHandsPreset()
{
    if (SelectedLeftHandPreset.IsValid())
    {
        ApplyHandPresetFile(GetPluginHandDir() / *SelectedLeftHandPreset + TEXT(".json"), EMannyHandTargetSide::BothSymmetric);
    }
    return FReply::Handled();
}

FReply SMannyPoseToolkitEditorWidget::OnApplyHandPair()
{
    if (SelectedLeftHandPreset.IsValid())
    {
        ApplyHandPresetFile(GetPluginHandDir() / *SelectedLeftHandPreset + TEXT(".json"), EMannyHandTargetSide::Left);
    }
    if (SelectedRightHandPreset.IsValid())
    {
        ApplyHandPresetFile(GetPluginHandDir() / *SelectedRightHandPreset + TEXT(".json"), EMannyHandTargetSide::Right);
    }
    return FReply::Handled();
}

bool SMannyPoseToolkitEditorWidget::ApplyBodyPresetFile(const FString& AbsoluteJsonPath)
{
    AMannyPosePreviewActor* PreviewActor = FindSelectedPreviewActor();
    if (!PreviewActor)
    {
        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NoPreviewActor", "Select an AMannyPosePreviewActor in the level first."));
        return false;
    }

    FMannyPoseData PoseData;
    FString Error;
    if (!UMannyPoseJsonLibrary::LoadPoseFromJsonFile(AbsoluteJsonPath, PoseData, Error))
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Failed to load body preset: %s"), *Error)));
        return false;
    }

    PreviewActor->Modify();
    PreviewActor->ApplyPoseData(PoseData);
    return true;
}

bool SMannyPoseToolkitEditorWidget::SaveBodyPresetFile(const FString& AbsoluteJsonPath)
{
    AMannyPosePreviewActor* PreviewActor = FindSelectedPreviewActor();
    if (!PreviewActor)
    {
        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NoPreviewActorSave", "Select an AMannyPosePreviewActor in the level first."));
        return false;
    }

    FMannyPoseData CapturedPose;
    FString Error;
    if (!CapturePoseFromPreviewActor(PreviewActor, CapturedPose, Error))
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Failed to capture body preset: %s"), *Error)));
        return false;
    }

    FMannyPoseData ExistingPose;
    FString LoadError;
    if (UMannyPoseJsonLibrary::LoadPoseFromJsonFile(AbsoluteJsonPath, ExistingPose, LoadError))
    {
        CapturedPose.Name = ExistingPose.Name;
        CapturedPose.Category = ExistingPose.Category;
        CapturedPose.Notes = ExistingPose.Notes;
        CapturedPose.LeftHandPreset = ExistingPose.LeftHandPreset;
        CapturedPose.RightHandPreset = ExistingPose.RightHandPreset;
        CapturedPose.IKControls = ExistingPose.IKControls;
        CapturedPose.FingerOffsets = ExistingPose.FingerOffsets;
    }
    else if (SelectedBodyPreset.IsValid())
    {
        CapturedPose.Name = *SelectedBodyPreset;
    }

    const FString JsonText = UMannyPoseJsonLibrary::ToPrettyJson(CapturedPose);
    if (!FFileHelper::SaveStringToFile(JsonText, *AbsoluteJsonPath))
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Failed to save body preset: %s"), *AbsoluteJsonPath)));
        return false;
    }

    RefreshBodyPreview();
    return true;
}

bool SMannyPoseToolkitEditorWidget::ApplyHandPresetFile(const FString& AbsoluteJsonPath, EMannyHandTargetSide TargetSide)
{
    AMannyPosePreviewActor* PreviewActor = FindSelectedPreviewActor();
    if (!PreviewActor)
    {
        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NoPreviewActorHands", "Select an AMannyPosePreviewActor in the level first."));
        return false;
    }

    FMannyHandPresetData PresetData;
    FString Error;
    if (!UMannyPoseJsonLibrary::LoadHandPresetFromJsonFile(AbsoluteJsonPath, PresetData, Error))
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Failed to load hand preset: %s"), *Error)));
        return false;
    }

    PreviewActor->Modify();
    PreviewActor->ApplyHandPreset(PresetData, TargetSide);
    return true;
}

AMannyPosePreviewActor* SMannyPoseToolkitEditorWidget::FindSelectedPreviewActor() const
{
    if (!GEditor)
    {
        return nullptr;
    }

    USelection* Selection = GEditor->GetSelectedActors();
    for (FSelectionIterator It(*Selection); It; ++It)
    {
        if (AMannyPosePreviewActor* Actor = Cast<AMannyPosePreviewActor>(*It))
        {
            return Actor;
        }
    }

    return nullptr;
}

FString SMannyPoseToolkitEditorWidget::GetPluginPoseDir() const
{
    return IPluginManager::Get().FindPlugin(TEXT("MannyPoseToolkit"))->GetBaseDir() / TEXT("Content/Poses/Bodies");
}

FString SMannyPoseToolkitEditorWidget::GetPluginHandDir() const
{
    return IPluginManager::Get().FindPlugin(TEXT("MannyPoseToolkit"))->GetBaseDir() / TEXT("Content/Poses/Hands");
}

void SMannyPoseToolkitEditorWidget::RebuildPresetOptions()
{
    BodyPresetItems.Reset();
    HandPresetItems.Reset();

    TArray<FString> BodyFiles;
    IFileManager::Get().FindFiles(BodyFiles, *(GetPluginPoseDir() / TEXT("*.json")), true, false);
    BodyFiles.Sort();
    for (const FString& File : BodyFiles)
    {
        BodyPresetItems.Add(MakeShared<FString>(MannyPoseToolkitEditorWidget::BasenameWithoutExtension(File)));
    }

    TArray<FString> HandFiles;
    IFileManager::Get().FindFiles(HandFiles, *(GetPluginHandDir() / TEXT("*.json")), true, false);
    HandFiles.Sort();
    for (const FString& File : HandFiles)
    {
        HandPresetItems.Add(MakeShared<FString>(MannyPoseToolkitEditorWidget::BasenameWithoutExtension(File)));
    }
}

TSharedRef<SWidget> SMannyPoseToolkitEditorWidget::GenerateComboItem(FStringItem Item) const
{
    return SNew(STextBlock)
        .Text(FText::FromString(Item.IsValid() ? *Item : TEXT("Invalid")));
}

void SMannyPoseToolkitEditorWidget::SetDefaultSelections()
{
    SelectedBodyPreset = BodyPresetItems.Num() > 0 ? BodyPresetItems[0] : nullptr;
    SelectedLeftHandPreset = HandPresetItems.Num() > 0 ? HandPresetItems[0] : nullptr;
    SelectedRightHandPreset = HandPresetItems.Num() > 1 ? HandPresetItems[1] : SelectedLeftHandPreset;
}

void SMannyPoseToolkitEditorWidget::RefreshBodyPreview()
{
    TArray<FBodyPreviewLineSegment> PreviewSegments;
    BodyPreviewStatusText = TEXT("Select a preview actor with a Manny skeletal mesh for an accurate line preview.");

    if (!SelectedBodyPreset.IsValid())
    {
        BodyPreviewStatusText = TEXT("No body preset selected.");
    }
    else
    {
        const FString AbsoluteJsonPath = GetPluginPoseDir() / *SelectedBodyPreset + TEXT(".json");
        FMannyPoseData LoadedPose;
        FString Error;
        if (UMannyPoseJsonLibrary::LoadPoseFromJsonFile(AbsoluteJsonPath, LoadedPose, Error))
        {
            if (AMannyPosePreviewActor* PreviewActor = FindSelectedPreviewActor())
            {
                if (BuildPreviewLineSegments(PreviewActor, LoadedPose, PreviewSegments))
                {
                    BodyPreviewStatusText = LoadedPose.Notes.IsEmpty()
                        ? TEXT("Uses the selected preview actor's actual skeleton. Apply to the actor for the final mesh result.")
                        : FString::Printf(TEXT("Uses the selected preview actor's actual skeleton. %s"), *LoadedPose.Notes);
                }
                else
                {
                    BodyPreviewStatusText = TEXT("Selected preview actor does not have a usable skeletal mesh for line preview.");
                }
            }
            else
            {
                BodyPreviewStatusText = TEXT("Select an AMannyPosePreviewActor for an accurate line preview.");
            }
        }
        else
        {
            BodyPreviewStatusText = FString::Printf(TEXT("Preview unavailable: %s"), *Error);
        }
    }

    if (BodyPreviewWidget.IsValid())
    {
        BodyPreviewWidget->SetPreviewSegments(PreviewSegments.IsEmpty() ? nullptr : &PreviewSegments);
    }
}

#undef LOCTEXT_NAMESPACE
