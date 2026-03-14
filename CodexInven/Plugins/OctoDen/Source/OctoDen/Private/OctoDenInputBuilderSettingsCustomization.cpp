#include "OctoDenInputBuilderSettingsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IPropertyUtilities.h"
#include "Modules/ModuleManager.h"
#include "OctoDenInputBuilderSettings.h"
#include "OctoDenModule.h"
#include "PropertyHandle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "OctoDenInputBuilderSettingsCustomization"

namespace
{
	EVisibility GetSetupRowsVisibility(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings)
	{
		return (InSettings.IsValid() && InSettings->HasSelectedInputMappingContext()) ? EVisibility::Visible : EVisibility::Collapsed;
	}

	EVisibility GetAddSectionVisibility(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings)
	{
		if (const UOctoDenInputBuilderSettings* Settings = InSettings.Get())
		{
			const FOctoDenManagedInputAnalysis Analysis = Settings->AnalyzeSelectedInputMappingContext();
			return Analysis.bHasSelectedInputMappingContext && Analysis.HasAvailableActions() ? EVisibility::Visible : EVisibility::Collapsed;
		}

		return EVisibility::Collapsed;
	}

	EVisibility GetCreateDefaultImcVisibility(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings)
	{
		return (InSettings.IsValid() && !InSettings->HasSelectedInputMappingContext()) ? EVisibility::Visible : EVisibility::Collapsed;
	}

	EVisibility GetAllAddedHintVisibility(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings)
	{
		if (const UOctoDenInputBuilderSettings* Settings = InSettings.Get())
		{
			const FOctoDenManagedInputAnalysis Analysis = Settings->AnalyzeSelectedInputMappingContext();
			return Analysis.bHasSelectedInputMappingContext && !Analysis.HasAvailableActions() ? EVisibility::Visible : EVisibility::Collapsed;
		}

		return EVisibility::Collapsed;
	}

	EVisibility GetPresetSummaryVisibility(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings, const EOctoDenStandardInputAction InAction)
	{
		if (const UOctoDenInputBuilderSettings* Settings = InSettings.Get())
		{
			EOctoDenStandardInputAction ResolvedAction = EOctoDenStandardInputAction::Move;
			if (Settings->ResolveSelectedAction(ResolvedAction) && ResolvedAction == InAction && UOctoDenInputBuilderSettings::UsesPresetBindings(ResolvedAction))
			{
				return EVisibility::Visible;
			}
		}

		return EVisibility::Collapsed;
	}

	EVisibility GetBindingVisibility(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings, const EOctoDenStandardInputAction InAction)
	{
		if (const UOctoDenInputBuilderSettings* Settings = InSettings.Get())
		{
			EOctoDenStandardInputAction ResolvedAction = EOctoDenStandardInputAction::Move;
			if (Settings->ResolveSelectedAction(ResolvedAction) && ResolvedAction == InAction && !UOctoDenInputBuilderSettings::UsesPresetBindings(ResolvedAction))
			{
				return EVisibility::Visible;
			}
		}

		return EVisibility::Collapsed;
	}

	FText GetSelectedActionLabel(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings)
	{
		if (const UOctoDenInputBuilderSettings* Settings = InSettings.Get())
		{
			EOctoDenStandardInputAction ResolvedAction = EOctoDenStandardInputAction::Move;
			if (Settings->ResolveSelectedAction(ResolvedAction))
			{
				return UOctoDenInputBuilderSettings::GetStandardActionDisplayText(ResolvedAction);
			}

			if (Settings->HasSelectedInputMappingContext())
			{
				return LOCTEXT("NoAvailableActionsLabel", "No Actions Available");
			}
		}

		return LOCTEXT("SelectImcFirstLabel", "Select IMC First");
	}

	FText GetAddButtonToolTip(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings)
	{
		if (const UOctoDenInputBuilderSettings* Settings = InSettings.Get())
		{
			FText FailReason;
			if (Settings->CanAddSelectedAction(&FailReason))
			{
				return LOCTEXT("AddInputTooltip", "Create or repair the selected managed input.");
			}

			return FailReason;
		}

		return FText::GetEmpty();
	}

