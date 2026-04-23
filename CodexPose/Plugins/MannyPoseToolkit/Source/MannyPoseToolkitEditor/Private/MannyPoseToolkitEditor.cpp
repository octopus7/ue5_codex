#include "MannyPoseToolkitEditor.h"

#include "LevelEditor.h"
#include "MannyPoseToolkitStyle.h"
#include "MannyPoseToolkitCommands.h"
#include "MannyPoseToolkitEditorWidget.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"

static const FName MannyPoseToolkitTabName("MannyPoseToolkit");

#define LOCTEXT_NAMESPACE "FMannyPoseToolkitEditorModule"

void FMannyPoseToolkitEditorModule::StartupModule()
{
    if (IsRunningCommandlet())
    {
        return;
    }

    FMannyPoseToolkitStyle::Initialize();
    FMannyPoseToolkitStyle::ReloadTextures();
    FMannyPoseToolkitCommands::Register();

    PluginCommands = MakeShareable(new FUICommandList);
    PluginCommands->MapAction(
        FMannyPoseToolkitCommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FMannyPoseToolkitEditorModule::PluginButtonClicked),
        FCanExecuteAction());

    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MannyPoseToolkitTabName, FOnSpawnTab::CreateRaw(this, &FMannyPoseToolkitEditorModule::OnSpawnPluginTab))
        .SetDisplayName(LOCTEXT("FMannyPoseToolkitTabTitle", "Manny Pose Toolkit"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);

    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMannyPoseToolkitEditorModule::RegisterMenus));
}

void FMannyPoseToolkitEditorModule::ShutdownModule()
{
    if (IsRunningCommandlet())
    {
        return;
    }

    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);

    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MannyPoseToolkitTabName);

    FMannyPoseToolkitCommands::Unregister();
    FMannyPoseToolkitStyle::Shutdown();
}

TSharedRef<SDockTab> FMannyPoseToolkitEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SBox)
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                SNew(SMannyPoseToolkitEditorWidget)
            ]
        ];
}

void FMannyPoseToolkitEditorModule::PluginButtonClicked()
{
    FGlobalTabmanager::Get()->TryInvokeTab(MannyPoseToolkitTabName);
}

void FMannyPoseToolkitEditorModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);

    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
    FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
    Section.AddMenuEntryWithCommandList(FMannyPoseToolkitCommands::Get().OpenPluginWindow, PluginCommands);

    UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
    FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("PluginTools");
    FToolMenuEntry& Entry = ToolbarSection.AddEntry(FToolMenuEntry::InitToolBarButton(FMannyPoseToolkitCommands::Get().OpenPluginWindow));
    Entry.SetCommandList(PluginCommands);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMannyPoseToolkitEditorModule, MannyPoseToolkitEditor)
