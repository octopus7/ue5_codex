// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodexTopDownInputConfigDataAsset.h"

const UInputMappingContext* UCodexTopDownInputConfigDataAsset::GetDefaultMappingContext() const
{
	return DefaultMappingContext;
}

const UInputAction* UCodexTopDownInputConfigDataAsset::GetMoveAction() const
{
	return MoveAction;
}

const UInputAction* UCodexTopDownInputConfigDataAsset::GetFireAction() const
{
	return FireAction;
}

const UInputAction* UCodexTopDownInputConfigDataAsset::GetInteractAction() const
{
	return InteractAction;
}

int32 UCodexTopDownInputConfigDataAsset::GetMappingPriority() const
{
	return MappingPriority;
}
