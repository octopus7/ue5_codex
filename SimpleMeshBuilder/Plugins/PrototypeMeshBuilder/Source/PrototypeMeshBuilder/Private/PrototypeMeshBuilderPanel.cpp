#include "PrototypeMeshBuilderPanel.h"

#include "PrototypeMeshBuilderController.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

void SPrototypeMeshBuilderPanel::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	GenerationModeOptions = {
		MakeShared<FString>(TEXT("primitive")),
		MakeShared<FString>(TEXT("voxel"))
	};
	ReasoningEffortOptions = {
		MakeShared<FString>(TEXT("medium")),
		MakeShared<FString>(TEXT("high")),
		MakeShared<FString>(TEXT("xhigh"))
	};
	VoxelResolutionOptions = {
		MakeShared<FString>(TEXT("16")),
		MakeShared<FString>(TEXT("32")),
		MakeShared<FString>(TEXT("64")),
		MakeShared<FString>(TEXT("128")),
		MakeShared<FString>(TEXT("256"))
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
					.Text(FText::FromString(TEXT("Generation Mode")))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 12.0f))
				[
					SAssignNew(GenerationModeComboBox, STextComboBox)
					.OptionsSource(&GenerationModeOptions)
					.InitiallySelectedItem(GenerationModeOptions[0])
					.OnSelectionChanged(this, &SPrototypeMeshBuilderPanel::HandleGenerationModeChanged)
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
					.Text(FText::FromString(TEXT("Voxel Resolution")))
					.Visibility_Lambda([this]()
					{
						return Controller.IsValid() && Controller->GetGenerationMode() == EPrototypeGenerationMode::Voxel
							? EVisibility::Visible
							: EVisibility::Collapsed;
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 12.0f))
				[
					SAssignNew(VoxelResolutionComboBox, STextComboBox)
					.OptionsSource(&VoxelResolutionOptions)
					.InitiallySelectedItem(VoxelResolutionOptions[1])
					.OnSelectionChanged(this, &SPrototypeMeshBuilderPanel::HandleVoxelResolutionChanged)
					.Visibility_Lambda([this]()
					{
						return Controller.IsValid() && Controller->GetGenerationMode() == EPrototypeGenerationMode::Voxel
							? EVisibility::Visible
							: EVisibility::Collapsed;
					})
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
					SAssignNew(UseSharedMaterialCheckBox, SCheckBox)
					.IsChecked(Controller.IsValid() && Controller->GetUseSharedMaterial() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Use Shared Lit Vertex Color Material")))
					]
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
						.Text(FText::FromString(TEXT("Delete")))
						.IsEnabled_Lambda([this]() { return Controller.IsValid() && Controller->CanDeleteSelectedPreview(); })
						.OnClicked(this, &SPrototypeMeshBuilderPanel::HandleDeleteClicked)
					]
					+ SUniformGridPanel::Slot(3, 0)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Clear All")))
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
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Selected Actor")))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 12.0f))
				[
					SNew(STextBlock)
					.AutoWrapText(true)
					.Text_Lambda([this]()
					{
						return Controller.IsValid() ? Controller->GetSelectedActorText() : FText::FromString(TEXT("Controller unavailable."));
					})
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 4.0f))
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("Queued / Running Jobs")))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.0f, 0.0f, 0.0f, 12.0f))
				[
					SNew(SBox)
					.HeightOverride(120.0f)
					[
						SAssignNew(JobListView, SListView<TSharedPtr<FString>>)
						.ListItemsSource(Controller.IsValid() ? &Controller->GetJobDisplayItems() : nullptr)
						.OnGenerateRow(this, &SPrototypeMeshBuilderPanel::HandleGenerateJobRow)
						.SelectionMode(ESelectionMode::None)
					]
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

	RegisterActiveTimer(0.25f, FWidgetActiveTimerDelegate::CreateSP(this, &SPrototypeMeshBuilderPanel::HandleActiveTimer));
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

FReply SPrototypeMeshBuilderPanel::HandleDeleteClicked()
{
	if (Controller.IsValid())
	{
		Controller->DeleteSelectedPreview();
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

void SPrototypeMeshBuilderPanel::HandleGenerationModeChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (Controller.IsValid() && NewSelection.IsValid())
	{
		Controller->SetGenerationMode(PrototypeGenerationModeFromString(*NewSelection));
	}
}

void SPrototypeMeshBuilderPanel::HandleVoxelResolutionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (Controller.IsValid() && NewSelection.IsValid())
	{
		Controller->SetVoxelResolution(FCString::Atoi(**NewSelection));
	}
}

TSharedRef<ITableRow> SPrototypeMeshBuilderPanel::HandleGenerateJobRow(TSharedPtr<FString> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
	[
		SNew(STextBlock)
		.AutoWrapText(true)
		.Text(FText::FromString(Item.IsValid() ? *Item : TEXT("")))
	];
}

EActiveTimerReturnType SPrototypeMeshBuilderPanel::HandleActiveTimer(double CurrentTime, float DeltaTime)
{
	if (JobListView.IsValid())
	{
		JobListView->RequestListRefresh();
	}

	return EActiveTimerReturnType::Continue;
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
	Controller->SetUseSharedMaterial(UseSharedMaterialCheckBox.IsValid() && UseSharedMaterialCheckBox->GetCheckedState() == ECheckBoxState::Checked);
	if (GenerationModeComboBox.IsValid() && GenerationModeComboBox->GetSelectedItem().IsValid())
	{
		Controller->SetGenerationMode(PrototypeGenerationModeFromString(*GenerationModeComboBox->GetSelectedItem()));
	}
	if (ReasoningEffortComboBox.IsValid() && ReasoningEffortComboBox->GetSelectedItem().IsValid())
	{
		Controller->SetReasoningEffort(*ReasoningEffortComboBox->GetSelectedItem());
	}
	if (VoxelResolutionComboBox.IsValid() && VoxelResolutionComboBox->GetSelectedItem().IsValid())
	{
		Controller->SetVoxelResolution(FCString::Atoi(**VoxelResolutionComboBox->GetSelectedItem()));
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

	if (UseSharedMaterialCheckBox.IsValid())
	{
		UseSharedMaterialCheckBox->SetIsChecked(Controller->GetUseSharedMaterial() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}

	if (GenerationModeComboBox.IsValid())
	{
		const FString SelectedMode = PrototypeGenerationModeToString(Controller->GetGenerationMode());
		for (const TSharedPtr<FString>& Option : GenerationModeOptions)
		{
			if (Option.IsValid() && *Option == SelectedMode)
			{
				GenerationModeComboBox->SetSelectedItem(Option);
				break;
			}
		}
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

	if (VoxelResolutionComboBox.IsValid())
	{
		const FString ResolutionText = FString::FromInt(Controller->GetVoxelResolution());
		for (const TSharedPtr<FString>& Option : VoxelResolutionOptions)
		{
			if (Option.IsValid() && *Option == ResolutionText)
			{
				VoxelResolutionComboBox->SetSelectedItem(Option);
				break;
			}
		}
	}
}
