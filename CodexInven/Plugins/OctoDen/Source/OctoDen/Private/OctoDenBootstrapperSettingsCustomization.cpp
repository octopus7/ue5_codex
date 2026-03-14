#include "OctoDenBootstrapperSettingsCustomization.h"

#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Modules/ModuleManager.h"
#include "OctoDenBootstrapperSettings.h"
#include "OctoDenModule.h"
#include "PropertyHandle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "OctoDenBootstrapperSettingsCustomization"

TSharedRef<IDetailCustomization> FOctoDenBootstrapperSettingsCustomization::MakeInstance()
{
	return MakeShared<FOctoDenBootstrapperSettingsCustomization>();
}

void FOctoDenBootstrapperSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TSharedRef<IPropertyHandle> ManagedMapNameProperty = DetailBuilder.GetProperty(
		GET_MEMBER_NAME_CHECKED(UOctoDenBootstrapperSettings, ManagedMapName));
	TSharedRef<IPropertyHandle> ManagedMapTemplateProperty = DetailBuilder.GetProperty(
		GET_MEMBER_NAME_CHECKED(UOctoDenBootstrapperSettings, ManagedMapTemplate));
	DetailBuilder.HideProperty(ManagedMapNameProperty);
	DetailBuilder.HideProperty(ManagedMapTemplateProperty);

	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

	TWeakObjectPtr<UOctoDenBootstrapperSettings> DialogSettings;
	for (const TWeakObjectPtr<UObject>& Object : CustomizedObjects)
	{
		if (UOctoDenBootstrapperSettings* Settings = Cast<UOctoDenBootstrapperSettings>(Object.Get()))
		{
			DialogSettings = Settings;
			break;
		}
	}

	const bool bShowCodeGenerationUI = FOctoDenModule::ShouldShowBootstrapperCodeGenerationUI(DialogSettings.Get());
	if (!bShowCodeGenerationUI)
	{
		DetailBuilder.HideCategory(TEXT("Code"));
		DetailBuilder.HideCategory(TEXT("GameInstance"));
		DetailBuilder.HideCategory(TEXT("GameMode"));
	}

	IDetailCategoryBuilder& MapsCategory = DetailBuilder.EditCategory(TEXT("Maps"));
	MapsCategory.AddCustomRow(LOCTEXT("ManagedMapFilterText", "Managed Map"))
	.NameContent()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("ManagedMapLabel", "Managed Map"))
		.Font(IDetailLayoutBuilder::GetDetailFont())
		.ToolTipText(LOCTEXT("ManagedMapTooltip", "Used by the Create and Open buttons to manage /Game/Maps/<Name>."))
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
			SNew(SEditableTextBox)
			.Text_Lambda([ManagedMapNameProperty]()
			{
				FString CurrentValue;
				ManagedMapNameProperty->GetValue(CurrentValue);
				return FText::FromString(CurrentValue);
			})
			.OnTextCommitted_Lambda([ManagedMapNameProperty](const FText& InText, ETextCommit::Type)
			{
				ManagedMapNameProperty->SetValue(InText.ToString());
			})
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.0f, 0.0f, 8.0f, 0.0f)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(120.0f)
			[
				ManagedMapTemplateProperty->CreatePropertyValueWidget()
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.0f, 0.0f, 8.0f, 0.0f)
		[
			SNew(SButton)
			.Text(LOCTEXT("CreateManagedMapButton", "Create"))
			.OnClicked_Lambda([DialogSettings]()
			{
				if (UOctoDenBootstrapperSettings* Settings = DialogSettings.Get())
				{
					FOctoDenModule& Module = FModuleManager::LoadModuleChecked<FOctoDenModule>(TEXT("OctoDen"));
					Module.CreateManagedMap(Settings);
				}

				return FReply::Handled();
			})
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SButton)
			.Text(LOCTEXT("OpenManagedMapButton", "Open"))
			.OnClicked_Lambda([DialogSettings]()
			{
				if (UOctoDenBootstrapperSettings* Settings = DialogSettings.Get())
				{
					FOctoDenModule& Module = FModuleManager::LoadModuleChecked<FOctoDenModule>(TEXT("OctoDen"));
					Module.OpenManagedMap(Settings);
				}

				return FReply::Handled();
			})
		]
	];

	if (!bShowCodeGenerationUI)
	{
		MapsCategory.AddCustomRow(LOCTEXT("CodeGenerationHiddenFilterText", "Code Generation"))
		.WholeRowContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("CodeGenerationHiddenMessage", "Create or open a saved target map to show the code generation settings."))
			.Font(IDetailLayoutBuilder::GetDetailFontItalic())
			.AutoWrapText(true)
		];
	}
}

#undef LOCTEXT_NAMESPACE
