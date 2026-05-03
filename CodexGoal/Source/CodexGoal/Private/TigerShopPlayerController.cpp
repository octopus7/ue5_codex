// Copyright Epic Games, Inc. All Rights Reserved.

#include "TigerShopPlayerController.h"

#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "RiceCakeWorkstation.h"
#include "TemporaryTigerCharacter.h"
#include "TigerClickerWidget.h"
#include "TigerCurrencyComponent.h"
#include "TigerShopSaveGame.h"

namespace
{
	const FString TigerShopSaveSlot = TEXT("TigerShopPrototype");
	constexpr int32 TigerShopSaveUserIndex = 0;
}

ATigerShopPlayerController::ATigerShopPlayerController()
{
	CurrencyComponent = CreateDefaultSubobject<UTigerCurrencyComponent>(TEXT("CurrencyComponent"));
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ATigerShopPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	LoadProgress();
	if (CurrencyComponent)
	{
		CurrencyComponent->OnCurrencyChanged.AddUniqueDynamic(this, &ATigerShopPlayerController::HandleCurrencyChangedForSave);
	}

	BuildClickerWidget();
}

void ATigerShopPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &ATigerShopPlayerController::HandlePrimaryClick);
	}
}

bool ATigerShopPlayerController::TryBuyClickUpgrade()
{
	return CurrencyComponent ? CurrencyComponent->TryBuyClickUpgrade() : false;
}

void ATigerShopPlayerController::HandleWorkstationClicked(ARiceCakeWorkstation* Workstation)
{
	if (!CurrencyComponent || !Workstation)
	{
		return;
	}

	CurrencyComponent->AddRiceCakes(CurrencyComponent->GetClickPower());
	Workstation->PlayClickFeedback();

	if (TemporaryTiger)
	{
		TemporaryTiger->PlayProductionPulse();
	}
}

void ATigerShopPlayerController::RegisterTemporaryTiger(ATemporaryTigerCharacter* InTemporaryTiger)
{
	TemporaryTiger = InTemporaryTiger;
}

void ATigerShopPlayerController::HandleCurrencyChangedForSave()
{
	SaveProgress();
}

void ATigerShopPlayerController::HandlePrimaryClick()
{
	FHitResult HitResult;
	if (!GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		return;
	}

	if (ARiceCakeWorkstation* Workstation = Cast<ARiceCakeWorkstation>(HitResult.GetActor()))
	{
		HandleWorkstationClicked(Workstation);
	}
}

void ATigerShopPlayerController::LoadProgress()
{
	if (!CurrencyComponent || !UGameplayStatics::DoesSaveGameExist(TigerShopSaveSlot, TigerShopSaveUserIndex))
	{
		return;
	}

	UTigerShopSaveGame* SaveGame = Cast<UTigerShopSaveGame>(UGameplayStatics::LoadGameFromSlot(TigerShopSaveSlot, TigerShopSaveUserIndex));
	if (!SaveGame)
	{
		return;
	}

	CurrencyComponent->SetState(SaveGame->RiceCakeCount, SaveGame->ClickPower, SaveGame->ClickUpgradeLevel);
}

void ATigerShopPlayerController::SaveProgress() const
{
	if (!CurrencyComponent)
	{
		return;
	}

	UTigerShopSaveGame* SaveGame = Cast<UTigerShopSaveGame>(UGameplayStatics::CreateSaveGameObject(UTigerShopSaveGame::StaticClass()));
	if (!SaveGame)
	{
		return;
	}

	SaveGame->RiceCakeCount = CurrencyComponent->GetRiceCakeCount();
	SaveGame->ClickPower = CurrencyComponent->GetClickPower();
	SaveGame->ClickUpgradeLevel = CurrencyComponent->GetClickUpgradeLevel();

	UGameplayStatics::SaveGameToSlot(SaveGame, TigerShopSaveSlot, TigerShopSaveUserIndex);
}

void ATigerShopPlayerController::BuildClickerWidget()
{
	if (ClickerWidget)
	{
		return;
	}

	ClickerWidget = CreateWidget<UTigerClickerWidget>(this, UTigerClickerWidget::StaticClass());
	if (ClickerWidget)
	{
		ClickerWidget->AddToViewport(100);
		ClickerWidget->InitializeForPlayer(this);
	}
}
