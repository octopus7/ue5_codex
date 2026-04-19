#include "Misc/MessageDialog.h"
#include "Misc/Paths.h"
#include "MannyPoseToolkitEditorWidget.h"

#include "Interfaces/IPluginManager.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "HAL/FileManager.h"
#include "MannyPoseJsonLibrary.h"
#include "MannyPosePreviewActor.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SBorder.h"
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
                })
                [
                    SNew(STextBlock)
                    .Text_Lambda([this]()
                    {
                        return FText::FromString(SelectedBodyPreset.IsValid() ? *SelectedBodyPreset : TEXT("None"));
                    })
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

#undef LOCTEXT_NAMESPACE
