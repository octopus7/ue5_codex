#include "VoxImporterEditorModule.h"

#include "ContentBrowserMenuContexts.h"
#include "ContentBrowserModule.h"
#include "Dialogs/Dialogs.h"
#include "Engine/StaticMesh.h"
#include "IContentBrowserSingleton.h"
#include "Misc/ScopedSlowTask.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "VoxReconstructionService.h"
#include "VoxStaticMeshUtilities.h"

#define LOCTEXT_NAMESPACE "VoxImporterEditorModule"

void FVoxImporterEditorModule::StartupModule()
{
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FVoxImporterEditorModule::RegisterMenus));
}

void FVoxImporterEditorModule::ShutdownModule()
{
	UToolMenus::UnregisterOwner(this);
}

void FVoxImporterEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.StaticMesh");
	if (!Menu)
	{
		return;
	}

	FToolMenuSection& Section = Menu->FindOrAddSection("GetAssetActions");
	Section.AddDynamicEntry("VoxImporterActions", FNewToolMenuSectionDelegate::CreateLambda([this](FToolMenuSection& InSection)
	{
		const UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
		if (!Context)
		{
			return;
		}

		TArray<TWeakObjectPtr<UStaticMesh>> EligibleMeshes;
		for (UStaticMesh* StaticMesh : Context->LoadSelectedObjects<UStaticMesh>())
		{
			if (VoxStaticMeshUtilities::IsPrimaryVoxSourceStaticMesh(StaticMesh))
			{
				EligibleMeshes.Add(StaticMesh);
			}
		}

		if (EligibleMeshes.IsEmpty())
		{
			return;
		}

		InSection.AddSubMenu(
			"VoxImporterSubMenu",
			LOCTEXT("VoxSubMenuLabel", "Vox"),
			LOCTEXT("VoxSubMenuToolTip", "Actions for Static Mesh assets imported from MagicaVoxel .vox files."),
			FNewToolMenuDelegate::CreateLambda([this, EligibleMeshes](UToolMenu* SubMenu)
			{
				FToolMenuSection& SubSection = SubMenu->AddSection("VoxImporterActions", LOCTEXT("VoxActionsHeading", "Vox"));
				SubSection.AddMenuEntry(
					"GenerateSmoothReconstruction",
					LOCTEXT("GenerateSmoothReconstructionLabel", "Generate Smooth Reconstruction"),
					LOCTEXT("GenerateSmoothReconstructionToolTip", "Rebuild this .vox asset as a smoother reconstructed Static Mesh and save it as a new asset."),
					FSlateIcon(),
					FToolUIActionChoice(FUIAction(FExecuteAction::CreateLambda([this, EligibleMeshes]()
					{
						GenerateSmoothReconstructions(EligibleMeshes);
					}))));
			}));
	}));
}

void FVoxImporterEditorModule::GenerateSmoothReconstructions(const TArray<TWeakObjectPtr<UStaticMesh>>& StaticMeshes) const
{
	TArray<UObject*> CreatedAssets;
	TArray<FString> Errors;

	FScopedSlowTask SlowTask(static_cast<float>(StaticMeshes.Num()), LOCTEXT("GenerateSmoothReconstructionsProgress", "Generating smooth VOX reconstructions..."));
	SlowTask.MakeDialogDelayed(0.2f);

	for (const TWeakObjectPtr<UStaticMesh>& StaticMeshWeak : StaticMeshes)
	{
		SlowTask.EnterProgressFrame(1.0f);

		UStaticMesh* StaticMesh = StaticMeshWeak.Get();
		if (!StaticMesh)
		{
			continue;
		}

		UObject* CreatedAsset = nullptr;
		FString ErrorMessage;
		if (VoxReconstructionService::GenerateSmoothReconstruction(StaticMesh, CreatedAsset, ErrorMessage))
		{
			if (CreatedAsset)
			{
				CreatedAssets.Add(CreatedAsset);
			}
		}
		else
		{
			Errors.Add(FString::Printf(TEXT("%s: %s"), *StaticMesh->GetName(), *ErrorMessage));
		}
	}

	if (!CreatedAssets.IsEmpty())
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		ContentBrowserModule.Get().SyncBrowserToAssets(CreatedAssets);
	}

	if (!Errors.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Join(Errors, TEXT("\n"))));
	}
}

IMPLEMENT_MODULE(FVoxImporterEditorModule, VoxImporterEditor)

#undef LOCTEXT_NAMESPACE
