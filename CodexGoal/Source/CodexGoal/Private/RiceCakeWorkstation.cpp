// Copyright Epic Games, Inc. All Rights Reserved.

#include "RiceCakeWorkstation.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ARiceCakeWorkstation::ARiceCakeWorkstation()
{
	PrimaryActorTick.bCanEverTick = true;

	WorkstationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorkstationMesh"));
	SetRootComponent(WorkstationMesh);
	WorkstationMesh->SetRelativeScale3D(FVector(1.8f, 1.2f, 0.35f));
	WorkstationMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WorkstationMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WorkstationMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		WorkstationMesh->SetStaticMesh(CubeMesh.Object);
	}

	LabelText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LabelText"));
	LabelText->SetupAttachment(WorkstationMesh);
	LabelText->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	LabelText->SetRelativeRotation(FRotator(65.0f, 0.0f, 0.0f));
	LabelText->SetHorizontalAlignment(EHTA_Center);
	LabelText->SetText(FText::FromString(TEXT("Rice Cake")));
	LabelText->SetWorldSize(34.0f);

	BaseMeshScale = WorkstationMesh->GetRelativeScale3D();
}

void ARiceCakeWorkstation::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (FeedbackTimeRemaining <= 0.0f)
	{
		return;
	}

	FeedbackTimeRemaining = FMath::Max(0.0f, FeedbackTimeRemaining - DeltaSeconds);
	const float Alpha = FeedbackTimeRemaining / 0.12f;
	const float ScalePulse = 1.0f + FMath::Sin(Alpha * PI) * 0.08f;
	WorkstationMesh->SetRelativeScale3D(BaseMeshScale * ScalePulse);

	if (FeedbackTimeRemaining <= 0.0f)
	{
		WorkstationMesh->SetRelativeScale3D(BaseMeshScale);
	}
}

void ARiceCakeWorkstation::PlayClickFeedback()
{
	FeedbackTimeRemaining = 0.12f;
}
