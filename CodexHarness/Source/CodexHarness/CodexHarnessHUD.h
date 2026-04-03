#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CodexHarnessHUD.generated.h"

UCLASS()
class CODEXHARNESS_API ACodexHarnessHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};
