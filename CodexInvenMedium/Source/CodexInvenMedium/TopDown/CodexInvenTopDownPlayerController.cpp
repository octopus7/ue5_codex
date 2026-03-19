#include "CodexInvenTopDownPlayerController.h"

#include "../CodexInvenMediumGameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "../Input/CodexInvenMediumInputConfigDataAsset.h"

ACodexInvenTopDownPlayerController::ACodexInvenTopDownPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ACodexInvenTopDownPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	ApplyDefaultInputMapping();
}

UInputMappingContext* ACodexInvenTopDownPlayerController::GetDefaultInputMappingContext() const
{
	const UCodexInvenMediumGameInstance* TypedGameInstance = GetGameInstance<UCodexInvenMediumGameInstance>();
	if (TypedGameInstance == nullptr || TypedGameInstance->DefaultInputConfig == nullptr)
	{
		return nullptr;
	}

	return TypedGameInstance->DefaultInputConfig->InputMappingContext;
}

void ACodexInvenTopDownPlayerController::ApplyDefaultInputMapping()
{
	UInputMappingContext* MappingContext = GetDefaultInputMappingContext();
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (MappingContext == nullptr || LocalPlayer == nullptr)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		InputSubsystem->ClearAllMappings();
		InputSubsystem->AddMappingContext(MappingContext, 0);
	}
}
