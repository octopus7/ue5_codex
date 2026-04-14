// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionSubsystem.h"

#include "Interaction/CodexInteractionAssetPaths.h"
#include "Interaction/CodexInteractionComponent.h"
#include "Interaction/CodexInteractionDualTileTransferPopupWidget.h"
#include "Interaction/CodexInteractionMessagePopupWidget.h"
#include "Interaction/CodexInteractionScrollMessagePopupWidget.h"
#include "Interaction/CodexPopupInteractableActor.h"
#include "Simple/GeminiFlashSimplePopupWidget.h"
#include "Interaction/CodexInteractionTarget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

namespace
{
	FString GetInteractionTypeName(const ECodexInteractionType InteractionType)
	{
		if (const UEnum* Enum = StaticEnum<ECodexInteractionType>())
		{
			return Enum->GetNameStringByValue(static_cast<int64>(InteractionType));
		}

		return TEXT("Unknown");
	}

	FString GetPopupResultName(const ECodexPopupResult PopupResult)
	{
		if (const UEnum* Enum = StaticEnum<ECodexPopupResult>())
		{
			return Enum->GetNameStringByValue(static_cast<int64>(PopupResult));
		}

		return TEXT("Unknown");
	}

	TSubclassOf<UUserWidget> ResolvePopupWidgetClass(const ECodexInteractionPopupStyle PopupStyle)
	{
		const TCHAR* AssetObjectPath = CodexInteractionAssetPaths::MessagePopupWidgetObjectPath;
		switch (PopupStyle)
		{
		case ECodexInteractionPopupStyle::ScrollMessage:
			AssetObjectPath = CodexInteractionAssetPaths::ScrollMessagePopupWidgetObjectPath;
			break;

		case ECodexInteractionPopupStyle::DualTileTransfer:
			AssetObjectPath = CodexInteractionAssetPaths::DualTileTransferPopupWidgetObjectPath;
			break;
		case ECodexInteractionPopupStyle::GeminiFlashSimple:
			AssetObjectPath = CodexInteractionAssetPaths::GeminiFlashSimplePopupWidgetObjectPath;
			break;

		case ECodexInteractionPopupStyle::Message:
		default:
			break;
		}

		FString ClassPath = CodexInteractionAssetPaths::MakeGeneratedClassObjectPath(AssetObjectPath);
		TSubclassOf<UUserWidget> LoadedClass = LoadClass<UUserWidget>(nullptr, *ClassPath);
		
		if (!LoadedClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load widget class at path: %s"), *ClassPath);
		}

		return LoadedClass;
	}
}

void UCodexInteractionSubsystem::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	RefreshInteractionStates();
}

TStatId UCodexInteractionSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCodexInteractionSubsystem, STATGROUP_Tickables);
}

void UCodexInteractionSubsystem::RegisterInteractionComponent(UCodexInteractionComponent* InteractionComponent)
{
	if (!IsValid(InteractionComponent) || RegisteredComponents.Contains(InteractionComponent))
	{
		return;
	}

	RegisteredComponents.Add(InteractionComponent);
	RegistrationOrder.Add(InteractionComponent, NextRegistrationOrder++);
}

void UCodexInteractionSubsystem::UnregisterInteractionComponent(UCodexInteractionComponent* InteractionComponent)
{
	if (!InteractionComponent)
	{
		return;
	}

	const bool bWasFocused = FocusedInteractionComponent == InteractionComponent;
	RegisteredComponents.Remove(InteractionComponent);
	RegistrationOrder.Remove(InteractionComponent);

	if (bWasFocused)
	{
		LogFocusTransition(InteractionComponent, nullptr);
		FocusedInteractionComponent = nullptr;
	}
}