	FText GetStatusText(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings, const EOctoDenStandardInputAction InAction)
	{
		if (const UOctoDenInputBuilderSettings* Settings = InSettings.Get())
		{
			const FOctoDenManagedInputAnalysis Analysis = Settings->AnalyzeSelectedInputMappingContext();
			if (!Analysis.bHasSelectedInputMappingContext)
			{
				return LOCTEXT("StatusSelectImc", "Select an IMC");
			}

			if (const FOctoDenManagedInputActionState* State = Analysis.FindActionState(InAction))
			{
				if (State->IsAdded())
				{
					return FText::Format(
						LOCTEXT("StatusAdded", "Added ({0} mapping(s))"),
						FText::AsNumber(State->ValidMappingCount));
				}
			}

			return LOCTEXT("StatusAvailable", "Available");
		}

		return FText::GetEmpty();
	}

	FSlateColor GetStatusColor(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings, const EOctoDenStandardInputAction InAction)
	{
		if (const UOctoDenInputBuilderSettings* Settings = InSettings.Get())
		{
			const FOctoDenManagedInputAnalysis Analysis = Settings->AnalyzeSelectedInputMappingContext();
			if (!Analysis.bHasSelectedInputMappingContext)
			{
				return FSlateColor::UseForeground();
			}

			if (const FOctoDenManagedInputActionState* State = Analysis.FindActionState(InAction))
			{
				return State->IsAdded()
					? FSlateColor(FLinearColor(0.25f, 0.65f, 0.30f, 1.0f))
					: FSlateColor(FLinearColor(0.75f, 0.65f, 0.20f, 1.0f));
			}
		}

		return FSlateColor::UseForeground();
	}

	FText GetNullMappingNotice(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings)
	{
		if (const UOctoDenInputBuilderSettings* Settings = InSettings.Get())
		{
			const FOctoDenManagedInputAnalysis Analysis = Settings->AnalyzeSelectedInputMappingContext();
			if (Analysis.NullActionMappingCount > 0)
			{
				return FText::Format(
					LOCTEXT("NullMappingNotice", "Detected {0} broken IMC mapping(s) with no IA. They will be cleaned the next time you add a managed input."),
					FText::AsNumber(Analysis.NullActionMappingCount));
			}
		}

		return FText::GetEmpty();
	}

	EVisibility GetNullMappingNoticeVisibility(const TWeakObjectPtr<UOctoDenInputBuilderSettings> InSettings)
	{
		if (const UOctoDenInputBuilderSettings* Settings = InSettings.Get())
		{
			return Settings->AnalyzeSelectedInputMappingContext().NullActionMappingCount > 0 ? EVisibility::Visible : EVisibility::Collapsed;
		}

		return EVisibility::Collapsed;
	}
}

TSharedRef<IDetailCustomization> FOctoDenInputBuilderSettingsCustomization::MakeInstance()
{
	return MakeShared<FOctoDenInputBuilderSettingsCustomization>();
}

void FOctoDenInputBuilderSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TSharedRef<IPropertyHandle> SelectedImcProperty = DetailBuilder.GetProperty(
		GET_MEMBER_NAME_CHECKED(UOctoDenInputBuilderSettings, SelectedInputMappingContext));
	TSharedRef<IPropertyHandle> SelectedActionProperty = DetailBuilder.GetProperty(
		GET_MEMBER_NAME_CHECKED(UOctoDenInputBuilderSettings, SelectedAction));
	TSharedRef<IPropertyHandle> InputActionPrefixProperty = DetailBuilder.GetProperty(
		GET_MEMBER_NAME_CHECKED(UOctoDenInputBuilderSettings, InputActionPrefix));
	TSharedRef<IPropertyHandle> InputActionFolderProperty = DetailBuilder.GetProperty(
		GET_MEMBER_NAME_CHECKED(UOctoDenInputBuilderSettings, InputActionFolder));
	TSharedRef<IPropertyHandle> JumpBindingsProperty = DetailBuilder.GetProperty(
		GET_MEMBER_NAME_CHECKED(UOctoDenInputBuilderSettings, JumpBindings));
	TSharedRef<IPropertyHandle> FireBindingsProperty = DetailBuilder.GetProperty(
		GET_MEMBER_NAME_CHECKED(UOctoDenInputBuilderSettings, FireBindings));
	const TSharedPtr<IPropertyUtilities> PropertyUtilities = DetailBuilder.GetPropertyUtilities();

	DetailBuilder.HideProperty(SelectedImcProperty);
	DetailBuilder.HideProperty(SelectedActionProperty);
	DetailBuilder.HideProperty(InputActionPrefixProperty);
	DetailBuilder.HideProperty(InputActionFolderProperty);
	DetailBuilder.HideProperty(JumpBindingsProperty);
	DetailBuilder.HideProperty(FireBindingsProperty);

	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

	TWeakObjectPtr<UOctoDenInputBuilderSettings> DialogSettings;
	for (const TWeakObjectPtr<UObject>& Object : CustomizedObjects)
	{
		if (UOctoDenInputBuilderSettings* Settings = Cast<UOctoDenInputBuilderSettings>(Object.Get()))
		{
			DialogSettings = Settings;
			break;
		}
	}

	IDetailCategoryBuilder& BuilderCategory = DetailBuilder.EditCategory(TEXT("Input Builder"));
	BuilderCategory.AddCustomRow(LOCTEXT("CreateDefaultImcFilter", "Create Default IMC"))
	.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings]()
	{
		return GetCreateDefaultImcVisibility(DialogSettings);
	}))
	.WholeRowContent()
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 0.0f, 0.0f, 8.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("CreateDefaultImcHint", "No IMC is selected. Create and select a default IMC at /Game/Input/Contexts/IMC_Default."))
			.Font(IDetailLayoutBuilder::GetDetailFontItalic())
			.AutoWrapText(true)
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBox)
			.WidthOverride(180.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("CreateDefaultImcButton", "Create Default IMC"))
				.OnClicked_Lambda([DialogSettings]()
				{
					if (UOctoDenInputBuilderSettings* Settings = DialogSettings.Get())
					{
						FOctoDenModule& Module = FModuleManager::LoadModuleChecked<FOctoDenModule>(TEXT("OctoDen"));
						Module.CreateDefaultInputMappingContext(Settings);
					}

					return FReply::Handled();
				})
			]
		]
	];

	BuilderCategory.AddCustomRow(LOCTEXT("TargetImcFilter", "Target IMC"))
	.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings]()
	{
		return GetSetupRowsVisibility(DialogSettings);
	}))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("TargetImcLabel", "Target IMC"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		.ToolTipText(LOCTEXT("TargetImcTooltip", "Select an existing Input Mapping Context asset to extend."))
	]
	.ValueContent()
	.MinDesiredWidth(420.0f)
	.MaxDesiredWidth(420.0f)
	[
		SelectedImcProperty->CreatePropertyValueWidget()
	];

	BuilderCategory.AddCustomRow(LOCTEXT("IaPrefixFilter", "Action Prefix"))
	.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings]()
	{
		return GetSetupRowsVisibility(DialogSettings);
	}))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("IaPrefixLabel", "Action Prefix"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	.MinDesiredWidth(320.0f)
	.MaxDesiredWidth(420.0f)
	[
		InputActionPrefixProperty->CreatePropertyValueWidget()
	];

	BuilderCategory.AddCustomRow(LOCTEXT("IaFolderFilter", "Action Folder"))
	.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings]()
	{
		return GetSetupRowsVisibility(DialogSettings);
	}))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("IaFolderLabel", "Action Folder"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	.MinDesiredWidth(320.0f)
	.MaxDesiredWidth(420.0f)
	[
		InputActionFolderProperty->CreatePropertyValueWidget()
	];

	BuilderCategory.AddCustomRow(LOCTEXT("ActionSelectionFilter", "Managed Action"))
	.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings]()
	{
		return GetAddSectionVisibility(DialogSettings);
	}))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("ManagedActionLabel", "Managed Action"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
	]
	.ValueContent()
	.MinDesiredWidth(420.0f)
	.MaxDesiredWidth(420.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.Padding(0.0f, 0.0f, 8.0f, 0.0f)
		[
			SNew(SComboButton)
			.OnGetMenuContent_Lambda([DialogSettings, SelectedActionProperty, PropertyUtilities]()
			{
				FMenuBuilder MenuBuilder(true, nullptr);
				if (UOctoDenInputBuilderSettings* Settings = DialogSettings.Get())
				{
					for (const EOctoDenStandardInputAction Action : Settings->AnalyzeSelectedInputMappingContext().GetAvailableActions())
					{
						MenuBuilder.AddMenuEntry(
							UOctoDenInputBuilderSettings::GetStandardActionDisplayText(Action),
							FText::GetEmpty(),
							FSlateIcon(),
							FUIAction(FExecuteAction::CreateLambda([SelectedActionProperty, PropertyUtilities, Action]()
							{
								SelectedActionProperty->SetValue(static_cast<uint8>(Action));
								if (PropertyUtilities.IsValid())
								{
									PropertyUtilities->ForceRefresh();
								}
							})));
					}
				}

				return MenuBuilder.MakeWidget();
			})
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text_Lambda([DialogSettings]()
				{
					return GetSelectedActionLabel(DialogSettings);
				})
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(120.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("AddInputButton", "Add Input"))
				.IsEnabled_Lambda([DialogSettings]()
				{
					if (const UOctoDenInputBuilderSettings* Settings = DialogSettings.Get())
					{
						return Settings->CanAddSelectedAction();
					}

					return false;
				})
				.ToolTipText_Lambda([DialogSettings]()
				{
					return GetAddButtonToolTip(DialogSettings);
				})
				.OnClicked_Lambda([DialogSettings]()
				{
					if (UOctoDenInputBuilderSettings* Settings = DialogSettings.Get())
					{
						EOctoDenStandardInputAction ResolvedAction = EOctoDenStandardInputAction::Move;
						if (Settings->ResolveSelectedAction(ResolvedAction))
						{
							Settings->SelectedAction = ResolvedAction;
						}

						FOctoDenModule& Module = FModuleManager::LoadModuleChecked<FOctoDenModule>(TEXT("OctoDen"));
						Module.BuildInputAssets(Settings);
					}

					return FReply::Handled();
				})
			]
		]
	];

	BuilderCategory.AddCustomRow(LOCTEXT("PresetSummaryFilter", "Preset Summary"))
	.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings]()
	{
		return GetPresetSummaryVisibility(DialogSettings, EOctoDenStandardInputAction::Move);
	}))
	.WholeRowContent()
	[
		SNew(STextBlock)
		.Text(UOctoDenInputBuilderSettings::GetPresetBindingSummary(EOctoDenStandardInputAction::Move))
		.AutoWrapText(true)
	];

	BuilderCategory.AddCustomRow(LOCTEXT("PresetSummaryLookFilter", "Preset Summary"))
	.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings]()
	{
		return GetPresetSummaryVisibility(DialogSettings, EOctoDenStandardInputAction::Look);
	}))
	.WholeRowContent()
	[
		SNew(STextBlock)
		.Text(UOctoDenInputBuilderSettings::GetPresetBindingSummary(EOctoDenStandardInputAction::Look))
		.AutoWrapText(true)
	];

	auto AddBindingRows = [&BuilderCategory, DialogSettings](const EOctoDenStandardInputAction InAction, const TSharedRef<IPropertyHandle>& BindingProperty)
	{
		const TSharedPtr<IPropertyHandle> PrimaryKeyProperty = BindingProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FOctoDenInputBindingDraft, PrimaryKey));
		const TSharedPtr<IPropertyHandle> SecondaryKeyProperty = BindingProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FOctoDenInputBindingDraft, SecondaryKey));
		const TSharedPtr<IPropertyHandle> GamepadKeyProperty = BindingProperty->GetChildHandle(GET_MEMBER_NAME_CHECKED(FOctoDenInputBindingDraft, GamepadKey));

		check(PrimaryKeyProperty.IsValid());
		check(SecondaryKeyProperty.IsValid());
		check(GamepadKeyProperty.IsValid());

		BuilderCategory.AddCustomRow(LOCTEXT("PrimaryKeyFilter", "Primary Key"))
		.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings, InAction]()
		{
			return GetBindingVisibility(DialogSettings, InAction);
		}))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("PrimaryKeyLabel", "Primary Key"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		.MinDesiredWidth(320.0f)
		.MaxDesiredWidth(420.0f)
		[
			PrimaryKeyProperty->CreatePropertyValueWidget()
		];

		BuilderCategory.AddCustomRow(LOCTEXT("SecondaryKeyFilter", "Secondary Key"))
		.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings, InAction]()
		{
			return GetBindingVisibility(DialogSettings, InAction);
		}))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("SecondaryKeyLabel", "Secondary Key"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		.MinDesiredWidth(320.0f)
		.MaxDesiredWidth(420.0f)
		[
			SecondaryKeyProperty->CreatePropertyValueWidget()
		];

		BuilderCategory.AddCustomRow(LOCTEXT("GamepadKeyFilter", "Gamepad Key"))
		.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings, InAction]()
		{
			return GetBindingVisibility(DialogSettings, InAction);
		}))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("GamepadKeyLabel", "Gamepad Key"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		.MinDesiredWidth(320.0f)
		.MaxDesiredWidth(420.0f)
		[
			GamepadKeyProperty->CreatePropertyValueWidget()
		];
	};

	AddBindingRows(EOctoDenStandardInputAction::Jump, JumpBindingsProperty);
	AddBindingRows(EOctoDenStandardInputAction::Fire, FireBindingsProperty);

	BuilderCategory.AddCustomRow(LOCTEXT("AllActionsAddedFilter", "All Actions Added"))
	.WholeRowContent()
	[
		SNew(STextBlock)
		.Visibility_Lambda([DialogSettings]()
		{
			return GetAllAddedHintVisibility(DialogSettings);
		})
		.Text(LOCTEXT("AllActionsAddedHint", "All managed actions are already present in the selected IMC. Remove one from the IMC to add it again."))
		.Font(IDetailLayoutBuilder::GetDetailFontItalic())
		.AutoWrapText(true)
	];

	IDetailCategoryBuilder& StatusCategory = DetailBuilder.EditCategory(TEXT("Status"));
	for (const EOctoDenStandardInputAction Action : UOctoDenInputBuilderSettings::GetAllStandardActions())
	{
		StatusCategory.AddCustomRow(UOctoDenInputBuilderSettings::GetStandardActionDisplayText(Action))
		.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings]()
		{
			return GetSetupRowsVisibility(DialogSettings);
		}))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(UOctoDenInputBuilderSettings::GetStandardActionDisplayText(Action))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		.MinDesiredWidth(320.0f)
		.MaxDesiredWidth(420.0f)
		[
			SNew(STextBlock)
			.Text_Lambda([DialogSettings, Action]()
			{
				return GetStatusText(DialogSettings, Action);
			})
			.ColorAndOpacity_Lambda([DialogSettings, Action]()
			{
				return GetStatusColor(DialogSettings, Action);
			})
		];
	}

	StatusCategory.AddCustomRow(LOCTEXT("NullMappingNoticeFilter", "Broken IMC Mappings"))
	.Visibility(TAttribute<EVisibility>::CreateLambda([DialogSettings]()
	{
		return GetSetupRowsVisibility(DialogSettings) == EVisibility::Visible
			? GetNullMappingNoticeVisibility(DialogSettings)
			: EVisibility::Collapsed;
	}))
	.WholeRowContent()
	[
		SNew(STextBlock)
		.Text_Lambda([DialogSettings]()
		{
			return GetNullMappingNotice(DialogSettings);
		})
		.AutoWrapText(true)
		.ColorAndOpacity(FSlateColor(FLinearColor(0.80f, 0.45f, 0.20f, 1.0f)))
	];
}

#undef LOCTEXT_NAMESPACE
