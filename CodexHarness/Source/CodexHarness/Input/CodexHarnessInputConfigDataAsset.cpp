#include "Input/CodexHarnessInputConfigDataAsset.h"

#include "InputAction.h"

const UInputAction* UCodexHarnessInputConfigDataAsset::GetInputAction(const ECodexHarnessConfiguredInputAction InAction) const
{
	switch (InAction)
	{
	case ECodexHarnessConfiguredInputAction::Move:
		return MoveAction;
	case ECodexHarnessConfiguredInputAction::Look:
		return LookAction;
	case ECodexHarnessConfiguredInputAction::Fire:
		return FireAction;
	case ECodexHarnessConfiguredInputAction::Restart:
		return RestartAction;
	default:
		return nullptr;
	}
}

bool UCodexHarnessInputConfigDataAsset::HasRequiredBindings() const
{
	return InputMappingContext != nullptr
		&& MoveAction != nullptr
		&& LookAction != nullptr
		&& FireAction != nullptr
		&& RestartAction != nullptr;
}