void UCodexInteractionSubsystem::RequestInteraction(APlayerController* RequestingController)
{
	if (HasActivePopup())
	{
		return;
	}

	UCodexInteractionComponent* InteractionComponent = GetFocusedInteractionComponent();
	if (!IsValid(InteractionComponent) || !InteractionComponent->IsInteractionEnabled())
	{
		AddDebugMessage(TEXT("Interaction Requested: None"), FColor::Silver);
		return;
	}

	AActor* TargetActor = InteractionComponent->GetOwner();
	FCodexInteractionRequest Request;
	Request.RequestingController = RequestingController;
	Request.TargetActor = TargetActor;
	Request.InteractionComponent = InteractionComponent;
	Request.InteractionType = InteractionComponent->GetInteractionType();
	Request.PromptText = InteractionComponent->GetPromptText();

	const FString Description = DescribeInteractionComponent(InteractionComponent);
	AddDebugMessage(FString::Printf(TEXT("Interaction Requested: %s"), *Description), FColor::Yellow);

	if (ACodexPopupInteractableActor* PopupActor = Cast<ACodexPopupInteractableActor>(TargetActor))
	{
		if (IsValid(TargetActor) && TargetActor->GetClass()->ImplementsInterface(UCodexInteractionTarget::StaticClass()))
		{
			ICodexInteractionTarget::Execute_HandleInteractionRequested(TargetActor, Request);
		}

		FCodexInteractionPopupRequest PopupRequest;
		PopupRequest.RequestId = FGuid::NewGuid();
		PopupRequest.InteractionRequest = Request;
		PopupActor->PopulatePopupRequest(PopupRequest);

		if (!OpenInteractionPopup(PopupRequest))
		{
			AddDebugMessage(FString::Printf(TEXT("Interaction Popup Failed: %s"), *Description), FColor::Red);
			EndInteractionRequest(Request);
		}

		return;
	}

	if (IsValid(TargetActor) && TargetActor->GetClass()->ImplementsInterface(UCodexInteractionTarget::StaticClass()))
	{
		ICodexInteractionTarget::Execute_HandleInteractionRequested(TargetActor, Request);
	}

	EndInteractionRequest(Request);
}

bool UCodexInteractionSubsystem::OpenInteractionPopup(const FCodexInteractionPopupRequest& Request)
{
	if (HasActivePopup())
	{
		return false;
	}

	APlayerController* RequestingController = Request.InteractionRequest.RequestingController;
	if (!IsValid(RequestingController))
	{
		return false;
	}

	const TSubclassOf<UUserWidget> PopupWidgetClass = ResolvePopupWidgetClass(Request.PopupStyle);
	if (!PopupWidgetClass)
	{
		return false;
	}

	UUserWidget* PopupWidget = nullptr;
	if (Request.PopupStyle == ECodexInteractionPopupStyle::ScrollMessage)
	{
		UCodexInteractionScrollMessagePopupWidget* ScrollPopupWidget = CreateWidget<UCodexInteractionScrollMessagePopupWidget>(RequestingController, PopupWidgetClass);
		if (ScrollPopupWidget == nullptr)
		{
			return false;
		}

		ScrollPopupWidget->ApplyPopupRequest(Request, *this);
		PopupWidget = ScrollPopupWidget;
	}
	else if (Request.PopupStyle == ECodexInteractionPopupStyle::DualTileTransfer)
	{
		UCodexInteractionDualTileTransferPopupWidget* DualTilePopupWidget = CreateWidget<UCodexInteractionDualTileTransferPopupWidget>(RequestingController, PopupWidgetClass);
		if (DualTilePopupWidget == nullptr)
		{
			return false;
		}

		DualTilePopupWidget->ApplyPopupRequest(Request, *this);
		PopupWidget = DualTilePopupWidget;
	}
	else if (Request.PopupStyle == ECodexInteractionPopupStyle::GeminiFlashSimple)
	{
		UGeminiFlashSimplePopupWidget* GeminiPopupWidget = CreateWidget<UGeminiFlashSimplePopupWidget>(RequestingController, PopupWidgetClass);
		if (GeminiPopupWidget == nullptr)
		{
			return false;
		}

		GeminiPopupWidget->ApplyPopupRequest(Request, *this);
		PopupWidget = GeminiPopupWidget;
	}
	else
	{
		UCodexInteractionMessagePopupWidget* MessagePopupWidget = CreateWidget<UCodexInteractionMessagePopupWidget>(RequestingController, PopupWidgetClass);
		if (MessagePopupWidget == nullptr)
		{
			return false;
		}

		MessagePopupWidget->ApplyPopupRequest(Request, *this);
		PopupWidget = MessagePopupWidget;
	}

	if (PopupWidget == nullptr)
	{
		return false;
	}

	ActivePopupRequest = Request;
	bHasActivePopupRequest = true;
	ActivePopupController = RequestingController;
	ActivePopupWidget = PopupWidget;

	PopupWidget->AddToViewport(1000);
	ApplyPopupInputMode(*RequestingController);
	return true;
}

