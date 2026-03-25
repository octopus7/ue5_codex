#include "Modules/ModuleManager.h"

#include "LevelEditor.h"
#include "PrototypeMeshBuilderController.h"
#include "PrototypeMeshBuilderPanel.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FPrototypeMeshBuilderModule"

namespace
{
	const FName PrototypeMeshBuilderTabName(TEXT("PrototypeMeshBuilder"));
}

class FPrototypeMeshBuilderModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		Controller = MakeShared<FPrototypeMeshBuilderController>();

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
			PrototypeMeshBuilderTabName,
			FOnSpawnTab::CreateRaw(this, &FPrototypeMeshBuilderModule::OnSpawnPluginTab))
			.SetDisplayName(LOCTEXT("PrototypeMeshBuilderTabTitle", "Prototype Mesh Builder"))
			.SetMenuType(ETabSpawnerMenuType::Hidden);

		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FPrototypeMeshBuilderModule::RegisterMenus));
	}

	virtual void ShutdownModule() override
	{
		if (UToolMenus* ToolMenus = UToolMenus::TryGet())
		{
			UToolMenus::UnRegisterStartupCallback(this);
			ToolMenus->UnregisterOwner(this);
		}

		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(PrototypeMeshBuilderTabName);

		if (Controller.IsValid())
		{
			Controller->CleanupPreview();
			Controller.Reset();
		}
	}

private:
	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
	{
		TSharedRef<SDockTab> Tab = SNew(SDockTab)
			.TabRole(ETabRole::NomadTab);

		Tab->SetContent(
			SNew(SPrototypeMeshBuilderPanel)
			.Controller(Controller));

		Tab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda([Controller = Controller](TSharedRef<SDockTab>)
		{
			if (Controller.IsValid())
			{
				Controller->CleanupPreview();
			}
		}));

		return Tab;
	}

	void RegisterMenus()
	{
		FToolMenuOwnerScoped OwnerScoped(this);

		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(TEXT("LevelEditor.MainMenu.Window"));
		FToolMenuSection& Section = Menu->FindOrAddSection(TEXT("WindowLayout"));
		Section.AddMenuEntry(
			TEXT("PrototypeMeshBuilder.OpenTab"),
			LOCTEXT("PrototypeMeshBuilderMenuLabel", "Prototype Mesh Builder"),
			LOCTEXT("PrototypeMeshBuilderMenuTooltip", "Open the prototype mesh builder tool."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateRaw(this, &FPrototypeMeshBuilderModule::PluginButtonClicked)));
	}

	void PluginButtonClicked()
	{
		FGlobalTabmanager::Get()->TryInvokeTab(PrototypeMeshBuilderTabName);
	}

private:
	TSharedPtr<FPrototypeMeshBuilderController> Controller;
};

IMPLEMENT_MODULE(FPrototypeMeshBuilderModule, PrototypeMeshBuilder)

#undef LOCTEXT_NAMESPACE
