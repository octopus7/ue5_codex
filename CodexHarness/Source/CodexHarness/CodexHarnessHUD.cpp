#include "CodexHarnessHUD.h"

#include "CodexHarnessGameMode.h"
#include "CodexHarnessHealthComponent.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"

void ACodexHarnessHUD::DrawHUD()
{
	Super::DrawHUD();

	if (Canvas == nullptr)
	{
		return;
	}

	const UCodexHarnessHealthComponent* const HealthComponent = PlayerOwner != nullptr && PlayerOwner->GetPawn() != nullptr
		? PlayerOwner->GetPawn()->FindComponentByClass<UCodexHarnessHealthComponent>()
		: nullptr;
	const ACodexHarnessGameMode* const CodexHarnessGameMode = GetWorld() != nullptr
		? GetWorld()->GetAuthGameMode<ACodexHarnessGameMode>()
		: nullptr;

	const float CurrentHealth = HealthComponent != nullptr ? HealthComponent->GetCurrentHealth() : 0.0f;
	const float MaxHealth = HealthComponent != nullptr ? HealthComponent->GetMaxHealth() : 0.0f;
	const float HealthFraction = HealthComponent != nullptr ? HealthComponent->GetHealthFraction() : 0.0f;
	const int32 CurrentWave = CodexHarnessGameMode != nullptr ? CodexHarnessGameMode->GetCurrentWave() : 0;
	const int32 RemainingEnemyCount = CodexHarnessGameMode != nullptr ? CodexHarnessGameMode->GetRemainingEnemyCount() : 0;
	const bool bIsGameOver = CodexHarnessGameMode != nullptr ? CodexHarnessGameMode->IsGameOver() : false;

	constexpr float PanelX = 40.0f;
	constexpr float PanelY = 40.0f;
	constexpr float HealthBarWidth = 240.0f;
	constexpr float HealthBarHeight = 18.0f;
	constexpr float TextOffsetY = 30.0f;

	DrawRect(FLinearColor(0.05f, 0.05f, 0.05f, 0.8f), PanelX - 8.0f, PanelY - 8.0f, 280.0f, 110.0f);
	DrawRect(FLinearColor(0.15f, 0.15f, 0.15f, 1.0f), PanelX, PanelY, HealthBarWidth, HealthBarHeight);
	DrawRect(FLinearColor(0.18f, 0.72f, 0.32f, 1.0f), PanelX, PanelY, HealthBarWidth * FMath::Clamp(HealthFraction, 0.0f, 1.0f), HealthBarHeight);

	UFont* const Font = GEngine != nullptr ? GEngine->GetSmallFont() : nullptr;
	DrawText(FString::Printf(TEXT("HP %.0f / %.0f"), CurrentHealth, MaxHealth), FLinearColor::White, PanelX, PanelY + TextOffsetY, Font, 1.0f, false);
	DrawText(FString::Printf(TEXT("Wave %d"), CurrentWave), FLinearColor::White, PanelX, PanelY + TextOffsetY + 24.0f, Font, 1.0f, false);
	DrawText(FString::Printf(TEXT("Remaining Enemies %d"), RemainingEnemyCount), FLinearColor::White, PanelX, PanelY + TextOffsetY + 48.0f, Font, 1.0f, false);

	if (bIsGameOver)
	{
		const float OverlayWidth = 480.0f;
		const float OverlayHeight = 140.0f;
		const float OverlayX = (Canvas->ClipX - OverlayWidth) * 0.5f;
		const float OverlayY = (Canvas->ClipY - OverlayHeight) * 0.5f;
		DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.75f), OverlayX, OverlayY, OverlayWidth, OverlayHeight);

		UFont* const LargeFont = GEngine != nullptr ? GEngine->GetLargeFont() : Font;
		DrawText(TEXT("GAME OVER"), FLinearColor::White, OverlayX + 120.0f, OverlayY + 28.0f, LargeFont, 1.0f, false);
		DrawText(TEXT("Press R to Restart"), FLinearColor::White, OverlayX + 128.0f, OverlayY + 82.0f, Font, 1.0f, false);
	}
}
