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

UENUM(BlueprintType)
enum class ECodexPopupButtonLayout : uint8
{
	Ok,
	YesNo
};

UENUM(BlueprintType)
enum class ECodexInteractionPopupStyle : uint8
{
	Message,
	ScrollMessage
};

UENUM(BlueprintType)
enum class ECodexPopupResult : uint8
{
	Ok,
	Yes,
	No,
	Closed
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

USTRUCT(BlueprintType)
struct CODEXUMG_API FCodexInteractionPopupRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	FGuid RequestId;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	FCodexInteractionRequest InteractionRequest;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	FText Title;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	FText Message;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	ECodexPopupButtonLayout ButtonLayout = ECodexPopupButtonLayout::Ok;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	ECodexInteractionPopupStyle PopupStyle = ECodexInteractionPopupStyle::Message;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	bool bAllowControllerClose = true;
};

USTRUCT(BlueprintType)
struct CODEXUMG_API FCodexInteractionPopupResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	FGuid RequestId;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	FCodexInteractionRequest InteractionRequest;

	UPROPERTY(BlueprintReadOnly, Category = "Codex|Interaction")
	ECodexPopupResult Result = ECodexPopupResult::Closed;
};
