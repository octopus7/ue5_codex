#include "MannyPoseToolkitEditorWidget.h"

#include "Misc/MessageDialog.h"
#include "Misc/Paths.h"
#include "Interfaces/IPluginManager.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "HAL/FileManager.h"
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
        FName Parent;
        FVector ReferenceOffset;
        FLinearColor Color;
        float Thickness;
    };

    const TArray<FBodyPreviewBoneDefinition>& GetBodyPreviewBones()
    {
        static const TArray<FBodyPreviewBoneDefinition> Bones =
        {
            { TEXT("pelvis"), NAME_None, FVector::ZeroVector, FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },

            { TEXT("spine_01"), TEXT("pelvis"), FVector(0.f, 0.f, 15.f), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("spine_02"), TEXT("spine_01"), FVector(0.f, 0.f, 15.f), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("spine_03"), TEXT("spine_02"), FVector(0.f, 0.f, 15.f), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("neck_01"), TEXT("spine_03"), FVector(0.f, 0.f, 10.f), FLinearColor(0.80f, 0.84f, 0.90f), 2.5f },
            { TEXT("head"), TEXT("neck_01"), FVector(0.f, 0.f, 12.f), FLinearColor(0.80f, 0.84f, 0.90f), 3.0f },

            { TEXT("clavicle_l"), TEXT("spine_03"), FVector(0.f, -9.f, 3.f), FLinearColor(0.35f, 0.80f, 1.00f), 2.0f },
            { TEXT("upperarm_l"), TEXT("clavicle_l"), FVector(0.f, -16.f, -5.f), FLinearColor(0.35f, 0.80f, 1.00f), 2.0f },
            { TEXT("lowerarm_l"), TEXT("upperarm_l"), FVector(0.f, -18.f, -1.f), FLinearColor(0.35f, 0.80f, 1.00f), 2.0f },
            { TEXT("hand_l"), TEXT("lowerarm_l"), FVector(0.f, -10.f, 0.f), FLinearColor(0.35f, 0.80f, 1.00f), 2.0f },

            { TEXT("clavicle_r"), TEXT("spine_03"), FVector(0.f, 9.f, 3.f), FLinearColor(1.00f, 0.72f, 0.35f), 2.0f },
            { TEXT("upperarm_r"), TEXT("clavicle_r"), FVector(0.f, 16.f, -5.f), FLinearColor(1.00f, 0.72f, 0.35f), 2.0f },
            { TEXT("lowerarm_r"), TEXT("upperarm_r"), FVector(0.f, 18.f, -1.f), FLinearColor(1.00f, 0.72f, 0.35f), 2.0f },
            { TEXT("hand_r"), TEXT("lowerarm_r"), FVector(0.f, 10.f, 0.f), FLinearColor(1.00f, 0.72f, 0.35f), 2.0f },

            { TEXT("thigh_l"), TEXT("pelvis"), FVector(2.f, -8.f, -24.f), FLinearColor(0.35f, 0.80f, 1.00f), 2.5f },
            { TEXT("calf_l"), TEXT("thigh_l"), FVector(0.f, 0.f, -25.f), FLinearColor(0.35f, 0.80f, 1.00f), 2.5f },
            { TEXT("foot_l"), TEXT("calf_l"), FVector(12.f, 0.f, -3.f), FLinearColor(0.35f, 0.80f, 1.00f), 2.5f },

            { TEXT("thigh_r"), TEXT("pelvis"), FVector(2.f, 8.f, -24.f), FLinearColor(1.00f, 0.72f, 0.35f), 2.5f },
            { TEXT("calf_r"), TEXT("thigh_r"), FVector(0.f, 0.f, -25.f), FLinearColor(1.00f, 0.72f, 0.35f), 2.5f },
            { TEXT("foot_r"), TEXT("calf_r"), FVector(12.f, 0.f, -3.f), FLinearColor(1.00f, 0.72f, 0.35f), 2.5f }
        };

        return Bones;
    }

    const FMannyPoseBoneRotation* FindPoseBone(const FMannyPoseData& PoseData, const FName BoneName)
    {
        return PoseData.FKBones.Find(BoneName);
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

    bool BuildPreviewWorldTransforms(const FMannyPoseData& PoseData, TMap<FName, FTransform>& OutWorldTransforms)
    {
        OutWorldTransforms.Reset();

        for (const FBodyPreviewBoneDefinition& Bone : GetBodyPreviewBones())
        {
            const FMannyPoseBoneRotation* PoseBone = FindPoseBone(PoseData, Bone.Name);
            const FVector LocalLocation = Bone.ReferenceOffset + (PoseBone ? PoseBone->Location : FVector::ZeroVector);
            const FQuat LocalRotation = FQuat(PoseBone ? PoseBone->Rotation : FRotator::ZeroRotator);

            if (Bone.Parent == NAME_None)
            {
                OutWorldTransforms.Add(Bone.Name, FTransform(LocalRotation, LocalLocation));
                continue;
            }

            const FTransform* ParentTransform = OutWorldTransforms.Find(Bone.Parent);
            if (!ParentTransform)
            {
                continue;
            }

            const FVector WorldLocation = ParentTransform->GetLocation() + ParentTransform->GetRotation().RotateVector(LocalLocation);
            const FQuat WorldRotation = (ParentTransform->GetRotation() * LocalRotation).GetNormalized();
            OutWorldTransforms.Add(Bone.Name, FTransform(WorldRotation, WorldLocation));
        }

        return OutWorldTransforms.Num() > 1;
    }

    bool BuildPreviewScreenPoints(const FMannyPoseData& PoseData, const FVector2D& ViewportSize, TMap<FName, FVector2D>& OutScreenPoints)
    {
        TMap<FName, FTransform> WorldTransforms;
        if (!BuildPreviewWorldTransforms(PoseData, WorldTransforms))
        {
            OutScreenPoints.Reset();
            return false;
        }

        FVector2D MinPoint(FLT_MAX, FLT_MAX);
        FVector2D MaxPoint(-FLT_MAX, -FLT_MAX);
        TMap<FName, FVector2D> ProjectedPoints;

        for (const TPair<FName, FTransform>& Pair : WorldTransforms)
        {
            const FVector2D Projected = ProjectPreviewPoint(Pair.Value.GetLocation());
            ProjectedPoints.Add(Pair.Key, Projected);

            MinPoint.X = FMath::Min(MinPoint.X, Projected.X);
            MinPoint.Y = FMath::Min(MinPoint.Y, Projected.Y);
            MaxPoint.X = FMath::Max(MaxPoint.X, Projected.X);
            MaxPoint.Y = FMath::Max(MaxPoint.Y, Projected.Y);
        }

        const FMargin Padding(14.f);
        const FVector2D AvailableSize(
            FMath::Max(ViewportSize.X - Padding.GetTotalSpaceAlong<Orient_Horizontal>(), 1.f),
            FMath::Max(ViewportSize.Y - Padding.GetTotalSpaceAlong<Orient_Vertical>(), 1.f));
        const FVector2D BoundsSize = MaxPoint - MinPoint;
        const float ScaleX = AvailableSize.X / FMath::Max(BoundsSize.X, 1.f);
        const float ScaleY = AvailableSize.Y / FMath::Max(BoundsSize.Y, 1.f);
        const float Scale = FMath::Min(ScaleX, ScaleY);
        const float ExtraX = (AvailableSize.X - BoundsSize.X * Scale) * 0.5f;
        const float BaselineY = ViewportSize.Y - Padding.Bottom;

        OutScreenPoints.Reset();
        for (const TPair<FName, FVector2D>& Pair : ProjectedPoints)
        {
            OutScreenPoints.Add(
                Pair.Key,
                FVector2D(
                    Padding.Left + ExtraX + (Pair.Value.X - MinPoint.X) * Scale,
                    BaselineY - (Pair.Value.Y - MinPoint.Y) * Scale));
        }

        return true;
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

    void SetPoseData(const FMannyPoseData* InPoseData)
    {
        bHasPoseData = InPoseData != nullptr;
        PoseData = InPoseData ? *InPoseData : FMannyPoseData();
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

        if (!bHasPoseData)
        {
            return LayerId + 1;
        }

        TMap<FName, FVector2D> ScreenPoints;
        if (!BuildPreviewScreenPoints(PoseData, Size, ScreenPoints))
        {
            return LayerId + 1;
        }

        for (const FBodyPreviewBoneDefinition& Bone : GetBodyPreviewBones())
        {
            if (Bone.Parent == NAME_None)
            {
                continue;
            }

            const FVector2D* StartPoint = ScreenPoints.Find(Bone.Parent);
            const FVector2D* EndPoint = ScreenPoints.Find(Bone.Name);
            if (!StartPoint || !EndPoint)
            {
                continue;
            }

            const TArray<FVector2f> LinePoints =
            {
                FVector2f(static_cast<float>(StartPoint->X), static_cast<float>(StartPoint->Y)),
                FVector2f(static_cast<float>(EndPoint->X), static_cast<float>(EndPoint->Y))
            };

            FSlateDrawElement::MakeLines(
                OutDrawElements,
                LayerId + 2,
                AllottedGeometry.ToPaintGeometry(),
                LinePoints,
                ESlateDrawEffect::None,
                Bone.Color,
                true,
                Bone.Thickness);
        }

        return LayerId + 2;
    }

private:
    FMannyPoseData PoseData;
    bool bHasPoseData = false;
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
                SNew(SButton)
                .Text(LOCTEXT("ApplyBodyPreset", "Apply Body Preset"))
                .OnClicked(this, &SMannyPoseToolkitEditorWidget::OnApplyBodyPreset)
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
    bHasBodyPreviewPose = false;
    BodyPreviewStatusText = TEXT("Approximate body-only line preview.");

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
            BodyPreviewPose = MoveTemp(LoadedPose);
            bHasBodyPreviewPose = true;

            BodyPreviewStatusText = BodyPreviewPose.Notes.IsEmpty()
                ? TEXT("Approximate body-only line preview. Apply to the actor for the exact Manny pose.")
                : FString::Printf(TEXT("Approximate body-only line preview. %s"), *BodyPreviewPose.Notes);
        }
        else
        {
            BodyPreviewStatusText = FString::Printf(TEXT("Preview unavailable: %s"), *Error);
        }
    }

    if (BodyPreviewWidget.IsValid())
    {
        BodyPreviewWidget->SetPoseData(bHasBodyPreviewPose ? &BodyPreviewPose : nullptr);
    }
}

#undef LOCTEXT_NAMESPACE
