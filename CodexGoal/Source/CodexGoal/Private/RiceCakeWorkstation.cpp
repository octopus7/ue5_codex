// Copyright Epic Games, Inc. All Rights Reserved.

#include "RiceCakeWorkstation.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	void TintMesh(UStaticMeshComponent* Mesh, const FLinearColor& Color)
	{
		if (!Mesh)
		{
			return;
		}

		UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		if (BaseMaterial)
		{
			Mesh->SetMaterial(0, BaseMaterial);
		}

		UMaterialInstanceDynamic* DynamicMaterial = Mesh->CreateDynamicMaterialInstance(0);
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
		}
	}
}

ARiceCakeWorkstation::ARiceCakeWorkstation()
{
	PrimaryActorTick.bCanEverTick = true;

	WorkstationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorkstationMesh"));
	SetRootComponent(WorkstationMesh);
	WorkstationMesh->SetRelativeScale3D(FVector(2.1f, 1.35f, 0.28f));
	WorkstationMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WorkstationMesh->SetCollisionResponseToAllChannels(ECR_Block);
	WorkstationMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CubeMesh.Succeeded())
	{
		WorkstationMesh->SetStaticMesh(CubeMesh.Object);
	}

	TrayMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrayMesh"));
	TrayMesh->SetupAttachment(WorkstationMesh);
	TrayMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 48.0f));
	TrayMesh->SetRelativeScale3D(FVector(0.42f, 0.42f, 0.055f));
	TrayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RiceCakeA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RiceCakeA"));
	RiceCakeA->SetupAttachment(TrayMesh);
	RiceCakeA->SetRelativeLocation(FVector(-34.0f, -12.0f, 24.0f));
	RiceCakeA->SetRelativeScale3D(FVector(0.22f, 0.22f, 0.12f));
	RiceCakeA->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RiceCakeB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RiceCakeB"));
	RiceCakeB->SetupAttachment(TrayMesh);
	RiceCakeB->SetRelativeLocation(FVector(8.0f, 20.0f, 27.0f));
	RiceCakeB->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.12f));
	RiceCakeB->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RiceCakeC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RiceCakeC"));
	RiceCakeC->SetupAttachment(TrayMesh);
	RiceCakeC->SetRelativeLocation(FVector(38.0f, -18.0f, 25.0f));
	RiceCakeC->SetRelativeScale3D(FVector(0.18f, 0.18f, 0.1f));
	RiceCakeC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MalletHandle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MalletHandle"));
	MalletHandle->SetupAttachment(WorkstationMesh);
	MalletHandle->SetRelativeLocation(FVector(85.0f, -60.0f, 72.0f));
	MalletHandle->SetRelativeRotation(FRotator(0.0f, 0.0f, -32.0f));
	MalletHandle->SetRelativeScale3D(FVector(0.08f, 0.08f, 0.7f));
	MalletHandle->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MalletHead = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MalletHead"));
	MalletHead->SetupAttachment(MalletHandle);
	MalletHead->SetRelativeLocation(FVector(0.0f, 0.0f, 48.0f));
	MalletHead->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	MalletHead->SetRelativeScale3D(FVector(0.18f, 0.18f, 0.32f));
	MalletHead->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (CylinderMesh.Succeeded())
	{
		TrayMesh->SetStaticMesh(CylinderMesh.Object);
		RiceCakeA->SetStaticMesh(CylinderMesh.Object);
		RiceCakeB->SetStaticMesh(CylinderMesh.Object);
		RiceCakeC->SetStaticMesh(CylinderMesh.Object);
		MalletHead->SetStaticMesh(CylinderMesh.Object);
	}

	if (CubeMesh.Succeeded())
	{
		MalletHandle->SetStaticMesh(CubeMesh.Object);
	}

	LabelText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LabelText"));
	LabelText->SetupAttachment(WorkstationMesh);
	LabelText->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f));
	LabelText->SetRelativeRotation(FRotator(65.0f, 0.0f, 0.0f));
	LabelText->SetHorizontalAlignment(EHTA_Center);
	LabelText->SetText(FText::FromString(TEXT("TTEOK")));
	LabelText->SetWorldSize(38.0f);
	LabelText->SetTextRenderColor(FColor(255, 244, 214));

	BaseMeshScale = WorkstationMesh->GetRelativeScale3D();
}

void ARiceCakeWorkstation::BeginPlay()
{
	Super::BeginPlay();

	TintMesh(WorkstationMesh, FLinearColor(0.36f, 0.18f, 0.08f));
	TintMesh(TrayMesh, FLinearColor(0.72f, 0.45f, 0.22f));
	TintMesh(RiceCakeA, FLinearColor(0.96f, 0.92f, 0.83f));
	TintMesh(RiceCakeB, FLinearColor(0.98f, 0.87f, 0.75f));
	TintMesh(RiceCakeC, FLinearColor(0.88f, 0.96f, 0.82f));
	TintMesh(MalletHandle, FLinearColor(0.28f, 0.13f, 0.055f));
	TintMesh(MalletHead, FLinearColor(0.45f, 0.24f, 0.11f));
}

void ARiceCakeWorkstation::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FaceLabelToCamera();

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

void ARiceCakeWorkstation::FaceLabelToCamera() const
{
	if (!LabelText)
	{
		return;
	}

	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager)
	{
		return;
	}

	const FVector ToCamera = CameraManager->GetCameraLocation() - LabelText->GetComponentLocation();
	if (ToCamera.IsNearlyZero())
	{
		return;
	}

	LabelText->SetWorldRotation(ToCamera.Rotation());
}
