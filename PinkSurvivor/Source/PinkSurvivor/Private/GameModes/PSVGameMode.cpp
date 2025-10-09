#include "GameModes/PSVGameMode.h"

#include "Characters/PSVPlayerCharacter.h"
#include "UI/PSVHUD.h"

APSVGameMode::APSVGameMode()
{
    DefaultPawnClass = APSVPlayerCharacter::StaticClass();
    HUDClass = APSVHUD::StaticClass();
}
