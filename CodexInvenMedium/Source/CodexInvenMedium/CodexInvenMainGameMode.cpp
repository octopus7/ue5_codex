#include "CodexInvenMainGameMode.h"

#include "TopDown/CodexInvenTopDownCharacter.h"
#include "TopDown/CodexInvenTopDownPlayerController.h"

ACodexInvenMainGameMode::ACodexInvenMainGameMode()
{
	DefaultPawnClass = ACodexInvenTopDownCharacter::StaticClass();
	PlayerControllerClass = ACodexInvenTopDownPlayerController::StaticClass();
}
