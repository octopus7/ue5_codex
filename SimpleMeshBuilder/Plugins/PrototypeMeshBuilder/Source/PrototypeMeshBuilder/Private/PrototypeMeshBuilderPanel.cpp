#include "PrototypeMeshBuilderPanel.h"

#include "PrototypeMeshBuilderController.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

void SPrototypeMeshBuilderPanel::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	ReasoningEffortOptions = {
		MakeShared<FString>(TEXT("medium")),
		MakeShared<FString>(TEXT("high")),
		MakeShared<FString>(TEXT("xhigh"))
	};

	ChildSlot
	[
		SNew(SBorder)
		.Padding(12.0f)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 8.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Codex Prototype Mesh Builder")))
					.Font(FAppStyle::GetFontStyle("HeadingMedium"))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Prompt")))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 12.0f))
				[
					SNew(SBox)
					.MinDesiredWidth(520.0f)
					[
						SAssignNew(PromptTextBox, SMultiLineEditableTextBox)
						.AutoWrapText(true)
						.Text(FText::FromString(Controller.IsValid() ? Controller->GetPrompt() : FString()))
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Asset Name")))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 12.0f))
				[
					SAssignNew(AssetNameTextBox, SEditableTextBox)
					.Text(FText::FromString(Controller.IsValid() ? Controller->GetAssetName() : FString()))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Reasoning Effort")))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 12.0f))
				[
					SAssignNew(ReasoningEffortComboBox, STextComboBox)
					.OptionsSource(&ReasoningEffortOptions)
					.InitiallySelectedItem(ReasoningEffortOptions[0])
					.OnSelectionChanged(this, &SPrototypeMeshBuilderPanel::HandleReasoningEffortChanged)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Content Path")))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 12.0f))
				[
					SAssignNew(ContentPathTextBox, SEditableTextBox)
					.Text(FText::FromString(Controller.IsValid() ? Controller->GetContentPath() : TEXT("/Game/Generated/PrototypeMeshes")))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 12.0f))
				[
					SNew(SUniformGridPanel)
					.SlotPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f))
					+ SUniformGridPanel::Slot(0, 0)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Generate")))
						.OnClicked(this, &SPrototypeMeshBuilderPanel::HandleGenerateClicked)
					]
					+ SUniformGridPanel::Slot(1, 0)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Save")))
						.IsEnabled_Lambda([this]() { return Controller.IsValid() && Controller->CanSave(); })
						.OnClicked(this, &SPrototypeMeshBuilderPanel::HandleSaveClicked)
					]
					+ SUniformGridPanel::Slot(2, 0)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Clear")))
						.OnClicked(this, &SPrototypeMeshBuilderPanel::HandleClearClicked)
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 12.0f))
				[
					SNew(SSeparator)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.AutoWrapText(true)
					.Text_Lambda([this]()
					{
						return Controller.IsValid() ? Controller->GetStatusText() : FText::FromString(TEXT("Controller unavailable."));
					})
				]
			]
		]
	];

	RefreshWidgetFields();
}

FReply SPrototypeMeshBuilderPanel::HandleGenerateClicked()
{
	SyncControllerFromWidgets();
	if (Controller.IsValid())
	{
		Controller->Generate();
		RefreshWidgetFields();
	}

	return FReply::Handled();
}

FReply SPrototypeMeshBuilderPanel::HandleSaveClicked()
{
	SyncControllerFromWidgets();
	if (Controller.IsValid())
	{
		Controller->Save();
		RefreshWidgetFields();
	}

	return FReply::Handled();
}

FReply SPrototypeMeshBuilderPanel::HandleClearClicked()
{
	if (Controller.IsValid())
	{
		Controller->Clear();
		RefreshWidgetFields();
	}

	return FReply::Handled();
}

void SPrototypeMeshBuilderPanel::HandleReasoningEffortChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (Controller.IsValid() && NewSelection.IsValid())
	{
		Controller->SetReasoningEffort(*NewSelection);
	}
}

void SPrototypeMeshBuilderPanel::SyncControllerFromWidgets() const
{
	if (!Controller.IsValid())
	{
		return;
	}

	Controller->SetPrompt(PromptTextBox.IsValid() ? PromptTextBox->GetText().ToString() : FString());
	Controller->SetAssetName(AssetNameTextBox.IsValid() ? AssetNameTextBox->GetText().ToString() : FString());
	Controller->SetContentPath(ContentPathTextBox.IsValid() ? ContentPathTextBox->GetText().ToString() : FString());
	if (ReasoningEffortComboBox.IsValid() && ReasoningEffortComboBox->GetSelectedItem().IsValid())
	{
		Controller->SetReasoningEffort(*ReasoningEffortComboBox->GetSelectedItem());
	}
}

void SPrototypeMeshBuilderPanel::RefreshWidgetFields() const
{
	if (!Controller.IsValid())
	{
		return;
	}

	if (AssetNameTextBox.IsValid())
	{
		AssetNameTextBox->SetText(FText::FromString(Controller->GetAssetName()));
	}

	if (ContentPathTextBox.IsValid())
	{
		ContentPathTextBox->SetText(FText::FromString(Controller->GetContentPath()));
	}

	if (ReasoningEffortComboBox.IsValid())
	{
		for (const TSharedPtr<FString>& Option : ReasoningEffortOptions)
		{
			if (Option.IsValid() && *Option == Controller->GetReasoningEffort())
			{
				ReasoningEffortComboBox->SetSelectedItem(Option);
				break;
			}
		}
	}
}
