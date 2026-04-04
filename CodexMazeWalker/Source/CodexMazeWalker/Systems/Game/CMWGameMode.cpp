#include "Systems/Game/CMWGameMode.h"

#include "Systems/Input/CMWTopDownCharacter.h"
#include "Systems/Input/CMWTopDownPlayerController.h"

ACMWGameMode::ACMWGameMode()
{
	DefaultPawnClass = ACMWTopDownCharacter::StaticClass();
	PlayerControllerClass = ACMWTopDownPlayerController::StaticClass();
}
