// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenTopDownPlayerController.h"

#include "CodexInvenAttendanceConfigDataAsset.h"
#include "CodexInvenAttendanceSubsystem.h"
#include "CodexInvenAttendanceWidgetBase.h"
#include "CodexInvenClockMvvmWidget.h"
#include "CodexInvenClockWidget.h"
#include "CodexInvenGameInstance.h"
#include "CodexInvenInputConfigDataAsset.h"
#include "CodexInvenPlayerHudWidget.h"
#include "CodexInvenProjectile.h"
#include "CodexInvenTopDownCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/EngineTypes.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

namespace
{
	constexpr float CursorTraceDistance = 100000.0f;
	constexpr int32 MaxCursorProjectileIgnores = 16;
	const TCHAR* ClockWidgetClassObjectPath = TEXT("/Game/UI/WBP_CodexClock.WBP_CodexClock_C");
	const TCHAR* ClockMvvmWidgetClassObjectPath = TEXT("/Game/UI/WBP_CodexClockMvvm.WBP_CodexClockMvvm_C");
}

ACodexInvenTopDownPlayerController::ACodexInvenTopDownPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	PlayerHudWidgetClass = UCodexInvenPlayerHudWidget::StaticClass();
}

void ACodexInvenTopDownPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	ApplyInputMappingContext();
	TryCreatePlayerHud();
	TryCreateClockWidget();
	TryCreateClockMvvmWidget();
	RefreshObservedOwnershipComponent();
	UpdateAimFromCursor();
	ScheduleAttendancePopupSequence();
}

void ACodexInvenTopDownPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (CursorAimResumeDelay > 0.0f && LastExplicitLookInputTime >= 0.0f && World->GetTimeSeconds() < LastExplicitLookInputTime + CursorAimResumeDelay)
	{
		return;
	}

	UpdateAimFromCursor();
}

void ACodexInvenTopDownPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	RefreshObservedOwnershipComponent();
	if (RuntimeAttendanceWidget == nullptr)
	{
		ScheduleAttendancePopupSequence();
	}
}

void ACodexInvenTopDownPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		BindConfiguredInput(*EnhancedInputComponent);
	}
}

const UCodexInvenInputConfigDataAsset* ACodexInvenTopDownPlayerController::GetInputConfig() const
{
	const UCodexInvenGameInstance* CodexInvenGameInstance = Cast<UCodexInvenGameInstance>(GetGameInstance());
	return CodexInvenGameInstance != nullptr ? CodexInvenGameInstance->GetInputConfig() : nullptr;
}

ACodexInvenTopDownCharacter* ACodexInvenTopDownPlayerController::GetTopDownCharacter() const
{
	return Cast<ACodexInvenTopDownCharacter>(GetPawn());
}

bool ACodexInvenTopDownPlayerController::ShouldUseCursorAim() const
{
	return bShowMouseCursor;
}

void ACodexInvenTopDownPlayerController::ApplyInputMappingContext()
{
	const UCodexInvenInputConfigDataAsset* InputConfig = GetInputConfig();
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (InputConfig == nullptr || InputConfig->InputMappingContext == nullptr || LocalPlayer == nullptr)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		InputSubsystem->AddMappingContext(InputConfig->InputMappingContext, InputMappingPriority);
	}
}

void ACodexInvenTopDownPlayerController::TryCreatePlayerHud()
{
	if (!IsLocalController() || RuntimePlayerHudWidget != nullptr || PlayerHudWidgetClass == nullptr)
	{
		return;
	}

	RuntimePlayerHudWidget = CreateWidget<UCodexInvenPlayerHudWidget>(this, PlayerHudWidgetClass);
	if (RuntimePlayerHudWidget != nullptr)
	{
		RuntimePlayerHudWidget->AddToViewport(100);
	}
}

void ACodexInvenTopDownPlayerController::TryCreateClockWidget()
{
	if (!IsLocalController() || RuntimeClockWidget != nullptr)
	{
		return;
	}

	TSubclassOf<UCodexInvenClockWidget> ResolvedClockWidgetClass = ClockWidgetClass;
	if (ResolvedClockWidgetClass == nullptr)
	{
		ResolvedClockWidgetClass = LoadClass<UCodexInvenClockWidget>(nullptr, ClockWidgetClassObjectPath);
	}

	if (ResolvedClockWidgetClass == nullptr)
	{
		return;
	}

	RuntimeClockWidget = CreateWidget<UCodexInvenClockWidget>(this, ResolvedClockWidgetClass);
	if (RuntimeClockWidget != nullptr)
	{
		RuntimeClockWidget->AddToViewport(200);
	}
}

