#include "Player/CHPlayerController.h"

#include "Combat/CHHealthComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Game/CHTopDownGameMode.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "Player/CHPlayerCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogCHPlayerController, Log, All);

namespace CHPlayerController
{
	static int32 GRestartSmokeRunCount = 0;

	static UInputModifierNegate* CreateNegateModifier(UObject* Outer, const bool bNegateX, const bool bNegateY)
	{
		UInputModifierNegate* const NegateModifier = NewObject<UInputModifierNegate>(Outer);
		NegateModifier->bX = bNegateX;
		NegateModifier->bY = bNegateY;
		NegateModifier->bZ = false;
		return NegateModifier;
	}

	static UInputModifierSwizzleAxis* CreateSwizzleModifier(UObject* Outer)
	{
		UInputModifierSwizzleAxis* const SwizzleModifier = NewObject<UInputModifierSwizzleAxis>(Outer);
		SwizzleModifier->Order = EInputAxisSwizzle::YXZ;
		return SwizzleModifier;
	}
}

ACHPlayerController::ACHPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ACHPlayerController::BeginPlay()
{
	Super::BeginPlay();

	EnsureInputDefinitions();
	ApplyGameplayMappingContext();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	bAutoMoveSmokeEnabled = FParse::Param(FCommandLine::Get(), TEXT("CHAutoMoveSmoke"));
	bAutoMoveSmokeInitialized = false;
	bAutoMoveSmokeFinished = false;
	bAimSmokeEnabled = FParse::Param(FCommandLine::Get(), TEXT("CHAimSmoke"));
	bAimSmokeLogged = false;
	bDamageSmokeEnabled = FParse::Param(FCommandLine::Get(), TEXT("CHDamageSmoke"));
	bFireSmokeEnabled = FParse::Param(FCommandLine::Get(), TEXT("CHFireSmoke"));
	bFireSmokeTriggered = false;
	bHealthObservationBound = false;
	bHasAimWorldPoint = false;
	bRestartSmokeEnabled = FParse::Param(FCommandLine::Get(), TEXT("CHRestartSmoke"));
	bRestartSmokeRespawnLogged = false;
	bRestartSmokeRestartIssued = false;
	AutoMoveSmokeElapsedTime = 0.0f;
	AimSmokeElapsedTime = 0.0f;
	DamageSmokeElapsedTime = 0.0f;
	FireSmokeElapsedTime = 0.0f;
	RestartSmokeElapsedTime = 0.0f;
	DamageSmokeStep = 0;
	ObservedDeathCount = 0;
	RestartSmokeStep = 0;
	AutoMoveSmokeStartLocation = FVector::ZeroVector;
	CurrentAimWorldPoint = FVector::ZeroVector;
	EnsureHealthObservation();
}

void ACHPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	EnsureHealthObservation();
	CachedMoveDeltaSeconds = DeltaTime;
	UpdateAimTarget(DeltaTime);
	UpdateAutoDamageSmoke(DeltaTime);
	UpdateAutoFireSmoke(DeltaTime);
	UpdateAutoRestartSmoke(DeltaTime);
	UpdateAutoMoveSmoke(DeltaTime);
}

void ACHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	EnsureInputDefinitions();

	UEnhancedInputComponent* const EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogCHPlayerController, Error, TEXT("EnhancedInputComponent를 찾지 못했습니다."));
		return;
	}

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACHPlayerController::HandleMoveAction);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACHPlayerController::HandleMoveAction);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ACHPlayerController::HandleFireAction);
	EnhancedInputComponent->BindAction(RestartAction, ETriggerEvent::Started, this, &ACHPlayerController::HandleRestartAction);
}

void ACHPlayerController::EnsureInputDefinitions()
{
	if (!MoveAction)
	{
		MoveAction = NewObject<UInputAction>(this, TEXT("RuntimeMoveAction"));
		MoveAction->ValueType = EInputActionValueType::Axis2D;
		MoveAction->AccumulationBehavior = EInputActionAccumulationBehavior::Cumulative;
	}

	if (!FireAction)
	{
		FireAction = NewObject<UInputAction>(this, TEXT("RuntimeFireAction"));
		FireAction->ValueType = EInputActionValueType::Boolean;
	}

	if (!RestartAction)
	{
		RestartAction = NewObject<UInputAction>(this, TEXT("RuntimeRestartAction"));
		RestartAction->ValueType = EInputActionValueType::Boolean;
	}

	if (GameplayMappingContext)
	{
		return;
	}

	GameplayMappingContext = NewObject<UInputMappingContext>(this, TEXT("RuntimeGameplayMappingContext"));
	GameplayMappingContext->UnmapAll();

	GameplayMappingContext->MapKey(MoveAction, EKeys::D);

	FEnhancedActionKeyMapping& MoveLeft = GameplayMappingContext->MapKey(MoveAction, EKeys::A);
	MoveLeft.Modifiers.Add(CHPlayerController::CreateNegateModifier(GameplayMappingContext, true, false));

	FEnhancedActionKeyMapping& MoveUp = GameplayMappingContext->MapKey(MoveAction, EKeys::W);
	MoveUp.Modifiers.Add(CHPlayerController::CreateSwizzleModifier(GameplayMappingContext));

	FEnhancedActionKeyMapping& MoveDown = GameplayMappingContext->MapKey(MoveAction, EKeys::S);
	MoveDown.Modifiers.Add(CHPlayerController::CreateSwizzleModifier(GameplayMappingContext));
	MoveDown.Modifiers.Add(CHPlayerController::CreateNegateModifier(GameplayMappingContext, false, true));

	GameplayMappingContext->MapKey(FireAction, EKeys::LeftMouseButton);
	GameplayMappingContext->MapKey(RestartAction, EKeys::R);
}

