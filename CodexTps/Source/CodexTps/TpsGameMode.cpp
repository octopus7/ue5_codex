#include "TpsGameMode.h"
#include "TpsCharacter.h"

ATpsGameMode::ATpsGameMode()
{
    DefaultPawnClass = ATpsCharacter::StaticClass();
}