void ACodexInvenTopDownPlayerController::TryCreateClockMvvmWidget()
{
	if (!IsLocalController() || RuntimeClockMvvmWidget != nullptr)
	{
		return;
	}

	TSubclassOf<UCodexInvenClockMvvmWidget> ResolvedClockMvvmWidgetClass = ClockMvvmWidgetClass;
	if (ResolvedClockMvvmWidgetClass == nullptr)
	{
		ResolvedClockMvvmWidgetClass = LoadClass<UCodexInvenClockMvvmWidget>(nullptr, ClockMvvmWidgetClassObjectPath);
	}

	if (ResolvedClockMvvmWidgetClass == nullptr)
	{
		return;
	}

	RuntimeClockMvvmWidget = CreateWidget<UCodexInvenClockMvvmWidget>(this, ResolvedClockMvvmWidgetClass);
	if (RuntimeClockMvvmWidget != nullptr)
	{
		RuntimeClockMvvmWidget->AddToViewport(210);
	}
}

void ACodexInvenTopDownPlayerController::ScheduleAttendancePopupSequence()
{
	if (!IsLocalController() || RuntimeAttendanceWidget != nullptr)
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	float PopupDelaySeconds = 0.0f;
	if (const UCodexInvenGameInstance* const CodexInvenGameInstance = Cast<UCodexInvenGameInstance>(GetGameInstance()))
	{
		if (const UCodexInvenAttendanceConfigDataAsset* const AttendanceConfig = CodexInvenGameInstance->GetAttendanceConfig())
		{
			PopupDelaySeconds = FMath::Max(0.0f, AttendanceConfig->PopupInitialDelaySeconds);
		}
	}

	World->GetTimerManager().ClearTimer(AttendancePopupSequenceTimerHandle);
	World->GetTimerManager().SetTimer(
		AttendancePopupSequenceTimerHandle,
		this,
		&ThisClass::HandleAttendancePopupSequenceStart,
		PopupDelaySeconds > 0.0f ? PopupDelaySeconds : 0.01f,
		false);
}

void ACodexInvenTopDownPlayerController::HandleAttendancePopupSequenceStart()
{
	if (!IsLocalController() || RuntimeAttendanceWidget != nullptr)
	{
		return;
	}

	ACodexInvenTopDownCharacter* const TopDownCharacter = GetTopDownCharacter();
	if (TopDownCharacter == nullptr || TopDownCharacter->GetOwnershipComponent() == nullptr)
	{
		if (UWorld* const World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				AttendancePopupSequenceTimerHandle,
				this,
				&ThisClass::HandleAttendancePopupSequenceStart,
				0.25f,
				false);
		}

		return;
	}

	UCodexInvenAttendanceSubsystem* const AttendanceSubsystem = GetGameInstance() != nullptr
		? GetGameInstance()->GetSubsystem<UCodexInvenAttendanceSubsystem>()
		: nullptr;
	if (AttendanceSubsystem == nullptr)
	{
		return;
	}

	PendingAttendancePopupEntries = AttendanceSubsystem->GetPendingPopupQueueForToday();
	if (PendingAttendancePopupEntries.IsEmpty())
	{
		return;
	}

	TryShowNextAttendancePopup();
}

void ACodexInvenTopDownPlayerController::TryShowNextAttendancePopup()
{
	if (RuntimeAttendanceWidget != nullptr)
	{
		return;
	}

	while (!PendingAttendancePopupEntries.IsEmpty())
	{
		const FCodexInvenAttendancePopupEntry PopupEntry = PendingAttendancePopupEntries[0];
		PendingAttendancePopupEntries.RemoveAt(0);

		if (PopupEntry.PopupWidgetClass == nullptr)
		{
			continue;
		}

		RuntimeAttendanceWidget = CreateWidget<UCodexInvenAttendanceWidgetBase>(this, PopupEntry.PopupWidgetClass);
		if (RuntimeAttendanceWidget == nullptr)
		{
			continue;
		}

		RuntimeAttendanceWidget->InitializeForEvent(PopupEntry.EventId);
		RuntimeAttendanceWidget->OnAttendanceWidgetClosed().AddUObject(this, &ThisClass::HandleAttendancePopupClosed);
		RuntimeAttendanceWidget->AddToViewport(400);
		bIsAttendancePopupVisible = true;
		return;
	}

	bIsAttendancePopupVisible = false;
}

