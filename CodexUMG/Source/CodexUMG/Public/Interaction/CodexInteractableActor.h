// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/CodexInteractionTarget.h"
#include "CodexInteractableActor.generated.h"

class UStaticMeshComponent;
class UCodexInteractionComponent;
class USceneComponent;

UCLASS(Blueprintable)
class CODEXUMG_API ACodexInteractableActor : public AActor, public ICodexInteractionTarget
{
	GENERATED_BODY()

public:
	ACodexInteractableActor();

	virtual void HandleInteractionRequested_Implementation(const FCodexInteractionRequest& Request) override;
	virtual void HandleInteractionEnded_Implementation(const FCodexInteractionRequest& Request) override;

	UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
	UCodexInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Codex|Interaction", meta = (DisplayName = "On Interaction Requested"))
	void ReceiveInteractionRequested(const FCodexInteractionRequest& Request);

	UFUNCTION(BlueprintImplementableEvent, Category = "Codex|Interaction", meta = (DisplayName = "On Interaction Ended"))
	void ReceiveInteractionEnded(const FCodexInteractionRequest& Request);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Codex|Interaction")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Codex|Interaction")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Codex|Interaction")
	TObjectPtr<UCodexInteractionComponent> InteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Codex|Interaction", meta = (ClampMin = "0.0"))
	float ConsumeDelaySeconds = 0.15f;

private:
	void ConsumeAndDestroy();
	bool bPendingConsume = false;
};
