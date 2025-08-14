// CodexTpsGameModeBase.cpp

#include "GameModes/CodexTpsGameModeBase.h"
#include "Characters/TPSCharacter.h"

ACodexTpsGameModeBase::ACodexTpsGameModeBase()
{
    // 기본 폰을 TPSCharacter로 설정
    DefaultPawnClass = ATPSCharacter::StaticClass();
}