void ACodexInvenTopDownPlayerController::HandleAttendancePopupClosed(const FName InClosedEventId)
{
	static_cast<void>(InClosedEventId);

	if (RuntimeAttendanceWidget != nullptr)
	{
		RuntimeAttendanceWidget->OnAttendanceWidgetClosed().RemoveAll(this);
		RuntimeAttendanceWidget = nullptr;
	}

	bIsAttendancePopupVisible = false;
	TryShowNextAttendancePopup();
}

void ACodexInvenTopDownPlayerController::RefreshObservedOwnershipComponent()
{
	if (RuntimePlayerHudWidget == nullptr)
	{
		return;
	}

	if (const ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter())
	{
		RuntimePlayerHudWidget->SetObservedOwnershipComponent(ControlledCharacter->GetOwnershipComponent());
		return;
	}

	RuntimePlayerHudWidget->SetObservedOwnershipComponent(nullptr);
}

bool ACodexInvenTopDownPlayerController::TryGetCursorGroundPoint(FVector& OutWorldPoint) const
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return false;
	}

	FVector MouseWorldLocation;
	FVector MouseWorldDirection;
	if (!DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection))
	{
		if (bLogCursorTrace)
		{
			UE_LOG(LogTemp, Log, TEXT("[Cursor] Failed to deproject mouse position."));
		}

		return false;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(CodexInvenCursorTrace), true);
	QueryParams.AddIgnoredActor(this);

	if (const APawn* ControlledPawn = GetPawn())
	{
		QueryParams.AddIgnoredActor(ControlledPawn);
	}

	const FVector TraceStart = MouseWorldLocation;
	const FVector TraceEnd = MouseWorldLocation + (MouseWorldDirection * CursorTraceDistance);

	for (int32 IgnoreIndex = 0; IgnoreIndex < MaxCursorProjectileIgnores; ++IgnoreIndex)
	{
		FHitResult HitResult;
		if (!World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			break;
		}

		AActor* HitActor = HitResult.GetActor();
		if (HitActor != nullptr && HitActor->IsA<ACodexInvenProjectile>())
		{
			QueryParams.AddIgnoredActor(HitActor);

			if (bLogCursorTrace)
			{
				UE_LOG(
					LogTemp,
					Log,
					TEXT("[Cursor] Ignored projectile hit %s at %s."),
					*GetNameSafe(HitActor),
					*HitResult.ImpactPoint.ToString());
			}

			continue;
		}

		OutWorldPoint = HitResult.ImpactPoint;
		return true;
	}

	const APawn* ControlledPawn = GetPawn();
	const float GroundHeight = ControlledPawn != nullptr ? ControlledPawn->GetActorLocation().Z : 0.0f;
	const FPlane GroundPlane(FVector(0.0f, 0.0f, GroundHeight), FVector::UpVector);
	OutWorldPoint = FMath::LinePlaneIntersection(TraceStart, TraceEnd, GroundPlane);

	if (bLogCursorTrace)
	{
		UE_LOG(LogTemp, Log, TEXT("[Cursor] Fell back to ground plane at %s."), *OutWorldPoint.ToString());
	}

	return true;
}

void ACodexInvenTopDownPlayerController::FireProjectileOnce()
{
	if (ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter())
	{
		FVector CursorWorldPoint;
		if (TryGetCursorGroundPoint(CursorWorldPoint))
		{
			ControlledCharacter->FireAtWorldLocation(CursorWorldPoint);
			return;
		}

		ControlledCharacter->FireAtWorldLocation(ControlledCharacter->GetActorLocation() + ControlledCharacter->GetActorForwardVector() * 1000.0f);
	}
}

bool ACodexInvenTopDownPlayerController::ShouldBlockFireInput() const
{
	return bIsAttendancePopupVisible || (RuntimePlayerHudWidget != nullptr && RuntimePlayerHudWidget->ShouldBlockFireInput());
}

void ACodexInvenTopDownPlayerController::UpdateAimFromCursor() const
{
	ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter();
	if (ControlledCharacter == nullptr)
	{
		return;
	}

	FVector CursorWorldPoint;
	if (TryGetCursorGroundPoint(CursorWorldPoint))
	{
		ControlledCharacter->AimAtWorldLocation(CursorWorldPoint);
	}
}