void ACHPlayerController::ApplyGameplayMappingContext()
{
	if (!IsLocalController())
	{
		return;
	}

	if (ULocalPlayer* const LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* const InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->RemoveMappingContext(GameplayMappingContext);
			InputSubsystem->AddMappingContext(GameplayMappingContext, 0);
			UE_LOG(LogCHPlayerController, Display, TEXT("EnhancedInput 이동 매핑 컨텍스트를 적용했습니다."));
		}
	}
}

void ACHPlayerController::HandleMoveAction(const FInputActionValue& InputActionValue)
{
	if (ACHPlayerCharacter* const PlayerCharacter = GetPawn<ACHPlayerCharacter>())
	{
		PlayerCharacter->Move(InputActionValue.Get<FVector2D>(), CachedMoveDeltaSeconds);
	}
}

void ACHPlayerController::HandleFireAction()
{
	ExecuteFireRequest(false);
}

bool ACHPlayerController::ExecuteFireRequest(const bool bFromSmoke)
{
	ACHPlayerCharacter* const PlayerCharacter = GetPawn<ACHPlayerCharacter>();
	if (!PlayerCharacter)
	{
		return false;
	}

	FCHFireResult FireResult;
	if (!PlayerCharacter->TryFire(FireResult))
	{
		return false;
	}

	const FString HitActorName = FireResult.HitActorName.IsNone() ? TEXT("None") : FireResult.HitActorName.ToString();
	const TCHAR* const LogPrefix = bFromSmoke ? TEXT("CHFireSmokeResult") : TEXT("CHFireShot");
	UE_LOG(
		LogCHPlayerController,
		Display,
		TEXT("%s Hit=%d Aim=%s End=%s Actor=%s"),
		LogPrefix,
		FireResult.bHit ? 1 : 0,
		*FireResult.AimWorldPoint.ToCompactString(),
		*FireResult.TraceEnd.ToCompactString(),
		*HitActorName);

	return true;
}

bool ACHPlayerController::ExecuteRestartRequest(const bool bFromSmoke)
{
	const ACHTopDownGameMode* const GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ACHTopDownGameMode>() : nullptr;
	if (!GameMode || !GameMode->IsGameOver())
	{
		return false;
	}

	UWorld* const World = GetWorld();
	if (!World)
	{
		return false;
	}

	UE_LOG(LogCHPlayerController, Display, TEXT("%s"), bFromSmoke ? TEXT("CHRestartSmokeRequest") : TEXT("CHRestartRequested"));
	UGameplayStatics::OpenLevel(this, FName(*World->GetName()));
	return true;
}

void ACHPlayerController::EnsureHealthObservation()
{
	if (bHealthObservationBound)
	{
		return;
	}

	ACHPlayerCharacter* const PlayerCharacter = GetPawn<ACHPlayerCharacter>();
	if (!PlayerCharacter)
	{
		return;
	}

	UCHHealthComponent* const HealthComponent = PlayerCharacter->GetHealthComponent();
	if (!HealthComponent)
	{
		return;
	}

	HealthComponent->OnDeath.AddUObject(this, &ACHPlayerController::HandleObservedDeath);
	bHealthObservationBound = true;
}

void ACHPlayerController::HandleObservedDeath(UCHHealthComponent* InHealthComponent)
{
	if (!InHealthComponent)
	{
		return;
	}

	++ObservedDeathCount;
	UE_LOG(LogCHPlayerController, Display, TEXT("CHObservedDeathCount=%d"), ObservedDeathCount);

	if (ACHTopDownGameMode* const GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ACHTopDownGameMode>() : nullptr)
	{
		GameMode->NotifyPlayerDied();
	}
}

