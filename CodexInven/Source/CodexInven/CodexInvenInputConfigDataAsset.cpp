// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenInputConfigDataAsset.h"

#include "InputAction.h"
#include "InputMappingContext.h"

const UInputAction* UCodexInvenInputConfigDataAsset::GetInputAction(const ECodexInvenConfiguredInputAction InAction) const
{
	switch (InAction)
	{
	case ECodexInvenConfiguredInputAction::Move:
		return MoveAction;
	case ECodexInvenConfiguredInputAction::Look:
		return LookAction;
	case ECodexInvenConfiguredInputAction::Jump:
		return JumpAction;
	case ECodexInvenConfiguredInputAction::Fire:
		return FireAction;
	default:
		return nullptr;
	}
}

bool UCodexInvenInputConfigDataAsset::HasRequiredBindings() const
{
	return InputMappingContext != nullptr
		&& MoveAction != nullptr
		&& LookAction != nullptr
		&& JumpAction != nullptr
		&& FireAction != nullptr;
}
