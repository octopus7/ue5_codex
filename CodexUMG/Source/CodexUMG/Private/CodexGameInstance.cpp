// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodexGameInstance.h"

#include "CodexProjectileConfigDataAsset.h"
#include "CodexTopDownInputConfigDataAsset.h"

const UCodexTopDownInputConfigDataAsset* UCodexGameInstance::GetTopDownInputConfig() const
{
	return TopDownInputConfig;
}

void UCodexGameInstance::SetTopDownInputConfig(UCodexTopDownInputConfigDataAsset* NewConfig)
{
	TopDownInputConfig = NewConfig;
}

const UCodexProjectileConfigDataAsset* UCodexGameInstance::GetPlayerProjectileConfig() const
{
	return PlayerProjectileConfig;
}

void UCodexGameInstance::SetPlayerProjectileConfig(UCodexProjectileConfigDataAsset* NewConfig)
{
	PlayerProjectileConfig = NewConfig;
}
