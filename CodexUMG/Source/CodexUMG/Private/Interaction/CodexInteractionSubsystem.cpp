// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionSubsystem.h"

#include "Interaction/CodexInteractionComponent.h"
#include "Interaction/CodexInteractionTarget.h"
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

	if (IsValid(TargetActor) && TargetActor->GetClass()->ImplementsInterface(UCodexInteractionTarget::StaticClass()))
	{
		ICodexInteractionTarget::Execute_HandleInteractionRequested(TargetActor, Request);
	}

	AddDebugMessage(FString::Printf(TEXT("Interaction Ended: %s"), *Description), FColor::Orange);

	if (IsValid(TargetActor) && TargetActor->GetClass()->ImplementsInterface(UCodexInteractionTarget::StaticClass()))
	{
		ICodexInteractionTarget::Execute_HandleInteractionEnded(TargetActor, Request);
	}
}

UCodexInteractionComponent* UCodexInteractionSubsystem::GetFocusedInteractionComponent() const
{
	return FocusedInteractionComponent;
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
