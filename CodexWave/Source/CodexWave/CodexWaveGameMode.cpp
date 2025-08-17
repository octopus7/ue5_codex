#include "CodexWaveGameMode.h"
#include "CubePlayerPawn.h"

ACodexWaveGameMode::ACodexWaveGameMode()
{
    DefaultPawnClass = ACubePlayerPawn::StaticClass();
}

