// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "CodexInteractionSubsystem.generated.h"

class APlayerController;
class UCodexInteractionComponent;

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

	UCodexInteractionComponent* GetFocusedInteractionComponent() const;

private:
	void RefreshInteractionStates();
	void LogFocusTransition(UCodexInteractionComponent* PreviousFocus, UCodexInteractionComponent* NewFocus);
	void AddDebugMessage(const FString& Message, const FColor& Color = FColor::White) const;
	FString DescribeInteractionComponent(const UCodexInteractionComponent* InteractionComponent) const;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UCodexInteractionComponent>> RegisteredComponents;

	UPROPERTY(Transient)
	TObjectPtr<UCodexInteractionComponent> FocusedInteractionComponent;

	TMap<TObjectPtr<UCodexInteractionComponent>, int32> RegistrationOrder;
	int32 NextRegistrationOrder = 0;
};
