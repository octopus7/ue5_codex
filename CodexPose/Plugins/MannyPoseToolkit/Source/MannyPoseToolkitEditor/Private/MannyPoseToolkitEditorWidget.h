#pragma once

#include "Widgets/SCompoundWidget.h"
#include "MannyPoseTypes.h"

class SMannyPoseBodyPreview;

class SMannyPoseToolkitEditorWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SMannyPoseToolkitEditorWidget) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    using FStringItem = TSharedPtr<FString>;

    FReply OnApplyBodyPreset();
    FReply OnSaveBodyPreset();
    FReply OnApplyLeftHandPreset();
    FReply OnApplyRightHandPreset();
    FReply OnApplyBothHandsPreset();
    FReply OnApplyHandPair();

    bool ApplyBodyPresetFile(const FString& AbsoluteJsonPath);
    bool SaveBodyPresetFile(const FString& AbsoluteJsonPath);
    bool ApplyHandPresetFile(const FString& AbsoluteJsonPath, EMannyHandTargetSide TargetSide);
    class AMannyPosePreviewActor* FindSelectedPreviewActor() const;
    FString GetPluginPoseDir() const;
    FString GetPluginHandDir() const;
    void RebuildPresetOptions();
    TSharedRef<class SWidget> GenerateComboItem(FStringItem Item) const;
    void SetDefaultSelections();
    void RefreshBodyPreview();

    TArray<FStringItem> BodyPresetItems;
    TArray<FStringItem> HandPresetItems;

    FStringItem SelectedBodyPreset;
    FStringItem SelectedLeftHandPreset;
    FStringItem SelectedRightHandPreset;

    TSharedPtr<SMannyPoseBodyPreview> BodyPreviewWidget;
    FString BodyPreviewStatusText;
};
