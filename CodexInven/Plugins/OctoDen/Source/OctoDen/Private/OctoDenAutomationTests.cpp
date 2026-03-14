#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Modules/ModuleManager.h"
#include "OctoDenModule.h"
#include "OctoDenInputBuilderSettings.h"
#include "ToolMenus.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOctoDenMenuRegistrationTest,
	"OctoDen.Menu.Registration",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOctoDenMenuRegistrationTest::RunTest(const FString& Parameters)
{
	FModuleManager::LoadModuleChecked<FOctoDenModule>(TEXT("OctoDen"));

	UToolMenus* ToolMenus = UToolMenus::Get();
	TestNotNull(TEXT("ToolMenus is available"), ToolMenus);
	if (ToolMenus == nullptr)
	{
		return false;
	}

	TestTrue(TEXT("OctoDen top-level menu is registered"), ToolMenus->IsMenuRegistered(TEXT("LevelEditor.MainMenu.OctoDen")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOctoDenInputBuilderDefaultsTest,
	"OctoDen.InputBuilder.Defaults",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOctoDenInputBuilderDefaultsTest::RunTest(const FString& Parameters)
{
	UOctoDenInputBuilderSettings* Settings = NewObject<UOctoDenInputBuilderSettings>();
	TestNotNull(TEXT("Input Builder settings object is created"), Settings);
	if (Settings == nullptr)
	{
		return false;
	}

	TestEqual(TEXT("Default IMC prefix"), Settings->InputMappingContextPrefix, FString(TEXT("IMC_")));
	TestEqual(TEXT("Default IA prefix"), Settings->InputActionPrefix, FString(TEXT("IA_")));
	TestEqual(TEXT("Default IMC folder"), Settings->InputMappingContextFolder, FString(TEXT("/Game/Input/Contexts")));
	TestEqual(TEXT("Default IA folder"), Settings->InputActionFolder, FString(TEXT("/Game/Input/Actions")));
	return true;
}

#endif