void UCodexInteractionSubsystem::SubmitInteractionPopupResult(const FCodexInteractionPopupResponse& Response)
{
	if (!HasActivePopup() || !bHasActivePopupRequest || Response.RequestId != ActivePopupRequest.RequestId)
	{
		return;
	}

	const FCodexInteractionPopupRequest CompletedRequest = ActivePopupRequest;
	FCodexInteractionPopupResponse CompletedResponse = Response;
	if (CompletedResponse.InteractionRequest.TargetActor == nullptr)
	{
		CompletedResponse.InteractionRequest = CompletedRequest.InteractionRequest;
	}

	CloseActivePopup();

	AddDebugMessage(
		FString::Printf(
			TEXT("Interaction Popup Result: %s -> %s"),
			*DescribeInteractionComponent(CompletedRequest.InteractionRequest.InteractionComponent),
			*GetPopupResultName(CompletedResponse.Result)),
		FColor::Green);

	AActor* TargetActor = CompletedResponse.InteractionRequest.TargetActor;
	if (IsValid(TargetActor) && TargetActor->GetClass()->ImplementsInterface(UCodexInteractionTarget::StaticClass()))
	{
		ICodexInteractionTarget::Execute_HandleInteractionPopupResult(TargetActor, CompletedResponse);
	}

	EndInteractionRequest(CompletedResponse.InteractionRequest);
}

void UCodexInteractionSubsystem::RequestCloseActivePopup(APlayerController* RequestingController)
{
	if (!HasActivePopup() || !bHasActivePopupRequest)
	{
		return;
	}

	if (!ActivePopupRequest.bAllowControllerClose)
	{
		return;
	}

	if (IsValid(RequestingController) && IsValid(ActivePopupController) && RequestingController != ActivePopupController)
	{
		return;
	}

	if (ActivePopupRequest.PopupStyle == ECodexInteractionPopupStyle::DualTileTransfer)
	{
		if (UCodexInteractionDualTileTransferPopupWidget* DualTilePopupWidget = Cast<UCodexInteractionDualTileTransferPopupWidget>(ActivePopupWidget))
		{
			DualTilePopupWidget->HandleControllerCloseRequested();
			return;
		}
	}

	FCodexInteractionPopupResponse Response;
	Response.RequestId = ActivePopupRequest.RequestId;
	Response.InteractionRequest = ActivePopupRequest.InteractionRequest;
	Response.Result = ECodexPopupResult::Closed;
	Response.bWasClosed = true;
	SubmitInteractionPopupResult(Response);
}

bool UCodexInteractionSubsystem::HasActivePopup() const
{
	return ActivePopupWidget != nullptr && bHasActivePopupRequest;
}

UCodexInteractionComponent* UCodexInteractionSubsystem::GetFocusedInteractionComponent() const
{
	return FocusedInteractionComponent;
}

void UCodexInteractionSubsystem::EndInteractionRequest(const FCodexInteractionRequest& Request)
{
	AddDebugMessage(
		FString::Printf(
			TEXT("Interaction Ended: %s"),
			*DescribeInteractionComponent(Request.InteractionComponent)),
		FColor::Orange);

	AActor* TargetActor = Request.TargetActor;
	if (IsValid(TargetActor) && TargetActor->GetClass()->ImplementsInterface(UCodexInteractionTarget::StaticClass()))
	{
		ICodexInteractionTarget::Execute_HandleInteractionEnded(TargetActor, Request);
	}
}

void UCodexInteractionSubsystem::CloseActivePopup()
{
	if (IsValid(ActivePopupWidget))
	{
		ActivePopupWidget->RemoveFromParent();
	}

	if (IsValid(ActivePopupController))
	{
		RestoreGameplayInputMode(*ActivePopupController);
	}

	ActivePopupWidget = nullptr;
	ActivePopupController = nullptr;
	ActivePopupRequest = FCodexInteractionPopupRequest();
	bHasActivePopupRequest = false;
}

