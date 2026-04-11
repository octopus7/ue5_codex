// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexInteractionComponent.h"

#include "Interaction/CodexInteractionAssetPaths.h"
#include "Interaction/CodexInteractionIndicatorWidget.h"
#include "Interaction/CodexInteractionSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

UCodexInteractionComponent::UCodexInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCodexInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	EnsureIndicatorWidget();
	ApplyInteractionWidgetState(ECodexInteractionWidgetState::Hidden);

	if (UWorld* World = GetWorld())
	{
		if (UCodexInteractionSubsystem* InteractionSubsystem = World->GetSubsystem<UCodexInteractionSubsystem>())
		{
			InteractionSubsystem->RegisterInteractionComponent(this);
		}
	}
}

void UCodexInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UCodexInteractionSubsystem* InteractionSubsystem = World->GetSubsystem<UCodexInteractionSubsystem>())
		{
			InteractionSubsystem->UnregisterInteractionComponent(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

FVector UCodexInteractionComponent::GetInteractionWorldLocation() const
{
	const AActor* Owner = GetOwner();
	return Owner ? Owner->GetActorLocation() + WidgetOffset : WidgetOffset;
}

void UCodexInteractionComponent::ApplyInteractionWidgetState(const ECodexInteractionWidgetState NewState)
{
	CurrentWidgetState = bInteractionEnabled ? NewState : ECodexInteractionWidgetState::Hidden;
	RefreshIndicatorWidget();
}

void UCodexInteractionComponent::RequestInteraction(APlayerController* RequestingController)
{
	if (!bInteractionEnabled)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (UCodexInteractionSubsystem* InteractionSubsystem = World->GetSubsystem<UCodexInteractionSubsystem>())
		{
			InteractionSubsystem->RequestInteraction(RequestingController);
		}
	}
}

void UCodexInteractionComponent::SetInteractionType(const ECodexInteractionType NewInteractionType)
{
	InteractionType = NewInteractionType;
}

void UCodexInteractionComponent::SetPromptText(const FText& NewPromptText)
{
	PromptText = NewPromptText;
	RefreshIndicatorWidget();
}

void UCodexInteractionComponent::SetVisibleDistance(const float NewVisibleDistance)
{
	VisibleDistance = FMath::Max(0.0f, NewVisibleDistance);
	if (InteractableDistance > VisibleDistance)
	{
		InteractableDistance = VisibleDistance;
	}
}

void UCodexInteractionComponent::SetInteractableDistance(const float NewInteractableDistance)
{
	InteractableDistance = FMath::Clamp(NewInteractableDistance, 0.0f, VisibleDistance);
}

void UCodexInteractionComponent::SetInteractionEnabled(const bool bEnabled)
{
	bInteractionEnabled = bEnabled;
	ApplyInteractionWidgetState(bInteractionEnabled ? CurrentWidgetState : ECodexInteractionWidgetState::Hidden);
}

void UCodexInteractionComponent::EnsureIndicatorWidget()
{
	if (IndicatorWidgetComponent || !GetOwner() || IsTemplate() || GetOwner()->HasAnyFlags(RF_ClassDefaultObject) || GetWorld() == nullptr)
	{
		return;
	}

	USceneComponent* RootComponent = GetOwner()->GetRootComponent();
	if (!RootComponent)
	{
		return;
	}

	TSubclassOf<UUserWidget> ResolvedWidgetClass = ResolveIndicatorWidgetClass();
	if (!ResolvedWidgetClass)
	{
		return;
	}

	IndicatorWidgetComponent = NewObject<UWidgetComponent>(GetOwner(), TEXT("CodexInteractionIndicatorWidget"));
	if (!IndicatorWidgetComponent)
	{
		return;
	}

	GetOwner()->AddOwnedComponent(IndicatorWidgetComponent);
	IndicatorWidgetComponent->SetupAttachment(RootComponent);
	IndicatorWidgetComponent->SetRelativeLocation(WidgetOffset);
	IndicatorWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	IndicatorWidgetComponent->SetBlendMode(EWidgetBlendMode::Transparent);
	IndicatorWidgetComponent->SetTickWhenOffscreen(true);
	IndicatorWidgetComponent->SetTwoSided(false);
	IndicatorWidgetComponent->SetDrawSize(FVector2D(280.0f, 72.0f));
	IndicatorWidgetComponent->SetPivot(FVector2D(24.0f / 280.0f, 0.5f));
	IndicatorWidgetComponent->SetWidgetClass(ResolvedWidgetClass);
	IndicatorWidgetComponent->SetCastShadow(false);
	IndicatorWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IndicatorWidgetComponent->RegisterComponent();
	IndicatorWidgetComponent->InitWidget();
}

void UCodexInteractionComponent::RefreshIndicatorWidget()
{
	if (IsTemplate() || GetOwner() == nullptr || GetOwner()->HasAnyFlags(RF_ClassDefaultObject) || GetWorld() == nullptr)
	{
		return;
	}

	EnsureIndicatorWidget();
	if (!IndicatorWidgetComponent)
	{
		return;
	}

	IndicatorWidgetComponent->SetRelativeLocation(WidgetOffset);
	IndicatorWidgetComponent->InitWidget();

	if (UCodexInteractionIndicatorWidget* IndicatorWidget = Cast<UCodexInteractionIndicatorWidget>(IndicatorWidgetComponent->GetUserWidgetObject()))
	{
		IndicatorWidget->ApplyInteractionState(CurrentWidgetState, PromptText);
	}
}

TSubclassOf<UUserWidget> UCodexInteractionComponent::ResolveIndicatorWidgetClass() const
{
	if (IndicatorWidgetClass)
	{
		return IndicatorWidgetClass;
	}

	return LoadClass<UUserWidget>(nullptr, *CodexInteractionAssetPaths::MakeGeneratedClassObjectPath(CodexInteractionAssetPaths::IndicatorWidgetObjectPath));
}
