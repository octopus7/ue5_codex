#include "OctoDenModule.h"

#include "Bootstrapper/OctoDenBootstrapperService.h"
#include "Bootstrapper/OctoDenBootstrapperSettings.h"
#include "Framework/Docking/TabManager.h"
#include "IDetailsView.h"
#include "InputBuilder/InputBuilderService.h"
#include "InputBuilder/OctoDenInputBuilderSettings.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Shared/OctoDenNotifications.h"
#include "ToolMenus.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

const FName FOctoDenModule::BootstrapperTabId(TEXT("OctoDen.Bootstrapper"));
const FName FOctoDenModule::InputBuilderTabId(TEXT("OctoDen.InputBuilder"));

#define LOCTEXT_NAMESPACE "OctoDenModule"

void FOctoDenModule::StartupModule()
{
	if (IsRunningCommandlet())
	{
		return;
	}

	BootstrapperSettings.Reset(NewObject<UOctoDenBootstrapperSettings>());
	BootstrapperSettings->AddToRoot();
	BootstrapperSettings->ResetToDefaults();

	InputBuilderSettings.Reset(NewObject<UOctoDenInputBuilderSettings>());
	InputBuilderSettings->AddToRoot();
	InputBuilderSettings->ResetToDefaults();

	RegisterTabs();
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FOctoDenModule::RegisterMenus));
}

void FOctoDenModule::ShutdownModule()
{
	if (IsRunningCommandlet())
	{
		return;
	}

	if (UToolMenus::TryGet() != nullptr)
	{
		UToolMenus::UnRegisterStartupCallback(this);
		UToolMenus::UnregisterOwner(this);
	}

	UnregisterTabs();
	BootstrapperDetailsView.Reset();
	InputBuilderDetailsView.Reset();

	if (BootstrapperSettings.IsValid())
	{
		BootstrapperSettings->RemoveFromRoot();
		BootstrapperSettings.Reset();
	}

	if (InputBuilderSettings.IsValid())
	{
		InputBuilderSettings->RemoveFromRoot();
		InputBuilderSettings.Reset();
	}
}

void FOctoDenModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	UToolMenu* MainMenu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu"));
	FToolMenuSection& Section = MainMenu->FindOrAddSection(TEXT("OctoDen"));
	Section.AddSubMenu(
		TEXT("OctoDenRoot"),
		LOCTEXT("OctoDenMenuLabel", "OctoDen"),
		LOCTEXT("OctoDenMenuTooltip", "Open OctoDen editor tools."),
		FNewToolMenuDelegate::CreateLambda([](UToolMenu* SubMenu)
		{
			FToolMenuSection& SubSection = SubMenu->AddSection(TEXT("OctoDenTools"));
			SubSection.AddMenuEntry(
				TEXT("OpenBootstrapper"),
				LOCTEXT("OpenBootstrapperLabel", "Bootstrapper"),
				LOCTEXT("OpenBootstrapperTooltip", "Open the project bootstrapper tool."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([]()
				{
					FGlobalTabmanager::Get()->TryInvokeTab(FOctoDenModule::BootstrapperTabId);
				})));
			SubSection.AddMenuEntry(
				TEXT("OpenInputBuilder"),
				LOCTEXT("OpenInputBuilderLabel", "Input Builder"),
				LOCTEXT("OpenInputBuilderTooltip", "Open the input authoring tool."),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateLambda([]()
				{
					FGlobalTabmanager::Get()->TryInvokeTab(FOctoDenModule::InputBuilderTabId);
				})));
		}));
}

