// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Interaction/CodexInteractionTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "CodexInteractionSubsystem.generated.h"

class APlayerController;
class UCodexInteractionComponent;
class UUserWidget;

UCLASS()
class CODEXUMG_API UCodexInteractionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	void RegisterInteractionComponent(UCodexInteractionComponent* InteractionComponent);
	void UnregisterInteractionComponent(UCodexInteractionComponent* InteractionComponent);
	void RequestInteraction(APlayerController* RequestingController);
	bool OpenInteractionPopup(const FCodexInteractionPopupRequest& Request);
	void SubmitInteractionPopupResult(const FCodexInteractionPopupResponse& Response);
	void RequestCloseActivePopup(APlayerController* RequestingController);
	bool HasActivePopup() const;

	UCodexInteractionComponent* GetFocusedInteractionComponent() const;

private:
	void EndInteractionRequest(const FCodexInteractionRequest& Request);
	void CloseActivePopup();
	void ApplyPopupInputMode(APlayerController& PlayerController);
	void RestoreGameplayInputMode(APlayerController& PlayerController);
	void RefreshInteractionStates();
	void LogFocusTransition(UCodexInteractionComponent* PreviousFocus, UCodexInteractionComponent* NewFocus);
	void AddDebugMessage(const FString& Message, const FColor& Color = FColor::White, bool bAddOnScreen = true) const;
	FString DescribeInteractionComponent(const UCodexInteractionComponent* InteractionComponent) const;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCodexInteractionComponent>> RegisteredComponents;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionComponent> FocusedInteractionComponent;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> ActivePopupWidget;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> ActivePopupController;

	FCodexInteractionPopupRequest ActivePopupRequest;
	bool bHasActivePopupRequest = false;

	TMap<TObjectPtr<UCodexInteractionComponent>, int32> RegistrationOrder;
	int32 NextRegistrationOrder = 0;
};
