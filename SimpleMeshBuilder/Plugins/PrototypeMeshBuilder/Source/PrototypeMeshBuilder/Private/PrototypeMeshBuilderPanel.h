#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SListView.h"

class FPrototypeMeshBuilderController;
class SEditableTextBox;
class SMultiLineEditableTextBox;
class STextComboBox;

class SPrototypeMeshBuilderPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPrototypeMeshBuilderPanel) {}
		SLATE_ARGUMENT(TSharedPtr<FPrototypeMeshBuilderController>, Controller)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FReply HandleGenerateClicked();
	FReply HandleSaveClicked();
	FReply HandleClearClicked();
	void HandleReasoningEffortChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
	TSharedRef<ITableRow> HandleGenerateJobRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable);
	EActiveTimerReturnType HandleActiveTimer(double CurrentTime, float DeltaTime);
	void SyncControllerFromWidgets() const;
	void RefreshWidgetFields() const;

private:
	TSharedPtr<FPrototypeMeshBuilderController> Controller;
	TArray<TSharedPtr<FString>> ReasoningEffortOptions;
	TSharedPtr<SMultiLineEditableTextBox> PromptTextBox;
	TSharedPtr<SEditableTextBox> AssetNameTextBox;
	TSharedPtr<SEditableTextBox> ContentPathTextBox;
	TSharedPtr<STextComboBox> ReasoningEffortComboBox;
	TSharedPtr<SListView<TSharedPtr<FString>>> JobListView;
};
