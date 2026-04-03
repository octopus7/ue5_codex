#include "UI/CHGameHUD.h"

#include "Combat/CHHealthComponent.h"
#include "Engine/Canvas.h"
#include "Game/CHTopDownGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Player/CHPlayerCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogCHGameHUD, Log, All);

ACHGameHUD::ACHGameHUD()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACHGameHUD::BeginPlay()
{
	Super::BeginPlay();

	bHudSmokeEnabled = FParse::Param(FCommandLine::Get(), TEXT("CHHudSmoke"));
	bHudSmokeLogged = false;
	HudSmokeElapsedTime = 0.0f;
}

void ACHGameHUD::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bHudSmokeEnabled || bHudSmokeLogged)
	{
		return;
	}

	HudSmokeElapsedTime += DeltaSeconds;
	if (HudSmokeElapsedTime < 0.25f)
	{
		return;
	}

	float CurrentHealth = 0.0f;
	float MaxHealth = 0.0f;
	int32 CurrentWave = 0;
	int32 AliveEnemies = 0;
	if (!CaptureHudState(CurrentHealth, MaxHealth, CurrentWave, AliveEnemies))
	{
		return;
	}

	UE_LOG(
		LogCHGameHUD,
		Display,
		TEXT("CHHudValues Health=%.2f MaxHealth=%.2f Wave=%d Alive=%d"),
		CurrentHealth,
		MaxHealth,
		CurrentWave,
		AliveEnemies);

	bHudSmokeLogged = true;
	if (PlayerOwner)
	{
		PlayerOwner->ConsoleCommand(TEXT("quit"));
		return;
	}

	if (APlayerController* const PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		PlayerController->ConsoleCommand(TEXT("quit"));
	}
}

void ACHGameHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	float CurrentHealth = 0.0f;
	float MaxHealth = 0.0f;
	int32 CurrentWave = 0;
	int32 AliveEnemies = 0;
	CaptureHudState(CurrentHealth, MaxHealth, CurrentWave, AliveEnemies);

	const FString HealthText = FString::Printf(TEXT("Health: %.0f / %.0f"), CurrentHealth, MaxHealth);
	const FString WaveText = FString::Printf(TEXT("Wave: %d"), CurrentWave);
	const FString EnemyText = FString::Printf(TEXT("Enemies: %d"), AliveEnemies);

	DrawText(HealthText, FLinearColor(0.95f, 0.95f, 0.95f, 1.0f), 40.0f, 40.0f, nullptr, 1.4f, false);
	DrawText(WaveText, FLinearColor(0.88f, 0.77f, 0.20f, 1.0f), 40.0f, 82.0f, nullptr, 1.2f, false);
	DrawText(EnemyText, FLinearColor(0.95f, 0.45f, 0.35f, 1.0f), 40.0f, 118.0f, nullptr, 1.2f, false);

	const ACHTopDownGameMode* const GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ACHTopDownGameMode>() : nullptr;
	if (GameMode && GameMode->IsGameOver())
	{
		DrawText(TEXT("GAME OVER"), FLinearColor(0.98f, 0.22f, 0.22f, 1.0f), 40.0f, 180.0f, nullptr, 2.0f, false);
		DrawText(TEXT("Press R to Restart"), FLinearColor(0.95f, 0.95f, 0.95f, 1.0f), 40.0f, 220.0f, nullptr, 1.1f, false);
	}
}

bool ACHGameHUD::CaptureHudState(float& OutCurrentHealth, float& OutMaxHealth, int32& OutWave, int32& OutAliveEnemies) const
{
	OutCurrentHealth = 0.0f;
	OutMaxHealth = 0.0f;
	OutWave = 0;
	OutAliveEnemies = 0;

	const ACHPlayerCharacter* const PlayerCharacter = ResolvePlayerCharacter();
	const ACHTopDownGameMode* const GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ACHTopDownGameMode>() : nullptr;
	if (!PlayerCharacter || !GameMode)
	{
		return false;
	}

	if (const UCHHealthComponent* const HealthComponent = PlayerCharacter->GetHealthComponent())
	{
		OutCurrentHealth = HealthComponent->GetCurrentHealth();
		OutMaxHealth = HealthComponent->GetMaxHealth();
	}

	OutWave = GameMode->GetCurrentWave();
	OutAliveEnemies = GameMode->GetAliveEnemyCount();
	return true;
}

const ACHPlayerCharacter* ACHGameHUD::ResolvePlayerCharacter() const
{
	if (const APawn* const OwningPawn = GetOwningPawn())
	{
		return Cast<ACHPlayerCharacter>(OwningPawn);
	}

	return Cast<ACHPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}