bool ACHPlayerController::ResolveAimWorldPoint(FVector& OutAimWorldPoint) const
{
	const APawn* const PlayerPawn = GetPawn();
	if (!PlayerPawn)
	{
		return false;
	}

	const FVector PawnLocation = PlayerPawn->GetActorLocation();
	if (bAimSmokeEnabled)
	{
		OutAimWorldPoint = PawnLocation + FVector(400.0f, 400.0f, 0.0f);
		OutAimWorldPoint.Z = PawnLocation.Z;
		return true;
	}

	FVector RayOrigin = FVector::ZeroVector;
	FVector RayDirection = FVector::ZeroVector;
	if (!DeprojectMousePositionToWorld(RayOrigin, RayDirection))
	{
		return false;
	}

	if (FMath::IsNearlyZero(RayDirection.Z))
	{
		return false;
	}

	const float RayDistance = (PawnLocation.Z - RayOrigin.Z) / RayDirection.Z;
	if (RayDistance < 0.0f)
	{
		return false;
	}

	OutAimWorldPoint = RayOrigin + (RayDirection * RayDistance);
	return true;
}

void ACHPlayerController::UpdateAimTarget(float DeltaTime)
{
	ACHPlayerCharacter* const PlayerCharacter = GetPawn<ACHPlayerCharacter>();
	if (!PlayerCharacter)
	{
		bHasAimWorldPoint = false;
		return;
	}

	FVector AimWorldPoint = FVector::ZeroVector;
	if (!ResolveAimWorldPoint(AimWorldPoint))
	{
		bHasAimWorldPoint = false;
		return;
	}

	CurrentAimWorldPoint = AimWorldPoint;
	bHasAimWorldPoint = true;
	PlayerCharacter->AimAt(CurrentAimWorldPoint);

	if (!bAimSmokeEnabled || bAimSmokeLogged)
	{
		return;
	}

	AimSmokeElapsedTime += DeltaTime;
	if (AimSmokeElapsedTime < 0.20f)
	{
		return;
	}

	UE_LOG(LogCHPlayerController, Display, TEXT("CHAimSmokeYaw=%.2f"), PlayerCharacter->GetActorRotation().Yaw);
	bAimSmokeLogged = true;
	if (!bAutoMoveSmokeEnabled && !bFireSmokeEnabled)
	{
		ConsoleCommand(TEXT("quit"));
	}
}

void ACHPlayerController::UpdateAutoRestartSmoke(const float DeltaTime)
{
	if (!bRestartSmokeEnabled || !IsLocalController())
	{
		return;
	}

	RestartSmokeElapsedTime += DeltaTime;

	if (CHPlayerController::GRestartSmokeRunCount > 0)
	{
		if (bRestartSmokeRespawnLogged || RestartSmokeElapsedTime < 0.25f)
		{
			return;
		}

		ACHPlayerCharacter* const PlayerCharacter = GetPawn<ACHPlayerCharacter>();
		UCHHealthComponent* const HealthComponent = PlayerCharacter ? PlayerCharacter->GetHealthComponent() : nullptr;
		const ACHTopDownGameMode* const GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<ACHTopDownGameMode>() : nullptr;
		if (!PlayerCharacter || !HealthComponent || !GameMode)
		{
			return;
		}

		UE_LOG(
			LogCHPlayerController,
			Display,
			TEXT("CHRestartSmokeRespawned Health=%.2f Wave=%d Alive=%d GameOver=%d"),
			HealthComponent->GetCurrentHealth(),
			GameMode->GetCurrentWave(),
			GameMode->GetAliveEnemyCount(),
			GameMode->IsGameOver() ? 1 : 0);
		bRestartSmokeRespawnLogged = true;
		ConsoleCommand(TEXT("quit"));
		return;
	}

	ACHPlayerCharacter* const PlayerCharacter = GetPawn<ACHPlayerCharacter>();
	if (!PlayerCharacter)
	{
		return;
	}

	if (RestartSmokeStep == 0 && RestartSmokeElapsedTime >= 0.20f)
	{
		UGameplayStatics::ApplyDamage(PlayerCharacter, 999.0f, this, PlayerCharacter, UDamageType::StaticClass());
		UE_LOG(LogCHPlayerController, Display, TEXT("CHRestartSmokeLethalDamageApplied"));
		RestartSmokeStep = 1;
		RestartSmokeElapsedTime = 0.0f;
		return;
	}

	if (RestartSmokeStep == 1 && ObservedDeathCount > 0 && !bRestartSmokeRestartIssued && RestartSmokeElapsedTime >= 0.10f)
	{
		CHPlayerController::GRestartSmokeRunCount = 1;
		bRestartSmokeRestartIssued = ExecuteRestartRequest(true);
	}
}

