#include "OctoDenInputBuilderSettings.h"

#include "InputMappingContext.h"
#include "Misc/PackageName.h"

#define LOCTEXT_NAMESPACE "OctoDenInputBuilderSettings"

namespace
{
	FString TrimTrailingSlash(FString InValue)
	{
		while (InValue.EndsWith(TEXT("/")))
		{
			InValue.LeftChopInline(1, EAllowShrinking::No);
		}

		return InValue;
	}

	FString NormalizeAssetFolder(FString InFolder)
	{
		InFolder.TrimStartAndEndInline();
		InFolder.ReplaceInline(TEXT("\\"), TEXT("/"));
		return TrimTrailingSlash(MoveTemp(InFolder));
	}
}

bool FOctoDenInputBindingDraft::HasAnyValidKey() const
{
	return PrimaryKey.IsValid() || SecondaryKey.IsValid() || GamepadKey.IsValid();
}

TArray<FKey> FOctoDenInputBindingDraft::GetUniqueValidKeys() const
{
	TArray<FKey> Result;
	TSet<FKey> UniqueKeys;

	auto AddKeyIfValid = [&Result, &UniqueKeys](const FKey& InKey)
	{
		if (InKey.IsValid() && !UniqueKeys.Contains(InKey))
		{
			UniqueKeys.Add(InKey);
			Result.Add(InKey);
		}
	};

	AddKeyIfValid(PrimaryKey);
	AddKeyIfValid(SecondaryKey);
	AddKeyIfValid(GamepadKey);
	return Result;
}

TArray<EOctoDenStandardInputAction> FOctoDenManagedInputAnalysis::GetAvailableActions() const
{
	TArray<EOctoDenStandardInputAction> Result;
	for (const FOctoDenManagedInputActionState& State : ActionStates)
	{
		if (!State.IsAdded())
		{
			Result.Add(State.Action);
		}
	}

	return Result;
}

const FOctoDenManagedInputActionState* FOctoDenManagedInputAnalysis::FindActionState(const EOctoDenStandardInputAction InAction) const
{
	return ActionStates.FindByPredicate([InAction](const FOctoDenManagedInputActionState& State)
	{
		return State.Action == InAction;
	});
}

bool FOctoDenManagedInputAnalysis::HasAvailableActions() const
{
	return ActionStates.ContainsByPredicate([](const FOctoDenManagedInputActionState& State)
	{
		return !State.IsAdded();
	});
}

TArray<EOctoDenStandardInputAction> UOctoDenInputBuilderSettings::GetAllStandardActions()
{
	return {
		EOctoDenStandardInputAction::Move,
		EOctoDenStandardInputAction::Look,
		EOctoDenStandardInputAction::Jump,
		EOctoDenStandardInputAction::Fire
	};
}

FText UOctoDenInputBuilderSettings::GetStandardActionDisplayText(const EOctoDenStandardInputAction InAction)
{
	switch (InAction)
	{
	case EOctoDenStandardInputAction::Move:
		return LOCTEXT("MoveActionLabel", "Move");
	case EOctoDenStandardInputAction::Look:
		return LOCTEXT("LookActionLabel", "Look");
	case EOctoDenStandardInputAction::Jump:
		return LOCTEXT("JumpActionLabel", "Jump");
	case EOctoDenStandardInputAction::Fire:
		return LOCTEXT("FireActionLabel", "Fire");
	default:
		return LOCTEXT("UnknownActionLabel", "Unknown");
	}
}

FString UOctoDenInputBuilderSettings::GetStandardActionStem(const EOctoDenStandardInputAction InAction)
{
	switch (InAction)
	{
	case EOctoDenStandardInputAction::Move:
		return TEXT("Move");
	case EOctoDenStandardInputAction::Look:
		return TEXT("Look");
	case EOctoDenStandardInputAction::Jump:
		return TEXT("Jump");
	case EOctoDenStandardInputAction::Fire:
		return TEXT("Fire");
	default:
		return TEXT("Action");
	}
}

EInputActionValueType UOctoDenInputBuilderSettings::GetStandardActionValueType(const EOctoDenStandardInputAction InAction)
{
	switch (InAction)
	{
	case EOctoDenStandardInputAction::Move:
	case EOctoDenStandardInputAction::Look:
		return EInputActionValueType::Axis2D;
	case EOctoDenStandardInputAction::Jump:
	case EOctoDenStandardInputAction::Fire:
		return EInputActionValueType::Boolean;
	default:
		return EInputActionValueType::Boolean;
	}
}

bool UOctoDenInputBuilderSettings::UsesPresetBindings(const EOctoDenStandardInputAction InAction)
{
	return InAction == EOctoDenStandardInputAction::Move || InAction == EOctoDenStandardInputAction::Look;
}

FText UOctoDenInputBuilderSettings::GetPresetBindingSummary(const EOctoDenStandardInputAction InAction)
{
	switch (InAction)
	{
	case EOctoDenStandardInputAction::Move:
		return LOCTEXT("MovePresetSummary", "Preset bindings: WASD and Gamepad Left Stick.");
	case EOctoDenStandardInputAction::Look:
		return LOCTEXT("LookPresetSummary", "Preset bindings: Mouse2D and Gamepad Right Stick.");
	default:
		return FText::GetEmpty();
	}
}