void UCodexInteractionSubsystem::ApplyPopupInputMode(APlayerController& PlayerController)
{
	if (ActivePopupWidget == nullptr)
	{
		return;
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(ActivePopupWidget->TakeWidget());
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController.SetInputMode(InputMode);
	PlayerController.bShowMouseCursor = true;
}

void UCodexInteractionSubsystem::RestoreGameplayInputMode(APlayerController& PlayerController)
{
	FInputModeGameOnly InputMode;
	PlayerController.SetInputMode(InputMode);
	PlayerController.bShowMouseCursor = false;
}

void UCodexInteractionSubsystem::RefreshInteractionStates()
{
	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	APawn* PlayerPawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	const FVector PlayerLocation = PlayerPawn ? PlayerPawn->GetActorLocation() : (PlayerController ? PlayerController->GetFocalLocation() : FVector::ZeroVector);
	const bool bHasPlayerContext = PlayerController != nullptr;

	UCodexInteractionComponent* BestComponent = nullptr;
	float BestDistanceSq = TNumericLimits<float>::Max();
	int32 BestRegistrationOrder = TNumericLimits<int32>::Max();

	for (int32 Index = RegisteredComponents.Num() - 1; Index >= 0; --Index)
	{
		UCodexInteractionComponent* InteractionComponent = RegisteredComponents[Index];
		if (!IsValid(InteractionComponent) || !IsValid(InteractionComponent->GetOwner()))
		{
			RegistrationOrder.Remove(InteractionComponent);
			RegisteredComponents.RemoveAtSwap(Index);
			continue;
		}

		if (!bHasPlayerContext || !InteractionComponent->IsInteractionEnabled())
		{
			InteractionComponent->ApplyInteractionWidgetState(ECodexInteractionWidgetState::Hidden);
			continue;
		}

		const float DistanceSq = FVector::DistSquared(PlayerLocation, InteractionComponent->GetInteractionWorldLocation());
		const float VisibleDistanceSq = FMath::Square(InteractionComponent->GetVisibleDistance());
		const float InteractableDistanceSq = FMath::Square(InteractionComponent->GetInteractableDistance());

		if (DistanceSq > VisibleDistanceSq)
		{
			InteractionComponent->ApplyInteractionWidgetState(ECodexInteractionWidgetState::Hidden);
			continue;
		}

		InteractionComponent->ApplyInteractionWidgetState(ECodexInteractionWidgetState::VisibleRange);

		if (DistanceSq <= InteractableDistanceSq)
		{
			const int32 ComponentRegistrationOrder = RegistrationOrder.FindRef(InteractionComponent);
			const bool bIsBetterDistance = DistanceSq < BestDistanceSq - 0.01f;
			const bool bIsStableTieBreak = FMath::IsNearlyEqual(DistanceSq, BestDistanceSq, 0.01f) && ComponentRegistrationOrder < BestRegistrationOrder;
			if (!BestComponent || bIsBetterDistance || bIsStableTieBreak)
			{
				BestComponent = InteractionComponent;
				BestDistanceSq = DistanceSq;
				BestRegistrationOrder = ComponentRegistrationOrder;
			}
		}
	}

	UCodexInteractionComponent* PreviousFocus = FocusedInteractionComponent;
	if (PreviousFocus != BestComponent)
	{
		LogFocusTransition(PreviousFocus, BestComponent);
		FocusedInteractionComponent = BestComponent;
	}

	for (UCodexInteractionComponent* InteractionComponent : RegisteredComponents)
	{
		if (!IsValid(InteractionComponent) || !InteractionComponent->IsInteractionEnabled())
		{
			continue;
		}

		if (InteractionComponent == BestComponent)
		{
			InteractionComponent->ApplyInteractionWidgetState(ECodexInteractionWidgetState::Interactable);
		}
	}
}

void UCodexInteractionSubsystem::LogFocusTransition(UCodexInteractionComponent* PreviousFocus, UCodexInteractionComponent* NewFocus)
{
	if (IsValid(PreviousFocus))
	{
		AddDebugMessage(FString::Printf(TEXT("Interaction Focus End: %s"), *DescribeInteractionComponent(PreviousFocus)), FColor::Cyan, false);
	}

	if (IsValid(NewFocus))
	{
		AddDebugMessage(FString::Printf(TEXT("Interaction Focus Start: %s"), *DescribeInteractionComponent(NewFocus)), FColor::Green, false);
	}
}

void UCodexInteractionSubsystem::AddDebugMessage(const FString& Message, const FColor& Color, const bool bAddOnScreen) const
{
	if (bAddOnScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, Color, Message);
	}

	UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
}

FString UCodexInteractionSubsystem::DescribeInteractionComponent(const UCodexInteractionComponent* InteractionComponent) const
{
	if (!InteractionComponent)
	{
		return TEXT("None");
	}

	const AActor* Owner = InteractionComponent->GetOwner();
	const FString ActorName = Owner ? Owner->GetName() : TEXT("None");
	return FString::Printf(TEXT("%s / %s"), *ActorName, *GetInteractionTypeName(InteractionComponent->GetInteractionType()));
}
