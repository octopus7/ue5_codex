#pragma once

#include "GameFramework/PlayerController.h"

#include "CHPlayerController.generated.h"

class UInputAction;
class UCHHealthComponent;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class CODEXHARNESS_API ACHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACHPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

private:
	void EnsureInputDefinitions();
	void ApplyGameplayMappingContext();
	void HandleMoveAction(const FInputActionValue& InputActionValue);
	void HandleFireAction();
	bool ExecuteFireRequest(bool bFromSmoke);
	bool ExecuteRestartRequest(bool bFromSmoke);
	void EnsureHealthObservation();
	void HandleObservedDeath(UCHHealthComponent* InHealthComponent);
	bool ResolveAimWorldPoint(FVector& OutAimWorldPoint) const;
	void UpdateAimTarget(float DeltaTime);
	void UpdateAutoDamageSmoke(float DeltaTime);
	void UpdateAutoFireSmoke(float DeltaTime);
	void UpdateAutoRestartSmoke(float DeltaTime);
	void UpdateAutoMoveSmoke(float DeltaTime);
	void HandleRestartAction();

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> GameplayMappingContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> RestartAction;

	bool bAutoMoveSmokeEnabled = false;
	bool bAutoMoveSmokeInitialized = false;
	bool bAutoMoveSmokeFinished = false;
	bool bAimSmokeEnabled = false;
	bool bAimSmokeLogged = false;
	bool bDamageSmokeEnabled = false;
	bool bFireSmokeEnabled = false;
	bool bFireSmokeTriggered = false;
	bool bHealthObservationBound = false;
	bool bHasAimWorldPoint = false;
	bool bRestartSmokeEnabled = false;
	bool bRestartSmokeRespawnLogged = false;
	bool bRestartSmokeRestartIssued = false;
	float CachedMoveDeltaSeconds = 0.016f;
	float AutoMoveSmokeElapsedTime = 0.0f;
	float AimSmokeElapsedTime = 0.0f;
	float DamageSmokeElapsedTime = 0.0f;
	float FireSmokeElapsedTime = 0.0f;
	float RestartSmokeElapsedTime = 0.0f;
	int32 DamageSmokeStep = 0;
	int32 ObservedDeathCount = 0;
	int32 RestartSmokeStep = 0;
	FVector AutoMoveSmokeStartLocation = FVector::ZeroVector;
	FVector CurrentAimWorldPoint = FVector::ZeroVector;
};