void FOctoDenModule::RegisterTabs()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(BootstrapperTabId, FOnSpawnTab::CreateRaw(this, &FOctoDenModule::SpawnBootstrapperTab))
		.SetDisplayName(LOCTEXT("BootstrapperTabTitle", "OctoDen Bootstrapper"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(InputBuilderTabId, FOnSpawnTab::CreateRaw(this, &FOctoDenModule::SpawnInputBuilderTab))
		.SetDisplayName(LOCTEXT("InputBuilderTabTitle", "OctoDen Input Builder"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FOctoDenModule::UnregisterTabs()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BootstrapperTabId);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(InputBuilderTabId);
}

void FOctoDenModule::ResetBootstrapper()
{
	if (BootstrapperSettings.IsValid())
	{
		BootstrapperSettings->ResetToDefaults();
		if (BootstrapperDetailsView.IsValid())
		{
			BootstrapperDetailsView->ForceRefresh();
		}
	}
	RefreshBootstrapperReadiness();
}

void FOctoDenModule::ResetInputBuilder()
{
	if (InputBuilderSettings.IsValid())
	{
		InputBuilderSettings->ResetToDefaults();
		if (InputBuilderDetailsView.IsValid())
		{
			InputBuilderDetailsView->ForceRefresh();
		}
	}
}

void FOctoDenModule::RefreshBootstrapperReadiness()
{
	bCanApplyBlueprints = BootstrapperSettings.IsValid() && OctoDenBootstrapper::AreNativeClassesReady(*BootstrapperSettings.Get());
}

TSharedRef<SDockTab> FOctoDenModule::SpawnBootstrapperTab(const FSpawnTabArgs& Args)
{
	ResetBootstrapper();
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)[BuildBootstrapperContent()];
}

TSharedRef<SDockTab> FOctoDenModule::SpawnInputBuilderTab(const FSpawnTabArgs& Args)
{
	ResetInputBuilder();
	return SNew(SDockTab).TabRole(ETabRole::NomadTab)[BuildInputBuilderContent()];
}

TSharedRef<SWidget> FOctoDenModule::BuildBootstrapperContent()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.bShowScrollBar = false;
	BootstrapperDetailsView = PropertyEditorModule.CreateDetailView(Args);
	BootstrapperDetailsView->SetObject(BootstrapperSettings.Get());

	return SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(8)
			[
				BootstrapperDetailsView.ToSharedRef()
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(8, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 6, 0)
				[
					SNew(SButton).Text(LOCTEXT("CreateManagedMapButton", "Create Managed Map")).OnClicked_Raw(this, &FOctoDenModule::HandleCreateManagedMap)
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 6, 0)
				[
					SNew(SButton).Text(LOCTEXT("OpenManagedMapButton", "Open Managed Map")).OnClicked_Raw(this, &FOctoDenModule::HandleOpenManagedMap)
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 6, 0)
				[
					SNew(SButton).Text(LOCTEXT("GenerateCodeButton", "Generate Code")).OnClicked_Raw(this, &FOctoDenModule::HandleGenerateCode)
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 6, 0)
				[
					SNew(SButton).IsEnabled_Lambda([this]() { return bCanApplyBlueprints; }).Text(LOCTEXT("CreateBlueprintsAndApplyButton", "Create Blueprints && Apply")).OnClicked_Raw(this, &FOctoDenModule::HandleCreateBlueprintsAndApply)
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton).Text(LOCTEXT("ResetBootstrapperButton", "Reset")).OnClicked_Lambda([this]()
					{
						ResetBootstrapper();
						return FReply::Handled();
					})
				]
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(8, 4)
			[
				SNew(STextBlock).AutoWrapText(true).Text_Lambda([this]()
				{
					return BootstrapperSettings.IsValid() ? BootstrapperSettings->LastStatus : FText::GetEmpty();
				})
			]
		];
}

TSharedRef<SWidget> FOctoDenModule::BuildInputBuilderContent()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.bShowScrollBar = false;
	InputBuilderDetailsView = PropertyEditorModule.CreateDetailView(Args);
	InputBuilderDetailsView->SetObject(InputBuilderSettings.Get());

	return SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight().Padding(8)
			[
				InputBuilderDetailsView.ToSharedRef()
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(8, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 6, 0)
				[
					SNew(SButton).Text(LOCTEXT("BuildManagedActionButton", "Build Managed Action")).OnClicked_Raw(this, &FOctoDenModule::HandleBuildManagedAction)
				]
				+ SHorizontalBox::Slot().AutoWidth().Padding(0, 0, 6, 0)
				[
					SNew(SButton).IsEnabled_Lambda([this]()
					{
						return InputBuilderSettings.IsValid() && OctoDenInputBuilder::CanLinkRuntimeInputConfig(*InputBuilderSettings.Get());
					}).Text(LOCTEXT("LinkRuntimeInputConfigButton", "Link Runtime Input Config")).OnClicked_Raw(this, &FOctoDenModule::HandleLinkRuntimeInputConfig)
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton).Text(LOCTEXT("ResetInputBuilderButton", "Reset")).OnClicked_Lambda([this]()
					{
						ResetInputBuilder();
						return FReply::Handled();
					})
				]
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(8, 4)
			[
				SNew(STextBlock).AutoWrapText(true).Text_Lambda([this]()
				{
					return InputBuilderSettings.IsValid() ? InputBuilderSettings->LastStatus : FText::GetEmpty();
				})
			]
		];
}

