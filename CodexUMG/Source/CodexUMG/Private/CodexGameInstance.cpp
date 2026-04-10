// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodexGameInstance.h"

#include "CodexTopDownInputConfigDataAsset.h"

const UCodexTopDownInputConfigDataAsset* UCodexGameInstance::GetTopDownInputConfig() const
{
	return TopDownInputConfig;
}

void UCodexGameInstance::SetTopDownInputConfig(UCodexTopDownInputConfigDataAsset* NewConfig)
{
	TopDownInputConfig = NewConfig;
}
