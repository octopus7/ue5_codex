#include "Transient/PickupPageScaffolding/CodexInvenPickupPageScaffoldCommandlet.h"

#include "AssetImportTask.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Blueprint/WidgetTree.h"
#include "CodexInvenPickupPageWidget.h"
#include "Components/CanvasPanel.h"
#include "Engine/Texture2D.h"
#include "Factories/TextureFactory.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/UnrealType.h"
#include "WidgetBlueprint.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenPickupPageScaffold, Log, All);

namespace
{
	const TCHAR* PickupPageWidgetPackagePath = TEXT("/Game/UI/Pickups/WBP_PickupPage");
	const TCHAR* PickupGuideTexturePackagePath = TEXT("/Game/Art/Pickups/Guides/T_PickupSafeGuide_2340x1440");
	const TCHAR* PickupGuideSourcePath = TEXT("Content/Art/Pickups/Guides/PickupSafeGuide_2340x1440.png");
	const FName PickupPageCreationContext(TEXT("CodexInvenPickupPageScaffold"));

	FString MakeObjectPath(const FString& InPackagePath)
	{
		return InPackagePath + TEXT(".") + FPackageName::GetLongPackageAssetName(InPackagePath);
	}

	bool SaveObjectPackage(UObject& InObject, FString& OutError)
	{
		UPackage* const Package = InObject.GetPackage();
		if (Package == nullptr)
		{
			OutError = FString::Printf(TEXT("Object %s has no package."), *InObject.GetName());
			return false;
		}

		const FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		SaveArgs.Error = GError;
		if (!UPackage::SavePackage(Package, &InObject, *Filename, SaveArgs))
		{
			OutError = FString::Printf(TEXT("Failed to save package %s."), *Package->GetName());
			return false;
		}

		return true;
	}

	bool SaveBlueprintAsset(UWidgetBlueprint& InWidgetBlueprint, FString& OutError)
	{
		return SaveObjectPackage(InWidgetBlueprint, OutError);
	}

