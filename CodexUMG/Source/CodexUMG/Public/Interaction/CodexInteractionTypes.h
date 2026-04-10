// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CodexInteractionTypes.generated.h"

class AActor;
class APlayerController;
class UCodexInteractionComponent;

UENUM(BlueprintType)
enum class ECodexInteractionType : uint8
{
	Eat,
	Pickup,
	Talk,
	Use
};

UENUM(BlueprintType)
enum class ECodexInteractionWidgetState : uint8
{
	Hidden,
	VisibleRange,
	Interactable
};

USTRUCT(BlueprintType)
struct CODEXUMG_API FCodexInteractionRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	TObjectPtr<APlayerController> RequestingController = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	TObjectPtr<UCodexInteractionComponent> InteractionComponent = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	ECodexInteractionType InteractionType = ECodexInteractionType::Use;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	FText PromptText;
};
