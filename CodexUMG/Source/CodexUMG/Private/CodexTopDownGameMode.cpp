// Copyright Epic Games, Inc. All Rights Reserved.

#include "CodexTopDownGameMode.h"

#include "CodexTopDownCharacter.h"
#include "CodexTopDownPlayerController.h"

ACodexTopDownGameMode::ACodexTopDownGameMode()
{
	DefaultPawnClass = ACodexTopDownCharacter::StaticClass();
	PlayerControllerClass = ACodexTopDownPlayerController::StaticClass();
}