	FString GetGuideImageFilename()
	{
		return FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), PickupGuideSourcePath));
	}

	UTexture2D* ImportOrUpdateTextureAsset(
		const FString& InSourceFilename,
		const FString& InPackagePath,
		FString& OutError)
	{
		if (!FPaths::FileExists(InSourceFilename))
		{
			OutError = FString::Printf(TEXT("Source image does not exist: %s"), *InSourceFilename);
			return nullptr;
		}

		UAssetImportTask* const ImportTask = NewObject<UAssetImportTask>(GetTransientPackage());
		UTextureFactory* const TextureFactory = NewObject<UTextureFactory>(ImportTask);
		if (ImportTask == nullptr || TextureFactory == nullptr)
		{
			OutError = TEXT("Failed to allocate texture import helpers for the pickup page guide.");
			return nullptr;
		}

		ImportTask->Filename = InSourceFilename;
		ImportTask->DestinationPath = FPackageName::GetLongPackagePath(InPackagePath);
		ImportTask->DestinationName = FPackageName::GetLongPackageAssetName(InPackagePath);
		ImportTask->bReplaceExisting = true;
		ImportTask->bReplaceExistingSettings = true;
		ImportTask->bAutomated = true;
		ImportTask->bSave = true;
		ImportTask->bAsync = false;
		ImportTask->Factory = TextureFactory;

		TArray<UAssetImportTask*> ImportTasks;
		ImportTasks.Add(ImportTask);
		FAssetToolsModule::GetModule().Get().ImportAssetTasks(ImportTasks);

		UTexture2D* ImportedTexture = nullptr;
		for (UObject* const ImportedObject : ImportTask->GetObjects())
		{
			ImportedTexture = Cast<UTexture2D>(ImportedObject);
			if (ImportedTexture != nullptr)
			{
				break;
			}
		}

		if (ImportedTexture == nullptr)
		{
			ImportedTexture = LoadObject<UTexture2D>(nullptr, *MakeObjectPath(InPackagePath));
		}

		if (ImportedTexture == nullptr)
		{
			OutError = FString::Printf(TEXT("Failed to import texture asset %s from %s."), *InPackagePath, *InSourceFilename);
			return nullptr;
		}

		ImportedTexture->MarkPackageDirty();
		if (!SaveObjectPackage(*ImportedTexture, OutError))
		{
			return nullptr;
		}

		return ImportedTexture;
	}

	void RenameExistingWidgetsToTransient(UWidgetTree& InWidgetTree)
	{
		TArray<UWidget*> ExistingWidgets;
		InWidgetTree.GetAllWidgets(ExistingWidgets);
		for (UWidget* ExistingWidget : ExistingWidgets)
		{
			if (ExistingWidget != nullptr)
			{
				ExistingWidget->Rename(nullptr, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional | REN_DoNotDirty);
			}
		}

		InWidgetTree.NamedSlotBindings.Empty();
		InWidgetTree.RootWidget = nullptr;
	}

	void BuildPickupPageWidgetTree(UWidgetBlueprint& InWidgetBlueprint)
	{
		UWidgetTree& WidgetTree = *InWidgetBlueprint.WidgetTree;
		RenameExistingWidgetsToTransient(WidgetTree);

		UCanvasPanel* const RootCanvasPanel = WidgetTree.ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("RootCanvasPanel"));
		RootCanvasPanel->SetClipping(EWidgetClipping::ClipToBounds);
		WidgetTree.RootWidget = RootCanvasPanel;
	}

	bool SetClassDefaultTextureProperty(UClass* InClass, const FName InPropertyName, UTexture2D* InValue, FString& OutError)
	{
		UObject* const DefaultObject = InClass != nullptr ? InClass->GetDefaultObject() : nullptr;
		FObjectProperty* const Property = InClass != nullptr ? FindFProperty<FObjectProperty>(InClass, InPropertyName) : nullptr;
		if (DefaultObject == nullptr || Property == nullptr || !Property->PropertyClass->IsChildOf(UTexture2D::StaticClass()))
		{
			OutError = FString::Printf(
				TEXT("Failed to find texture property %s on class %s."),
				*InPropertyName.ToString(),
				InClass != nullptr ? *InClass->GetName() : TEXT("<null>"));
			return false;
		}

		DefaultObject->Modify();
		Property->SetObjectPropertyValue_InContainer(DefaultObject, InValue);
		return true;
	}

	bool CreateOrUpdatePickupPageWidgetBlueprint(UTexture2D* InGuideTexture, UWidgetBlueprint*& OutWidgetBlueprint, FString& OutError)
	{
		UWidgetBlueprint* WidgetBlueprint = LoadObject<UWidgetBlueprint>(nullptr, *MakeObjectPath(PickupPageWidgetPackagePath));
		if (WidgetBlueprint == nullptr)
		{
			UPackage* const Package = CreatePackage(PickupPageWidgetPackagePath);
			WidgetBlueprint = Cast<UWidgetBlueprint>(FKismetEditorUtilities::CreateBlueprint(
				UCodexInvenPickupPageWidget::StaticClass(),
				Package,
				*FPackageName::GetLongPackageAssetName(PickupPageWidgetPackagePath),
				BPTYPE_Normal,
				UWidgetBlueprint::StaticClass(),
				UWidgetBlueprintGeneratedClass::StaticClass(),
				PickupPageCreationContext));
			if (WidgetBlueprint == nullptr)
			{
				OutError = FString::Printf(TEXT("Failed to create widget blueprint %s."), PickupPageWidgetPackagePath);
				return false;
			}

			FAssetRegistryModule::AssetCreated(WidgetBlueprint);
		}
		else if (WidgetBlueprint->ParentClass != UCodexInvenPickupPageWidget::StaticClass())
		{
			OutError = FString::Printf(
				TEXT("Existing widget blueprint %s uses parent %s instead of %s."),
				*WidgetBlueprint->GetName(),
				*GetNameSafe(WidgetBlueprint->ParentClass),
				*UCodexInvenPickupPageWidget::StaticClass()->GetName());
			return false;
		}

		if (WidgetBlueprint->WidgetTree == nullptr)
		{
			WidgetBlueprint->WidgetTree = NewObject<UWidgetTree>(WidgetBlueprint, TEXT("WidgetTree"), RF_Transactional);
		}

		WidgetBlueprint->SetFlags(RF_Transactional);
		WidgetBlueprint->Modify();
		WidgetBlueprint->WidgetTree->SetFlags(RF_Transactional);
		WidgetBlueprint->WidgetTree->Modify();

		BuildPickupPageWidgetTree(*WidgetBlueprint);
		WidgetBlueprint->WidgetVariableNameToGuidMap.Reset();

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBlueprint);
		FKismetEditorUtilities::CompileBlueprint(WidgetBlueprint);

		if (WidgetBlueprint->GeneratedClass == nullptr)
		{
			OutError = FString::Printf(TEXT("Widget blueprint %s did not generate a class."), PickupPageWidgetPackagePath);
			return false;
		}

		if (!SetClassDefaultTextureProperty(WidgetBlueprint->GeneratedClass, TEXT("BackgroundTexture"), InGuideTexture, OutError))
		{
			return false;
		}

		WidgetBlueprint->MarkPackageDirty();
		if (!SaveBlueprintAsset(*WidgetBlueprint, OutError))
		{
			return false;
		}

		OutWidgetBlueprint = WidgetBlueprint;
		return true;
	}
}

UCodexInvenPickupPageScaffoldCommandlet::UCodexInvenPickupPageScaffoldCommandlet()
{
	IsServer = false;
	IsClient = false;
	IsEditor = true;
	LogToConsole = true;
	ShowErrorCount = true;
	HelpDescription = TEXT("Create or update /Game/UI/Pickups/WBP_PickupPage as a BP child of UCodexInvenPickupPageWidget.");
}

int32 UCodexInvenPickupPageScaffoldCommandlet::Main(const FString& InParams)
{
	static_cast<void>(InParams);

	FString ErrorMessage;
	UTexture2D* const GuideTexture = ImportOrUpdateTextureAsset(
		GetGuideImageFilename(),
		PickupGuideTexturePackagePath,
		ErrorMessage);
	if (GuideTexture == nullptr)
	{
		UE_LOG(LogCodexInvenPickupPageScaffold, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UWidgetBlueprint* WidgetBlueprint = nullptr;
	if (!CreateOrUpdatePickupPageWidgetBlueprint(GuideTexture, WidgetBlueprint, ErrorMessage))
	{
		UE_LOG(LogCodexInvenPickupPageScaffold, Error, TEXT("%s"), *ErrorMessage);
		return 1;
	}

	UE_LOG(
		LogCodexInvenPickupPageScaffold,
		Display,
		TEXT("Pickup page widget blueprint is ready at %s with guide texture %s."),
		PickupPageWidgetPackagePath,
		PickupGuideTexturePackagePath);

	return 0;
}