void ACHPlayerController::UpdateAutoDamageSmoke(const float DeltaTime)
{
	if (!bDamageSmokeEnabled || !IsLocalController())
	{
		return;
	}

	ACHPlayerCharacter* const PlayerCharacter = GetPawn<ACHPlayerCharacter>();
	if (!PlayerCharacter)
	{
		return;
	}

	UCHHealthComponent* const HealthComponent = PlayerCharacter->GetHealthComponent();
	if (!HealthComponent)
	{
		return;
	}

	DamageSmokeElapsedTime += DeltaTime;

	if (DamageSmokeStep == 0 && DamageSmokeElapsedTime >= 0.20f)
	{
		const float AppliedDamage = UGameplayStatics::ApplyDamage(PlayerCharacter, 40.0f, this, PlayerCharacter, UDamageType::StaticClass());
		UE_LOG(
			LogCHPlayerController,
			Display,
			TEXT("CHDamageSmokeStep=1 Applied=%.2f Health=%.2f Dead=%d DeathCount=%d"),
			AppliedDamage,
			HealthComponent->GetCurrentHealth(),
			HealthComponent->IsDead() ? 1 : 0,
			ObservedDeathCount);
		++DamageSmokeStep;
		return;
	}

	if (DamageSmokeStep == 1 && DamageSmokeElapsedTime >= 0.40f)
	{
		const float AppliedDamage = UGameplayStatics::ApplyDamage(PlayerCharacter, 70.0f, this, PlayerCharacter, UDamageType::StaticClass());
		UE_LOG(
			LogCHPlayerController,
			Display,
			TEXT("CHDamageSmokeStep=2 Applied=%.2f Health=%.2f Dead=%d DeathCount=%d"),
			AppliedDamage,
			HealthComponent->GetCurrentHealth(),
			HealthComponent->IsDead() ? 1 : 0,
			ObservedDeathCount);
		++DamageSmokeStep;
		return;
	}

	if (DamageSmokeStep == 2 && DamageSmokeElapsedTime >= 0.60f)
	{
		const float AppliedDamage = UGameplayStatics::ApplyDamage(PlayerCharacter, 10.0f, this, PlayerCharacter, UDamageType::StaticClass());
		UE_LOG(
			LogCHPlayerController,
			Display,
			TEXT("CHDamageSmokeStep=3 Applied=%.2f Health=%.2f Dead=%d DeathCount=%d"),
			AppliedDamage,
			HealthComponent->GetCurrentHealth(),
			HealthComponent->IsDead() ? 1 : 0,
			ObservedDeathCount);
		++DamageSmokeStep;
		ConsoleCommand(TEXT("quit"));
	}
}

void ACHPlayerController::UpdateAutoFireSmoke(const float DeltaTime)
{
	if (!bFireSmokeEnabled || bFireSmokeTriggered || !IsLocalController())
	{
		return;
	}

	ACHPlayerCharacter* const PlayerCharacter = GetPawn<ACHPlayerCharacter>();
	if (!PlayerCharacter)
	{
		return;
	}

	FireSmokeElapsedTime += DeltaTime;
	if (FireSmokeElapsedTime < 0.25f)
	{
		return;
	}

	if (!ExecuteFireRequest(true))
	{
		return;
	}

	bFireSmokeTriggered = true;
	if (!bAutoMoveSmokeEnabled)
	{
		ConsoleCommand(TEXT("quit"));
	}
}

void ACHPlayerController::UpdateAutoMoveSmoke(float DeltaTime)
{
	if (!bAutoMoveSmokeEnabled || bAutoMoveSmokeFinished || !IsLocalController())
	{
		return;
	}

	ACHPlayerCharacter* const PlayerCharacter = GetPawn<ACHPlayerCharacter>();
	if (!PlayerCharacter)
	{
		return;
	}

	if (!bAutoMoveSmokeInitialized)
	{
		bAutoMoveSmokeInitialized = true;
		AutoMoveSmokeElapsedTime = 0.0f;
		AutoMoveSmokeStartLocation = PlayerCharacter->GetActorLocation();
		UE_LOG(LogCHPlayerController, Display, TEXT("CHAutoMoveSmoke 시작 위치: %s"), *AutoMoveSmokeStartLocation.ToCompactString());
	}

	AutoMoveSmokeElapsedTime += DeltaTime;

	if (AutoMoveSmokeElapsedTime < 0.60f)
	{
		HandleMoveAction(FInputActionValue(FVector2D(0.0f, 1.0f)));
		return;
	}

	if (AutoMoveSmokeElapsedTime < 0.85f)
	{
		return;
	}

	const float TravelDistance = FVector::Dist2D(PlayerCharacter->GetActorLocation(), AutoMoveSmokeStartLocation);
	UE_LOG(LogCHPlayerController, Display, TEXT("CHAutoMoveSmokeDistance=%.2f"), TravelDistance);
	bAutoMoveSmokeFinished = true;
	ConsoleCommand(TEXT("quit"));
}

void ACHPlayerController::HandleRestartAction()
{
	ExecuteRestartRequest(false);
}
