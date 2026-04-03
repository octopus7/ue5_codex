#pragma once

#include "GameFramework/HUD.h"

#include "CHGameHUD.generated.h"

class ACHPlayerCharacter;

UCLASS()
class CODEXHARNESS_API ACHGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACHGameHUD();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void DrawHUD() override;

private:
	bool CaptureHudState(float& OutCurrentHealth, float& OutMaxHealth, int32& OutWave, int32& OutAliveEnemies) const;
	const ACHPlayerCharacter* ResolvePlayerCharacter() const;

	bool bHudSmokeEnabled = false;
	bool bHudSmokeLogged = false;
	float HudSmokeElapsedTime = 0.0f;
};
