// Fill out your copyright notice in the Description page of Project Settings.


#include "CodexInvenBasicMapGameMode.h"

#include "CodexInvenTopDownCharacter.h"
#include "CodexInvenTopDownPlayerController.h"

ACodexInvenBasicMapGameMode::ACodexInvenBasicMapGameMode()
{
	DefaultPawnClass = ACodexInvenTopDownCharacter::StaticClass();
	PlayerControllerClass = ACodexInvenTopDownPlayerController::StaticClass();
}

