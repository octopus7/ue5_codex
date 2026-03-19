#include "Bootstrapper/OctoDenBootstrapperSettings.h"
#include "EnhancedActionKeyMapping.h"
#include "InputAction.h"
#include "InputBuilder/InputBuilderService.h"
#include "InputBuilder/OctoDenInputBuilderSettings.h"
#include "InputBuilder/OctoDenInputMappingRules.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "Misc/AutomationTest.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"

namespace
{
	TArray<FEnhancedActionKeyMapping>& AccessTestMappings(UInputMappingContext& InputMappingContext)
	{
		return const_cast<TArray<FEnhancedActionKeyMapping>&>(InputMappingContext.GetMappings());
	}

	const FEnhancedActionKeyMapping* FindMapping(const UInputMappingContext& InputMappingContext, const UInputAction* Action, const FKey& Key)
	{
		for (const FEnhancedActionKeyMapping& Mapping : InputMappingContext.GetMappings())
		{
			if (Mapping.Action == Action && Mapping.Key == Key)
			{
				return &Mapping;
			}
		}
		return nullptr;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOctoDenMenuRegistrationTest, "OctoDen.Menu.Registration", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOctoDenMenuRegistrationTest::RunTest(const FString& Parameters)
{
	TestTrue(TEXT("OctoDen module loads"), FModuleManager::Get().LoadModulePtr<IModuleInterface>(TEXT("OctoDen")) != nullptr);
	TestNotNull(TEXT("ToolMenus available"), UToolMenus::Get());
	TestNotNull(TEXT("Level editor main menu exists"), UToolMenus::Get()->FindMenu(TEXT("LevelEditor.MainMenu")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOctoDenInputDefaultsTest, "OctoDen.InputBuilder.Defaults", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOctoDenInputDefaultsTest::RunTest(const FString& Parameters)
{
	UOctoDenInputBuilderSettings* Settings = NewObject<UOctoDenInputBuilderSettings>();
	Settings->ResetToDefaults();
	TestEqual(TEXT("Default prefix"), Settings->InputActionPrefix, FString(TEXT("IA_")));
	TestEqual(TEXT("Default action folder"), Settings->InputActionFolder, FString(TEXT("/Game/Input/Actions")));
	TestEqual(TEXT("Default config folder"), Settings->InputConfigFolder, FString(TEXT("/Game/Input/Configs")));
	TestEqual(TEXT("Default config asset"), Settings->InputConfigAssetName, FString(TEXT("DA_DefaultInputConfig")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOctoDenImcAnalysisTest, "OctoDen.InputBuilder.Analysis", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOctoDenImcAnalysisTest::RunTest(const FString& Parameters)
{
	UOctoDenInputBuilderSettings* Settings = NewObject<UOctoDenInputBuilderSettings>();
	Settings->ResetToDefaults();
	UInputMappingContext* Context = NewObject<UInputMappingContext>();
	UInputAction* MoveAction = NewObject<UInputAction>();
	MoveAction->Rename(TEXT("IA_Move"));
	Context->MapKey(MoveAction, EKeys::D);
	AccessTestMappings(*Context).Add(FEnhancedActionKeyMapping());
	Settings->SelectedInputMappingContext = Context;

	FOctoDenInputContextAnalysis Analysis;
	OctoDenInputBuilder::AnalyzeInputContext(*Settings, Analysis);
	TestTrue(TEXT("Selected IMC is reported"), Analysis.bHasSelection);
	TestEqual(TEXT("One null mapping counted"), Analysis.NullActionMappingCount, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOctoDenJumpOverwriteTest, "OctoDen.InputBuilder.JumpOverwrite", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOctoDenJumpOverwriteTest::RunTest(const FString& Parameters)
{
	UInputMappingContext* Context = NewObject<UInputMappingContext>();
	UInputAction* Action = NewObject<UInputAction>();
	Context->MapKey(Action, EKeys::J);

	FOctoDenManagedActionBuildResult Result;
	FText Failure;
	TestTrue(TEXT("Jump overwrite succeeds"), OctoDenInputMappingRules::ApplyMappings(*Context, *Action, EOctoDenManagedInputAction::Jump, UOctoDenInputBuilderSettings::MakeDefaultDraft(EOctoDenManagedInputAction::Jump), Result, Failure));
	TestNull(TEXT("Old J mapping removed"), FindMapping(*Context, Action, EKeys::J));
	TestNotNull(TEXT("Space mapping exists"), FindMapping(*Context, Action, EKeys::SpaceBar));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOctoDenMovePresetTest, "OctoDen.InputBuilder.MovePreset", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOctoDenMovePresetTest::RunTest(const FString& Parameters)
{
	UInputMappingContext* Context = NewObject<UInputMappingContext>();
	UInputAction* Action = NewObject<UInputAction>();
	FOctoDenManagedActionBuildResult Result;
	FText Failure;
	TestTrue(TEXT("Move preset succeeds"), OctoDenInputMappingRules::ApplyMappings(*Context, *Action, EOctoDenManagedInputAction::Move, FOctoDenKeyDraft(), Result, Failure));
	TestNotNull(TEXT("D mapped"), FindMapping(*Context, Action, EKeys::D));
	TestNotNull(TEXT("A mapped"), FindMapping(*Context, Action, EKeys::A));
	TestNotNull(TEXT("W mapped"), FindMapping(*Context, Action, EKeys::W));
	TestNotNull(TEXT("S mapped"), FindMapping(*Context, Action, EKeys::S));
	TestNotNull(TEXT("Left stick mapped"), FindMapping(*Context, Action, EKeys::Gamepad_Left2D));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOctoDenRemoveAndReaddTest, "OctoDen.InputBuilder.RemoveAndReadd", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOctoDenRemoveAndReaddTest::RunTest(const FString& Parameters)
{
	UInputMappingContext* Context = NewObject<UInputMappingContext>();
	UInputAction* Action = NewObject<UInputAction>();
	Context->MapKey(Action, EKeys::Mouse2D);

	FOctoDenManagedActionBuildResult Result;
	FText Failure;
	TestTrue(TEXT("Look rebuild succeeds"), OctoDenInputMappingRules::ApplyMappings(*Context, *Action, EOctoDenManagedInputAction::Look, FOctoDenKeyDraft(), Result, Failure));
	TestEqual(TEXT("Two final mappings"), Context->GetMappings().Num(), 2);
	TestNotNull(TEXT("Mouse2D exists"), FindMapping(*Context, Action, EKeys::Mouse2D));
	TestNotNull(TEXT("Gamepad_Right2D exists"), FindMapping(*Context, Action, EKeys::Gamepad_Right2D));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOctoDenBootstrapperDefaultsTest, "OctoDen.Bootstrapper.Defaults", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOctoDenBootstrapperDefaultsTest::RunTest(const FString& Parameters)
{
	UOctoDenBootstrapperSettings* Settings = NewObject<UOctoDenBootstrapperSettings>();
	Settings->ResetToDefaults();
	TestEqual(TEXT("Runtime module defaults to project"), Settings->RuntimeModuleName, FString(TEXT("CodexInvenMedium")));
	TestTrue(TEXT("Target map defaults under /Game/Maps"), Settings->TargetMap.ToString().StartsWith(TEXT("/Game/Maps/")));
	return true;
}
