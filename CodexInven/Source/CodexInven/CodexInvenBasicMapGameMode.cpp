// Fill out your copyright notice in the Description page of Project Settings.


#include "CodexInvenBasicMapGameMode.h"

#include "Blueprint/UserWidget.h"
#include "CodexInvenOwnershipDebugWidget.h"
#include "CodexInvenPickupSpawner.h"
#include "CodexInvenTopDownCharacter.h"
#include "CodexInvenTopDownPlayerController.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	constexpr float OwnershipWidgetRetryIntervalSeconds = 0.1f;
}

ACodexInvenBasicMapGameMode::ACodexInvenBasicMapGameMode()
{
	DefaultPawnClass = ACodexInvenTopDownCharacter::StaticClass();
	PlayerControllerClass = ACodexInvenTopDownPlayerController::StaticClass();
	PickupSpawnerClass = ACodexInvenPickupSpawner::StaticClass();
	OwnershipDebugWidgetClass = UCodexInvenOwnershipDebugWidget::StaticClass();
}

void ACodexInvenBasicMapGameMode::StartPlay()
{
	Super::StartPlay();

	TrySpawnPickupSpawner();
	TryInitializeOwnershipDebugWidget();
}

void ACodexInvenBasicMapGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WidgetInitRetryTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void ACodexInvenBasicMapGameMode::TrySpawnPickupSpawner()
{
	if (!HasAuthority() || PickupSpawnerClass == nullptr)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<ACodexInvenPickupSpawner> It(World); It; ++It)
		{
			if (PickupSpawnerClass == nullptr || It->IsA(PickupSpawnerClass))
			{
				RuntimePickupSpawner = *It;
				return;
			}
		}

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = this;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		RuntimePickupSpawner = World->SpawnActor<ACodexInvenPickupSpawner>(PickupSpawnerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
	}
}

void ACodexInvenBasicMapGameMode::TryInitializeOwnershipDebugWidget()
{
	UWorld* World = GetWorld();
	if (World == nullptr || OwnershipDebugWidgetClass == nullptr)
	{
		return;
	}

	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (LocalPlayerController == nullptr || !LocalPlayerController->IsLocalController())
	{
		World->GetTimerManager().SetTimer(
			WidgetInitRetryTimerHandle,
			this,
			&ThisClass::HandleOwnershipWidgetInitRetry,
			OwnershipWidgetRetryIntervalSeconds,
			true);
		return;
	}

	if (RuntimeOwnershipDebugWidget == nullptr)
	{
		RuntimeOwnershipDebugWidget = CreateWidget<UCodexInvenOwnershipDebugWidget>(LocalPlayerController, OwnershipDebugWidgetClass);
		if (RuntimeOwnershipDebugWidget != nullptr)
		{
			RuntimeOwnershipDebugWidget->AddToViewport(100);
		}
	}

	ACodexInvenTopDownCharacter* ControlledCharacter = Cast<ACodexInvenTopDownCharacter>(LocalPlayerController->GetPawn());
	if (RuntimeOwnershipDebugWidget == nullptr || ControlledCharacter == nullptr || ControlledCharacter->GetOwnershipComponent() == nullptr)
	{
		World->GetTimerManager().SetTimer(
			WidgetInitRetryTimerHandle,
			this,
			&ThisClass::HandleOwnershipWidgetInitRetry,
			OwnershipWidgetRetryIntervalSeconds,
			true);
		return;
	}

	RuntimeOwnershipDebugWidget->SetObservedOwnershipComponent(ControlledCharacter->GetOwnershipComponent());
	World->GetTimerManager().ClearTimer(WidgetInitRetryTimerHandle);
}

void ACodexInvenBasicMapGameMode::HandleOwnershipWidgetInitRetry()
{
	TryInitializeOwnershipDebugWidget();
}

