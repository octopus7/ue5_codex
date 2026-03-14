#include "OctoDenInputBuilderUtilities.h"

#include "EnhancedInputLibrary.h"
#include "EnhancedActionKeyMapping.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

#define LOCTEXT_NAMESPACE "OctoDenInputBuilderUtilities"

namespace
{
	TArray<FEnhancedActionKeyMapping>& AccessMappings(UInputMappingContext& InInputMappingContext)
	{
		return const_cast<TArray<FEnhancedActionKeyMapping>&>(InInputMappingContext.GetMappings());
	}

	void AddNegateModifier(FEnhancedActionKeyMapping& InMapping, UInputMappingContext& InInputMappingContext, const bool bNegateX, const bool bNegateY)
	{
		UInputModifierNegate* NegateModifier = NewObject<UInputModifierNegate>(&InInputMappingContext);
		NegateModifier->bX = bNegateX;
		NegateModifier->bY = bNegateY;
		NegateModifier->bZ = false;
		InMapping.Modifiers.Add(NegateModifier);
	}

	void AddSwizzleModifier(FEnhancedActionKeyMapping& InMapping, UInputMappingContext& InInputMappingContext)
	{
		UInputModifierSwizzleAxis* SwizzleModifier = NewObject<UInputModifierSwizzleAxis>(&InInputMappingContext);
		SwizzleModifier->Order = EInputAxisSwizzle::YXZ;
		InMapping.Modifiers.Add(SwizzleModifier);
	}

	int32 RemoveNullActionMappings(UInputMappingContext& InInputMappingContext)
	{
		TArray<FEnhancedActionKeyMapping>& Mappings = AccessMappings(InInputMappingContext);
		return Mappings.RemoveAll([](const FEnhancedActionKeyMapping& Mapping)
		{
			return Mapping.Action == nullptr;
		});
	}

	int32 CountMappingsForAction(const UInputMappingContext& InInputMappingContext, const UInputAction& InInputAction)
	{
		int32 Result = 0;
		for (const FEnhancedActionKeyMapping& Mapping : InInputMappingContext.GetMappings())
		{
			if (Mapping.Action == &InInputAction)
			{
				++Result;
			}
		}

		return Result;
	}

	void AddBooleanDraftMappings(UInputMappingContext& InInputMappingContext, UInputAction& InInputAction, const FOctoDenInputBindingDraft& InBindingDraft, int32& OutAddedMappings)
	{
		for (const FKey& Key : InBindingDraft.GetUniqueValidKeys())
		{
			InInputMappingContext.MapKey(&InInputAction, Key);
			++OutAddedMappings;
		}
	}

	void AddMovePresetMappings(UInputMappingContext& InInputMappingContext, UInputAction& InInputAction, int32& OutAddedMappings)
	{
		InInputMappingContext.MapKey(&InInputAction, EKeys::D);
		++OutAddedMappings;

		FEnhancedActionKeyMapping& AMapping = InInputMappingContext.MapKey(&InInputAction, EKeys::A);
		AddNegateModifier(AMapping, InInputMappingContext, true, false);
		++OutAddedMappings;

		FEnhancedActionKeyMapping& WMapping = InInputMappingContext.MapKey(&InInputAction, EKeys::W);
		AddSwizzleModifier(WMapping, InInputMappingContext);
		++OutAddedMappings;

		FEnhancedActionKeyMapping& SMapping = InInputMappingContext.MapKey(&InInputAction, EKeys::S);
		AddSwizzleModifier(SMapping, InInputMappingContext);
		AddNegateModifier(SMapping, InInputMappingContext, false, true);
		++OutAddedMappings;

		InInputMappingContext.MapKey(&InInputAction, EKeys::Gamepad_Left2D);
		++OutAddedMappings;
	}

	void AddLookPresetMappings(UInputMappingContext& InInputMappingContext, UInputAction& InInputAction, int32& OutAddedMappings)
	{
		InInputMappingContext.MapKey(&InInputAction, EKeys::Mouse2D);
		++OutAddedMappings;

		InInputMappingContext.MapKey(&InInputAction, EKeys::Gamepad_Right2D);
		++OutAddedMappings;
	}
}

void OctoDenInputBuilder::ConfigureManagedInputAction(UInputAction& InInputAction, const EOctoDenStandardInputAction InManagedAction)
{
	InInputAction.Modify();
	InInputAction.ValueType = UOctoDenInputBuilderSettings::GetStandardActionValueType(InManagedAction);
	InInputAction.MarkPackageDirty();
}

bool OctoDenInputBuilder::ApplyManagedActionMappings(
	UInputMappingContext& InInputMappingContext,
	UInputAction& InInputAction,
	const EOctoDenStandardInputAction InManagedAction,
	const FOctoDenInputBindingDraft& InBindingDraft,
	FApplyManagedActionMappingsResult& OutResult,
	FText& OutFailReason)
{
	OutResult = FApplyManagedActionMappingsResult();

	if (!UOctoDenInputBuilderSettings::UsesPresetBindings(InManagedAction) && !InBindingDraft.HasAnyValidKey())
	{
		OutFailReason = FText::Format(
			LOCTEXT("ManagedActionRequiresKeys", "{0} requires at least one valid key binding."),
			UOctoDenInputBuilderSettings::GetStandardActionDisplayText(InManagedAction));
		return false;
	}

	InInputMappingContext.Modify();
	OutResult.RemovedNullMappings = RemoveNullActionMappings(InInputMappingContext);
	OutResult.RemovedExistingMappings = CountMappingsForAction(InInputMappingContext, InInputAction);

	if (OutResult.RemovedExistingMappings > 0)
	{
		InInputMappingContext.UnmapAllKeysFromAction(&InInputAction);
	}
	else if (OutResult.RemovedNullMappings > 0)
	{
		UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(&InInputMappingContext, true);
	}

	switch (InManagedAction)
	{
	case EOctoDenStandardInputAction::Move:
		AddMovePresetMappings(InInputMappingContext, InInputAction, OutResult.AddedMappings);
		break;
	case EOctoDenStandardInputAction::Look:
		AddLookPresetMappings(InInputMappingContext, InInputAction, OutResult.AddedMappings);
		break;
	case EOctoDenStandardInputAction::Jump:
	case EOctoDenStandardInputAction::Fire:
		AddBooleanDraftMappings(InInputMappingContext, InInputAction, InBindingDraft, OutResult.AddedMappings);
		break;
	default:
		break;
	}

	InInputMappingContext.MarkPackageDirty();
	OutFailReason = FText::GetEmpty();
	return true;
}

#undef LOCTEXT_NAMESPACE