void ACodexInvenTopDownPlayerController::HandleMove(const FInputActionValue& InValue)
{
	if (ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter())
	{
		ControlledCharacter->MoveInTopDownPlane(InValue.Get<FVector2D>());
		UpdateAimFromCursor();
	}
}

void ACodexInvenTopDownPlayerController::HandleLook(const FInputActionValue& InValue)
{
	const FVector2D LookInput = InValue.Get<FVector2D>();
	if (LookInput.IsNearlyZero())
	{
		UpdateAimFromCursor();
		return;
	}

	if (ShouldUseCursorAim())
	{
		if (bLogCursorTrace)
		{
			UE_LOG(LogTemp, Log, TEXT("[Cursor] Ignored raw look input while cursor aim is active: %s"), *LookInput.ToString());
		}

		UpdateAimFromCursor();
		return;
	}

	ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter();
	if (ControlledCharacter == nullptr)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		LastExplicitLookInputTime = World->GetTimeSeconds();
	}

	const FRotator CameraRotation = PlayerCameraManager != nullptr ? PlayerCameraManager->GetCameraRotation() : FRotator::ZeroRotator;
	FVector CameraForward = CameraRotation.Vector();
	CameraForward.Z = 0.0f;
	CameraForward.Normalize();

	FVector CameraRight = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	CameraRight.Z = 0.0f;
	CameraRight.Normalize();

	FVector AimDirection = (CameraForward * LookInput.Y) + (CameraRight * LookInput.X);
	AimDirection.Z = 0.0f;
	AimDirection.Normalize();

	if (AimDirection.IsNearlyZero())
	{
		return;
	}

	const FVector AimTarget = ControlledCharacter->GetActorLocation() + (AimDirection * 1000.0f);
	ControlledCharacter->AimAtWorldLocation(AimTarget);
}

void ACodexInvenTopDownPlayerController::HandleJumpStarted()
{
	if (ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter())
	{
		ControlledCharacter->BeginGenericJump();
	}
}

void ACodexInvenTopDownPlayerController::HandleJumpCompleted()
{
	if (ACodexInvenTopDownCharacter* ControlledCharacter = GetTopDownCharacter())
	{
		ControlledCharacter->EndGenericJump();
	}
}

void ACodexInvenTopDownPlayerController::HandleFireStarted()
{
	if (ShouldBlockFireInput())
	{
		return;
	}

	if (bIsAutomaticFireActive)
	{
		return;
	}

	bIsAutomaticFireActive = true;
	FireProjectileOnce();

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	World->GetTimerManager().SetTimer(
		AutomaticFireTimerHandle,
		this,
		&ThisClass::HandleAutoFireTick,
		AutomaticFireInterval,
		true,
		AutomaticFireInterval);
}

void ACodexInvenTopDownPlayerController::HandleFireCompleted()
{
	bIsAutomaticFireActive = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(AutomaticFireTimerHandle);
	}
}

void ACodexInvenTopDownPlayerController::HandleAutoFireTick()
{
	if (!bIsAutomaticFireActive)
	{
		HandleFireCompleted();
		return;
	}

	if (ShouldBlockFireInput())
	{
		HandleFireCompleted();
		return;
	}

	FireProjectileOnce();
}

void ACodexInvenTopDownPlayerController::BindConfiguredInput(UEnhancedInputComponent& InEnhancedInputComponent)
{
	const UCodexInvenInputConfigDataAsset* InputConfig = GetInputConfig();
	if (InputConfig == nullptr || !InputConfig->HasRequiredBindings())
	{
		return;
	}

	InEnhancedInputComponent.BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMove);
	InEnhancedInputComponent.BindAction(InputConfig->LookAction, ETriggerEvent::Triggered, this, &ThisClass::HandleLook);
	InEnhancedInputComponent.BindAction(InputConfig->JumpAction, ETriggerEvent::Started, this, &ThisClass::HandleJumpStarted);
	InEnhancedInputComponent.BindAction(InputConfig->JumpAction, ETriggerEvent::Completed, this, &ThisClass::HandleJumpCompleted);
	InEnhancedInputComponent.BindAction(InputConfig->FireAction, ETriggerEvent::Started, this, &ThisClass::HandleFireStarted);
	InEnhancedInputComponent.BindAction(InputConfig->FireAction, ETriggerEvent::Completed, this, &ThisClass::HandleFireCompleted);
	InEnhancedInputComponent.BindAction(InputConfig->FireAction, ETriggerEvent::Canceled, this, &ThisClass::HandleFireCompleted);
}
