#include "GameModes/PSVGameMode.h"

#include "Characters/PSVPlayerCharacter.h"

APSVGameMode::APSVGameMode()
{
    DefaultPawnClass = APSVPlayerCharacter::StaticClass();
}