FReply FOctoDenModule::HandleCreateManagedMap()
{
	FText Failure;
	if (!OctoDenBootstrapper::CreateManagedMap(*BootstrapperSettings.Get(), Failure))
	{
		BootstrapperSettings->LastStatus = Failure;
		OctoDenNotifications::ShowFailureDialog(LOCTEXT("CreateManagedMapFailureTitle", "Create Managed Map Failed"), Failure);
		return FReply::Handled();
	}

	RefreshBootstrapperReadiness();
	BootstrapperDetailsView->ForceRefresh();
	OctoDenNotifications::ShowSuccessToast(BootstrapperSettings->LastStatus);
	return FReply::Handled();
}

FReply FOctoDenModule::HandleOpenManagedMap()
{
	FText Failure;
	if (!OctoDenBootstrapper::OpenManagedMap(*BootstrapperSettings.Get(), Failure))
	{
		BootstrapperSettings->LastStatus = Failure;
		OctoDenNotifications::ShowFailureDialog(LOCTEXT("OpenManagedMapFailureTitle", "Open Managed Map Failed"), Failure);
		return FReply::Handled();
	}

	OctoDenNotifications::ShowSuccessToast(BootstrapperSettings->LastStatus);
	return FReply::Handled();
}

FReply FOctoDenModule::HandleGenerateCode()
{
	FText Failure;
	if (!OctoDenBootstrapper::GenerateNativeClasses(*BootstrapperSettings.Get(), Failure))
	{
		BootstrapperSettings->LastStatus = Failure;
		OctoDenNotifications::ShowFailureDialog(LOCTEXT("GenerateCodeFailureTitle", "Generate Code Failed"), Failure);
		RefreshBootstrapperReadiness();
		return FReply::Handled();
	}

	RefreshBootstrapperReadiness();
	BootstrapperDetailsView->ForceRefresh();
	OctoDenNotifications::ShowSuccessToast(BootstrapperSettings->LastStatus);
	return FReply::Handled();
}

FReply FOctoDenModule::HandleCreateBlueprintsAndApply()
{
	FText Failure;
	if (!OctoDenBootstrapper::CreateBlueprintsAndApply(*BootstrapperSettings.Get(), Failure))
	{
		BootstrapperSettings->LastStatus = Failure;
		OctoDenNotifications::ShowFailureDialog(LOCTEXT("ApplyFailureTitle", "Apply Failed"), Failure);
		return FReply::Handled();
	}

	OctoDenNotifications::ShowSuccessToast(BootstrapperSettings->LastStatus);
	return FReply::Handled();
}

FReply FOctoDenModule::HandleBuildManagedAction()
{
	FOctoDenManagedActionBuildResult BuildResult;
	FText Failure;
	if (!OctoDenInputBuilder::BuildManagedAction(*InputBuilderSettings.Get(), BuildResult, Failure))
	{
		InputBuilderSettings->LastStatus = Failure;
		OctoDenNotifications::ShowFailureDialog(LOCTEXT("BuildManagedActionFailureTitle", "Build Managed Action Failed"), Failure);
		return FReply::Handled();
	}

	OctoDenNotifications::ShowSuccessToast(InputBuilderSettings->LastStatus);
	return FReply::Handled();
}

FReply FOctoDenModule::HandleLinkRuntimeInputConfig()
{
	FText Failure;
	if (!OctoDenInputBuilder::LinkRuntimeInputConfig(*InputBuilderSettings.Get(), Failure))
	{
		InputBuilderSettings->LastStatus = Failure;
		OctoDenNotifications::ShowFailureDialog(LOCTEXT("LinkRuntimeInputConfigFailureTitle", "Link Runtime Input Config Failed"), Failure);
		return FReply::Handled();
	}

	OctoDenNotifications::ShowSuccessToast(InputBuilderSettings->LastStatus);
	return FReply::Handled();
}

IMPLEMENT_MODULE(FOctoDenModule, OctoDen)

#undef LOCTEXT_NAMESPACE