FOctoDenManagedInputAnalysis UOctoDenInputBuilderSettings::AnalyzeSelectedInputMappingContext() const
{
	FOctoDenManagedInputAnalysis Result;
	Result.bHasSelectedInputMappingContext = SelectedInputMappingContext != nullptr;

	for (const EOctoDenStandardInputAction Action : GetAllStandardActions())
	{
		FOctoDenManagedInputActionState& State = Result.ActionStates.AddDefaulted_GetRef();
		State.Action = Action;
	}

	if (SelectedInputMappingContext == nullptr)
	{
		return Result;
	}

	for (const FEnhancedActionKeyMapping& Mapping : SelectedInputMappingContext->GetMappings())
	{
		if (Mapping.Action == nullptr)
		{
			++Result.NullActionMappingCount;
			continue;
		}

		if (!Mapping.Key.IsValid())
		{
			continue;
		}

		const FString MappingPackagePath = Mapping.Action->GetOutermost()->GetName();
		for (FOctoDenManagedInputActionState& State : Result.ActionStates)
		{
			if (MappingPackagePath == GetCanonicalInputActionPackagePath(State.Action))
			{
				++State.ValidMappingCount;
				break;
			}
		}
	}

	return Result;
}

bool UOctoDenInputBuilderSettings::ResolveSelectedAction(EOctoDenStandardInputAction& OutAction) const
{
	const FOctoDenManagedInputAnalysis Analysis = AnalyzeSelectedInputMappingContext();
	if (!Analysis.bHasSelectedInputMappingContext || !Analysis.HasAvailableActions())
	{
		return false;
	}

	if (const FOctoDenManagedInputActionState* SelectedState = Analysis.FindActionState(SelectedAction))
	{
		if (!SelectedState->IsAdded())
		{
			OutAction = SelectedAction;
			return true;
		}
	}

	for (const FOctoDenManagedInputActionState& State : Analysis.ActionStates)
	{
		if (!State.IsAdded())
		{
			OutAction = State.Action;
			return true;
		}
	}

	return false;
}

bool UOctoDenInputBuilderSettings::HasSelectedInputMappingContext() const
{
	return SelectedInputMappingContext != nullptr;
}

bool UOctoDenInputBuilderSettings::CanAddSelectedAction(FText* OutFailReason) const
{
	if (SelectedInputMappingContext == nullptr)
	{
		if (OutFailReason != nullptr)
		{
			*OutFailReason = LOCTEXT("MissingSelectedIMC", "Select an existing Input Mapping Context.");
		}
		return false;
	}

	const FString NormalizedFolder = NormalizeAssetFolder(InputActionFolder);
	if (NormalizedFolder.IsEmpty() || !FPackageName::IsValidLongPackageName(NormalizedFolder))
	{
		if (OutFailReason != nullptr)
		{
			*OutFailReason = LOCTEXT("InvalidInputActionFolder", "The Input Action folder must be a valid package path such as /Game/Input/Actions.");
		}
		return false;
	}

	EOctoDenStandardInputAction ActionToAdd = EOctoDenStandardInputAction::Move;
	if (!ResolveSelectedAction(ActionToAdd))
	{
		if (OutFailReason != nullptr)
		{
			*OutFailReason = LOCTEXT("NoResolvableAction", "There is no managed action available to add.");
		}
		return false;
	}

	if (!UsesPresetBindings(ActionToAdd) && !GetBindingDraft(ActionToAdd).HasAnyValidKey())
	{
		if (OutFailReason != nullptr)
		{
			*OutFailReason = FText::Format(
				LOCTEXT("MissingDraftBindings", "{0} requires at least one valid key binding."),
				GetStandardActionDisplayText(ActionToAdd));
		}
		return false;
	}

	if (OutFailReason != nullptr)
	{
		*OutFailReason = FText::GetEmpty();
	}

	return true;
}

bool UOctoDenInputBuilderSettings::SelectedActionUsesPresetBindings() const
{
	EOctoDenStandardInputAction ResolvedAction = EOctoDenStandardInputAction::Move;
	return ResolveSelectedAction(ResolvedAction) && UsesPresetBindings(ResolvedAction);
}

FString UOctoDenInputBuilderSettings::GetCanonicalInputActionName(const EOctoDenStandardInputAction InAction) const
{
	return InputActionPrefix + GetStandardActionStem(InAction);
}

FString UOctoDenInputBuilderSettings::GetCanonicalInputActionPackagePath(const EOctoDenStandardInputAction InAction) const
{
	return NormalizeAssetFolder(InputActionFolder) / GetCanonicalInputActionName(InAction);
}

FString UOctoDenInputBuilderSettings::GetCanonicalInputActionObjectPath(const EOctoDenStandardInputAction InAction) const
{
	const FString PackagePath = GetCanonicalInputActionPackagePath(InAction);
	const FString AssetName = GetCanonicalInputActionName(InAction);
	return PackagePath + TEXT(".") + AssetName;
}

const FOctoDenInputBindingDraft& UOctoDenInputBuilderSettings::GetBindingDraft(const EOctoDenStandardInputAction InAction) const
{
	switch (InAction)
	{
	case EOctoDenStandardInputAction::Jump:
		return JumpBindings;
	case EOctoDenStandardInputAction::Fire:
		return FireBindings;
	default:
		return JumpBindings;
	}
}

FOctoDenInputBindingDraft& UOctoDenInputBuilderSettings::GetBindingDraft(const EOctoDenStandardInputAction InAction)
{
	switch (InAction)
	{
	case EOctoDenStandardInputAction::Jump:
		return JumpBindings;
	case EOctoDenStandardInputAction::Fire:
		return FireBindings;
	default:
		return JumpBindings;
	}
}

#undef LOCTEXT_NAMESPACE
