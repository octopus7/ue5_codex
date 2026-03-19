#include "InputBuilder/OctoDenInputMappingRules.h"

#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputLibrary.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

namespace
{
	TArray<FEnhancedActionKeyMapping>& AccessMappings(UInputMappingContext& InputMappingContext)
	{
		return const_cast<TArray<FEnhancedActionKeyMapping>&>(InputMappingContext.GetMappings());
	}

	int32 RemoveNullMappings(UInputMappingContext& InputMappingContext)
	{
		TArray<FEnhancedActionKeyMapping>& Mappings = AccessMappings(InputMappingContext);
		return Mappings.RemoveAll([](const FEnhancedActionKeyMapping& Mapping)
		{
			return Mapping.Action == nullptr;
		});
	}

	int32 CountMappingsForAction(const UInputMappingContext& InputMappingContext, const UInputAction& InputAction)
	{
		int32 Count = 0;
		for (const FEnhancedActionKeyMapping& Mapping : InputMappingContext.GetMappings())
		{
			if (Mapping.Action == &InputAction)
			{
				++Count;
			}
		}
		return Count;
	}

	void AddNegate(FEnhancedActionKeyMapping& Mapping, UObject& Outer, const bool bNegateX, const bool bNegateY)
	{
		UInputModifierNegate* Modifier = NewObject<UInputModifierNegate>(&Outer);
		Modifier->bX = bNegateX;
		Modifier->bY = bNegateY;
		Mapping.Modifiers.Add(Modifier);
	}

	void AddSwizzle(FEnhancedActionKeyMapping& Mapping, UObject& Outer)
	{
		UInputModifierSwizzleAxis* Modifier = NewObject<UInputModifierSwizzleAxis>(&Outer);
		Modifier->Order = EInputAxisSwizzle::YXZ;
		Mapping.Modifiers.Add(Modifier);
	}
}

void OctoDenInputMappingRules::ConfigureInputAction(UInputAction& InputAction, const EOctoDenManagedInputAction ManagedAction)
{
	InputAction.Modify();
	InputAction.ValueType = UOctoDenInputBuilderSettings::GetManagedValueType(ManagedAction);
	InputAction.MarkPackageDirty();
}

bool OctoDenInputMappingRules::ApplyMappings(UInputMappingContext& InputMappingContext, UInputAction& InputAction, const EOctoDenManagedInputAction ManagedAction, const FOctoDenKeyDraft& Draft, FOctoDenManagedActionBuildResult& OutResult, FText& OutFailure)
{
	OutResult = FOctoDenManagedActionBuildResult();
	FOctoDenKeyDraft EffectiveDraft = Draft;

	if (!UOctoDenInputBuilderSettings::UsesPresetMappings(ManagedAction) && !EffectiveDraft.HasAnyValidKey())
	{
		OutFailure = FText::Format(
			NSLOCTEXT("OctoDenMappingRules", "MissingKeys", "{0} requires at least one valid key."),
			UOctoDenInputBuilderSettings::GetManagedActionLabel(ManagedAction));
		return false;
	}

	if (ManagedAction == EOctoDenManagedInputAction::Jump || ManagedAction == EOctoDenManagedInputAction::Fire)
	{
		EffectiveDraft = UOctoDenInputBuilderSettings::MakeDefaultDraft(ManagedAction);
	}

	InputMappingContext.Modify();
	OutResult.RemovedNullMappings = RemoveNullMappings(InputMappingContext);
	OutResult.RemovedExistingMappings = CountMappingsForAction(InputMappingContext, InputAction);
	if (OutResult.RemovedExistingMappings > 0)
	{
		InputMappingContext.UnmapAllKeysFromAction(&InputAction);
	}
	else if (OutResult.RemovedNullMappings > 0)
	{
		UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(&InputMappingContext, true);
	}

	switch (ManagedAction)
	{
	case EOctoDenManagedInputAction::Move:
		InputMappingContext.MapKey(&InputAction, EKeys::D);
		++OutResult.AddedMappings;
		{
			FEnhancedActionKeyMapping& A = InputMappingContext.MapKey(&InputAction, EKeys::A);
			AddNegate(A, InputMappingContext, true, false);
			++OutResult.AddedMappings;
		}
		{
			FEnhancedActionKeyMapping& W = InputMappingContext.MapKey(&InputAction, EKeys::W);
			AddSwizzle(W, InputMappingContext);
			++OutResult.AddedMappings;
		}
		{
			FEnhancedActionKeyMapping& S = InputMappingContext.MapKey(&InputAction, EKeys::S);
			AddSwizzle(S, InputMappingContext);
			AddNegate(S, InputMappingContext, false, true);
			++OutResult.AddedMappings;
		}
		InputMappingContext.MapKey(&InputAction, EKeys::Gamepad_Left2D);
		++OutResult.AddedMappings;
		break;
	case EOctoDenManagedInputAction::Look:
		InputMappingContext.MapKey(&InputAction, EKeys::Mouse2D);
		InputMappingContext.MapKey(&InputAction, EKeys::Gamepad_Right2D);
		OutResult.AddedMappings += 2;
		break;
	case EOctoDenManagedInputAction::Jump:
	case EOctoDenManagedInputAction::Fire:
		for (const FKey& Key : EffectiveDraft.GetUniqueValidKeys())
		{
			InputMappingContext.MapKey(&InputAction, Key);
			++OutResult.AddedMappings;
		}
		break;
	default:
		break;
	}

	InputMappingContext.MarkPackageDirty();
	return true;
}
