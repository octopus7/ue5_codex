#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "EnhancedActionKeyMapping.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Misc/PackageName.h"
#include "Modules/ModuleManager.h"
#include "OctoDenInputBuilderSettings.h"
#include "OctoDenInputBuilderUtilities.h"
#include "OctoDenModule.h"
#include "ToolMenus.h"
#include "UObject/Package.h"

namespace
{
	TArray<FEnhancedActionKeyMapping>& AccessTestMappings(UInputMappingContext& InInputMappingContext)
	{
		return const_cast<TArray<FEnhancedActionKeyMapping>&>(InInputMappingContext.GetMappings());
	}

	FString MakeAutomationActionFolder(const FString& InStem)
	{
		return FString::Printf(TEXT("/Game/OctoDenAutomation/%s"), *InStem);
	}

	UInputAction* FindOrCreateInputActionAtPackagePath(const FString& InPackagePath)
	{
		const FString AssetName = FPackageName::GetLongPackageAssetName(InPackagePath);
		UPackage* const Package = CreatePackage(*InPackagePath);
		if (UInputAction* ExistingAction = FindObject<UInputAction>(Package, *AssetName))
		{
			return ExistingAction;
		}

		return NewObject<UInputAction>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
	}

	const FEnhancedActionKeyMapping* FindMapping(const UInputMappingContext& InInputMappingContext, const UInputAction* InInputAction, const FKey& InKey)
	{
		return InInputMappingContext.GetMappings().FindByPredicate([InInputAction, &InKey](const FEnhancedActionKeyMapping& Mapping)
		{
			return Mapping.Action == InInputAction && Mapping.Key == InKey;
		});
	}
}

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

	TestNull(TEXT("Default selected IMC"), Settings->SelectedInputMappingContext);
	TestEqual(TEXT("Default IA prefix"), Settings->InputActionPrefix, FString(TEXT("IA_")));
	TestEqual(TEXT("Default IA folder"), Settings->InputActionFolder, FString(TEXT("/Game/Input/Actions")));
	TestEqual(TEXT("Default Input Config folder"), Settings->InputConfigFolder, FString(TEXT("/Game/Input/Configs")));
	TestEqual(TEXT("Default Input Config asset name"), Settings->InputConfigAssetName, FString(TEXT("DA_DefaultInputConfig")));
	TestEqual(TEXT("Default Input Config package path"), Settings->GetInputConfigPackagePath(), FString(TEXT("/Game/Input/Configs/DA_DefaultInputConfig")));
	TestEqual(TEXT("Default selected action"), Settings->SelectedAction, EOctoDenStandardInputAction::Move);
	TestTrue(TEXT("Jump draft starts with defaults"), Settings->JumpBindings.HasAnyValidKey());
	TestEqual(TEXT("Jump default primary key"), Settings->JumpBindings.PrimaryKey, EKeys::SpaceBar);
	TestEqual(TEXT("Jump default gamepad key"), Settings->JumpBindings.GamepadKey, EKeys::Gamepad_FaceButton_Bottom);
	TestTrue(TEXT("Fire draft starts with defaults"), Settings->FireBindings.HasAnyValidKey());
	TestEqual(TEXT("Fire default primary key"), Settings->FireBindings.PrimaryKey, EKeys::LeftMouseButton);
	TestEqual(TEXT("Fire default gamepad key"), Settings->FireBindings.GamepadKey, EKeys::Gamepad_RightTrigger);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOctoDenInputBuilderAvailabilityTest,
	"OctoDen.InputBuilder.Availability",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOctoDenInputBuilderAvailabilityTest::RunTest(const FString& Parameters)
{
	UOctoDenInputBuilderSettings* Settings = NewObject<UOctoDenInputBuilderSettings>();
	UInputMappingContext* InputMappingContext = NewObject<UInputMappingContext>();
	TestNotNull(TEXT("Settings created"), Settings);
	TestNotNull(TEXT("Input Mapping Context created"), InputMappingContext);
	if (Settings == nullptr || InputMappingContext == nullptr)
	{
		return false;
	}

	Settings->InputActionFolder = MakeAutomationActionFolder(TEXT("Availability"));
	Settings->SelectedInputMappingContext = InputMappingContext;

	FOctoDenManagedInputAnalysis Analysis = Settings->AnalyzeSelectedInputMappingContext();
	TestEqual(TEXT("Empty IMC exposes four actions"), Analysis.GetAvailableActions().Num(), 4);
	TestFalse(TEXT("Runtime link is unavailable until all actions exist"), Settings->CanLinkRuntimeInputConfig());

	UInputAction* MoveAction = FindOrCreateInputActionAtPackagePath(Settings->GetCanonicalInputActionPackagePath(EOctoDenStandardInputAction::Move));
	TestNotNull(TEXT("Move action asset created"), MoveAction);
	InputMappingContext->MapKey(MoveAction, EKeys::D);

	Analysis = Settings->AnalyzeSelectedInputMappingContext();
	TestEqual(TEXT("Move becomes unavailable once mapped"), Analysis.GetAvailableActions().Num(), 3);
	const FOctoDenManagedInputActionState* MoveState = Analysis.FindActionState(EOctoDenStandardInputAction::Move);
	TestNotNull(TEXT("Move state exists"), MoveState);
	if (MoveState != nullptr)
	{
		TestTrue(TEXT("Move is treated as added"), MoveState->IsAdded());
	}

	AccessTestMappings(*InputMappingContext).Add(FEnhancedActionKeyMapping(nullptr, EKeys::LeftMouseButton));
	Analysis = Settings->AnalyzeSelectedInputMappingContext();
	TestEqual(TEXT("Broken null mappings do not consume an action slot"), Analysis.GetAvailableActions().Num(), 3);
	TestEqual(TEXT("Broken null mappings are counted"), Analysis.NullActionMappingCount, 1);

	for (const EOctoDenStandardInputAction Action : UOctoDenInputBuilderSettings::GetAllStandardActions())
	{
		if (Action == EOctoDenStandardInputAction::Move)
		{
			continue;
		}

		UInputAction* ManagedAction = FindOrCreateInputActionAtPackagePath(Settings->GetCanonicalInputActionPackagePath(Action));
		TestNotNull(TEXT("Managed action asset created"), ManagedAction);
		if (ManagedAction == nullptr)
		{
			return false;
		}

		const FKey KeyToUse = Action == EOctoDenStandardInputAction::Look
			? EKeys::Mouse2D
			: (Action == EOctoDenStandardInputAction::Jump ? EKeys::SpaceBar : EKeys::LeftMouseButton);
		InputMappingContext->MapKey(ManagedAction, KeyToUse);
	}

	Analysis = Settings->AnalyzeSelectedInputMappingContext();
	TestEqual(TEXT("All four actions mapped leaves no remaining actions"), Analysis.GetAvailableActions().Num(), 0);
	TestTrue(TEXT("Runtime link becomes available once all actions are present"), Settings->CanLinkRuntimeInputConfig());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOctoDenInputBuilderJumpRepairTest,
	"OctoDen.InputBuilder.ApplyJumpRepair",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOctoDenInputBuilderJumpRepairTest::RunTest(const FString& Parameters)
{
	UOctoDenInputBuilderSettings* Settings = NewObject<UOctoDenInputBuilderSettings>();
	UInputMappingContext* InputMappingContext = NewObject<UInputMappingContext>();
	TestNotNull(TEXT("Settings created"), Settings);
	TestNotNull(TEXT("Input Mapping Context created"), InputMappingContext);
	if (Settings == nullptr || InputMappingContext == nullptr)
	{
		return false;
	}

	Settings->InputActionFolder = MakeAutomationActionFolder(TEXT("JumpRepair"));
	Settings->SelectedInputMappingContext = InputMappingContext;

	UInputAction* JumpAction = FindOrCreateInputActionAtPackagePath(Settings->GetCanonicalInputActionPackagePath(EOctoDenStandardInputAction::Jump));
	TestNotNull(TEXT("Jump action asset created"), JumpAction);
	if (JumpAction == nullptr)
	{
		return false;
	}

	JumpAction->ValueType = EInputActionValueType::Axis3D;
	InputMappingContext->MapKey(JumpAction, EKeys::Q);
	AccessTestMappings(*InputMappingContext).Add(FEnhancedActionKeyMapping(nullptr, EKeys::R));

	Settings->JumpBindings.PrimaryKey = EKeys::SpaceBar;
	Settings->JumpBindings.GamepadKey = EKeys::Gamepad_FaceButton_Bottom;

	OctoDenInputBuilder::ConfigureManagedInputAction(*JumpAction, EOctoDenStandardInputAction::Jump);
	TestEqual(TEXT("Existing Jump IA is forced back to Boolean"), JumpAction->ValueType, EInputActionValueType::Boolean);

	OctoDenInputBuilder::FApplyManagedActionMappingsResult ApplyResult;
	FText FailReason;
	const bool bApplySucceeded = OctoDenInputBuilder::ApplyManagedActionMappings(
		*InputMappingContext,
		*JumpAction,
		EOctoDenStandardInputAction::Jump,
		Settings->JumpBindings,
		ApplyResult,
		FailReason);

	TestTrue(TEXT("Jump repair succeeds"), bApplySucceeded);
	TestTrue(TEXT("Jump repair fail reason stays empty"), FailReason.IsEmpty());
	TestEqual(TEXT("Broken null mappings are removed"), ApplyResult.RemovedNullMappings, 1);
	TestEqual(TEXT("Old Jump mappings are replaced"), ApplyResult.RemovedExistingMappings, 1);
	TestEqual(TEXT("Only requested jump keys are added"), ApplyResult.AddedMappings, 2);
	TestNull(TEXT("Old jump key was removed"), FindMapping(*InputMappingContext, JumpAction, EKeys::Q));
	TestNotNull(TEXT("Primary jump key exists"), FindMapping(*InputMappingContext, JumpAction, EKeys::SpaceBar));
	TestNotNull(TEXT("Gamepad jump key exists"), FindMapping(*InputMappingContext, JumpAction, EKeys::Gamepad_FaceButton_Bottom));
	TestFalse(TEXT("Broken null mappings are gone"), InputMappingContext->GetMappings().ContainsByPredicate([](const FEnhancedActionKeyMapping& Mapping)
	{
		return Mapping.Action == nullptr;
	}));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOctoDenInputBuilderMovePresetTest,
	"OctoDen.InputBuilder.ApplyMovePreset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOctoDenInputBuilderMovePresetTest::RunTest(const FString& Parameters)
{
	UOctoDenInputBuilderSettings* Settings = NewObject<UOctoDenInputBuilderSettings>();
	UInputMappingContext* InputMappingContext = NewObject<UInputMappingContext>();
	TestNotNull(TEXT("Settings created"), Settings);
	TestNotNull(TEXT("Input Mapping Context created"), InputMappingContext);
	if (Settings == nullptr || InputMappingContext == nullptr)
	{
		return false;
	}

	Settings->InputActionFolder = MakeAutomationActionFolder(TEXT("MovePreset"));
	Settings->SelectedInputMappingContext = InputMappingContext;

	UInputAction* MoveAction = FindOrCreateInputActionAtPackagePath(Settings->GetCanonicalInputActionPackagePath(EOctoDenStandardInputAction::Move));
	TestNotNull(TEXT("Move action asset created"), MoveAction);
	if (MoveAction == nullptr)
	{
		return false;
	}

	MoveAction->ValueType = EInputActionValueType::Boolean;
	OctoDenInputBuilder::ConfigureManagedInputAction(*MoveAction, EOctoDenStandardInputAction::Move);
	TestEqual(TEXT("Move IA uses Axis2D"), MoveAction->ValueType, EInputActionValueType::Axis2D);

	OctoDenInputBuilder::FApplyManagedActionMappingsResult ApplyResult;
	FText FailReason;
	const bool bApplySucceeded = OctoDenInputBuilder::ApplyManagedActionMappings(
		*InputMappingContext,
		*MoveAction,
		EOctoDenStandardInputAction::Move,
		FOctoDenInputBindingDraft(),
		ApplyResult,
		FailReason);

	TestTrue(TEXT("Move preset apply succeeds"), bApplySucceeded);
	TestEqual(TEXT("Move preset creates five mappings"), ApplyResult.AddedMappings, 5);

	const FEnhancedActionKeyMapping* DMapping = FindMapping(*InputMappingContext, MoveAction, EKeys::D);
	const FEnhancedActionKeyMapping* AMapping = FindMapping(*InputMappingContext, MoveAction, EKeys::A);
	const FEnhancedActionKeyMapping* WMapping = FindMapping(*InputMappingContext, MoveAction, EKeys::W);
	const FEnhancedActionKeyMapping* SMapping = FindMapping(*InputMappingContext, MoveAction, EKeys::S);
	const FEnhancedActionKeyMapping* GamepadMapping = FindMapping(*InputMappingContext, MoveAction, EKeys::Gamepad_Left2D);

	TestNotNull(TEXT("D mapping exists"), DMapping);
	TestNotNull(TEXT("A mapping exists"), AMapping);
	TestNotNull(TEXT("W mapping exists"), WMapping);
	TestNotNull(TEXT("S mapping exists"), SMapping);
	TestNotNull(TEXT("Gamepad move mapping exists"), GamepadMapping);

	if (DMapping != nullptr)
	{
		TestEqual(TEXT("D mapping has no modifiers"), DMapping->Modifiers.Num(), 0);
	}

	if (AMapping != nullptr)
	{
		TestEqual(TEXT("A mapping has one modifier"), AMapping->Modifiers.Num(), 1);
		const UInputModifierNegate* NegateModifier = Cast<UInputModifierNegate>(AMapping->Modifiers[0]);
		TestNotNull(TEXT("A mapping uses negate"), NegateModifier);
		if (NegateModifier != nullptr)
		{
			TestTrue(TEXT("A negate flips X"), NegateModifier->bX);
			TestFalse(TEXT("A negate does not flip Y"), NegateModifier->bY);
		}
	}

	if (WMapping != nullptr)
	{
		TestEqual(TEXT("W mapping has one modifier"), WMapping->Modifiers.Num(), 1);
		const UInputModifierSwizzleAxis* SwizzleModifier = Cast<UInputModifierSwizzleAxis>(WMapping->Modifiers[0]);
		TestNotNull(TEXT("W mapping uses swizzle"), SwizzleModifier);
		if (SwizzleModifier != nullptr)
		{
			TestEqual(TEXT("W swizzle uses YXZ"), SwizzleModifier->Order, EInputAxisSwizzle::YXZ);
		}
	}

	if (SMapping != nullptr)
	{
		TestEqual(TEXT("S mapping has two modifiers"), SMapping->Modifiers.Num(), 2);
		const UInputModifierSwizzleAxis* SwizzleModifier = Cast<UInputModifierSwizzleAxis>(SMapping->Modifiers[0]);
		const UInputModifierNegate* NegateModifier = Cast<UInputModifierNegate>(SMapping->Modifiers[1]);
		TestNotNull(TEXT("S mapping keeps swizzle"), SwizzleModifier);
		TestNotNull(TEXT("S mapping keeps negate"), NegateModifier);
		if (NegateModifier != nullptr)
		{
			TestFalse(TEXT("S negate does not flip X"), NegateModifier->bX);
			TestTrue(TEXT("S negate flips Y"), NegateModifier->bY);
		}
	}

	FOctoDenManagedInputAnalysis Analysis = Settings->AnalyzeSelectedInputMappingContext();
	const FOctoDenManagedInputActionState* MoveState = Analysis.FindActionState(EOctoDenStandardInputAction::Move);
	TestNotNull(TEXT("Move state exists"), MoveState);
	if (MoveState != nullptr)
	{
		TestTrue(TEXT("Move is unavailable after preset apply"), MoveState->IsAdded());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FOctoDenInputBuilderReaddFireTest,
	"OctoDen.InputBuilder.ReAddAfterRemoval",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOctoDenInputBuilderReaddFireTest::RunTest(const FString& Parameters)
{
	UOctoDenInputBuilderSettings* Settings = NewObject<UOctoDenInputBuilderSettings>();
	UInputMappingContext* InputMappingContext = NewObject<UInputMappingContext>();
	TestNotNull(TEXT("Settings created"), Settings);
	TestNotNull(TEXT("Input Mapping Context created"), InputMappingContext);
	if (Settings == nullptr || InputMappingContext == nullptr)
	{
		return false;
	}

	Settings->InputActionFolder = MakeAutomationActionFolder(TEXT("ReAddFire"));
	Settings->SelectedInputMappingContext = InputMappingContext;

	UInputAction* FireAction = FindOrCreateInputActionAtPackagePath(Settings->GetCanonicalInputActionPackagePath(EOctoDenStandardInputAction::Fire));
	TestNotNull(TEXT("Fire action asset created"), FireAction);
	if (FireAction == nullptr)
	{
		return false;
	}

	OctoDenInputBuilder::ConfigureManagedInputAction(*FireAction, EOctoDenStandardInputAction::Fire);

	Settings->FireBindings.PrimaryKey = EKeys::LeftMouseButton;
	Settings->FireBindings.GamepadKey = EKeys::Gamepad_RightTrigger;

	OctoDenInputBuilder::FApplyManagedActionMappingsResult ApplyResult;
	FText FailReason;
	TestTrue(TEXT("Initial fire apply succeeds"), OctoDenInputBuilder::ApplyManagedActionMappings(
		*InputMappingContext,
		*FireAction,
		EOctoDenStandardInputAction::Fire,
		Settings->FireBindings,
		ApplyResult,
		FailReason));

	FOctoDenManagedInputAnalysis Analysis = Settings->AnalyzeSelectedInputMappingContext();
	const FOctoDenManagedInputActionState* FireState = Analysis.FindActionState(EOctoDenStandardInputAction::Fire);
	TestNotNull(TEXT("Fire state exists"), FireState);
	if (FireState != nullptr)
	{
		TestTrue(TEXT("Fire is unavailable after initial add"), FireState->IsAdded());
	}

	InputMappingContext->UnmapAllKeysFromAction(FireAction);
	Analysis = Settings->AnalyzeSelectedInputMappingContext();
	FireState = Analysis.FindActionState(EOctoDenStandardInputAction::Fire);
	TestNotNull(TEXT("Fire state still exists"), FireState);
	if (FireState != nullptr)
	{
		TestFalse(TEXT("Fire becomes available after IMC removal"), FireState->IsAdded());
	}

	Settings->FireBindings = FOctoDenInputBindingDraft();
	Settings->FireBindings.PrimaryKey = EKeys::RightMouseButton;

	TestTrue(TEXT("Fire can be re-added after removal"), OctoDenInputBuilder::ApplyManagedActionMappings(
		*InputMappingContext,
		*FireAction,
		EOctoDenStandardInputAction::Fire,
		Settings->FireBindings,
		ApplyResult,
		FailReason));
	TestNotNull(TEXT("Default fire mouse key is restored"), FindMapping(*InputMappingContext, FireAction, EKeys::LeftMouseButton));
	TestNotNull(TEXT("Default fire gamepad key is restored"), FindMapping(*InputMappingContext, FireAction, EKeys::Gamepad_RightTrigger));
	TestNull(TEXT("Hidden custom fire key is ignored"), FindMapping(*InputMappingContext, FireAction, EKeys::RightMouseButton));
	return true;
}

#endif
