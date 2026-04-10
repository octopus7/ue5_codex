// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "Interaction/CodexInteractionTypes.h"
#include "CodexInteractionComponent.generated.h"

class APlayerController;
class UUserWidget;
class UWidgetComponent;

UCLASS(ClassGroup = (Codex), meta = (BlueprintSpawnableComponent))
class CODEXUMG_API UCodexInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCodexInteractionComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	ECodexInteractionType GetInteractionType() const { return InteractionType; }
	ECodexInteractionWidgetState GetCurrentWidgetState() const { return CurrentWidgetState; }
	const FText& GetPromptText() const { return PromptText; }
	float GetVisibleDistance() const { return VisibleDistance; }
	float GetInteractableDistance() const { return InteractableDistance; }
	const FVector& GetWidgetOffset() const { return WidgetOffset; }
	bool IsInteractionEnabled() const { return bInteractionEnabled; }
	FVector GetInteractionWorldLocation() const;

	void ApplyInteractionWidgetState(ECodexInteractionWidgetState NewState);
	void RequestInteraction(APlayerController* RequestingController);

	void SetInteractionType(ECodexInteractionType NewInteractionType);
	void SetPromptText(const FText& NewPromptText);
	void SetVisibleDistance(float NewVisibleDistance);
	void SetInteractableDistance(float NewInteractableDistance);
	void SetInteractionEnabled(bool bEnabled);

	UWidgetComponent* GetIndicatorWidgetComponent() const { return IndicatorWidgetComponent; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Codex|Interaction")
	ECodexInteractionType InteractionType = ECodexInteractionType::Use;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Codex|Interaction")
	FText PromptText = FText::FromString(TEXT("Interact"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Codex|Interaction", meta = (ClampMin = "0.0"))
	float VisibleDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Codex|Interaction", meta = (ClampMin = "0.0"))
	float InteractableDistance = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Codex|Interaction")
	FVector WidgetOffset = FVector(0.0f, 0.0f, 80.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Codex|Interaction")
	TSubclassOf<UUserWidget> IndicatorWidgetClass;

private:
	void EnsureIndicatorWidget();
	void RefreshIndicatorWidget();
	TSubclassOf<UUserWidget> ResolveIndicatorWidgetClass() const;

	UPROPERTY(Transient)
	TObjectPtr<UWidgetComponent> IndicatorWidgetComponent;

	ECodexInteractionWidgetState CurrentWidgetState = ECodexInteractionWidgetState::Hidden;
	bool bInteractionEnabled = true;
};
