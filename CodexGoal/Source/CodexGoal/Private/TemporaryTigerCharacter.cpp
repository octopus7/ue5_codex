// Copyright Epic Games, Inc. All Rights Reserved.

#include "TemporaryTigerCharacter.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	void TintTigerMesh(UStaticMeshComponent* Mesh, const FLinearColor& Color)
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

ATemporaryTigerCharacter::ATemporaryTigerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SkeletonRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SkeletonRoot"));
	SetRootComponent(SkeletonRoot);

	BodyBone = CreateDefaultSubobject<USceneComponent>(TEXT("BodyBone"));
	BodyBone->SetupAttachment(SkeletonRoot);
	BodyBone->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));

	HeadBone = CreateDefaultSubobject<USceneComponent>(TEXT("HeadBone"));
	HeadBone->SetupAttachment(BodyBone);
	HeadBone->SetRelativeLocation(FVector(0.0f, 0.0f, 95.0f));

	LeftArmBone = CreateDefaultSubobject<USceneComponent>(TEXT("LeftArmBone"));
	LeftArmBone->SetupAttachment(BodyBone);
	LeftArmBone->SetRelativeLocation(FVector(0.0f, -55.0f, 35.0f));
	LeftArmBone->SetRelativeRotation(FRotator(0.0f, 0.0f, -18.0f));

	RightArmBone = CreateDefaultSubobject<USceneComponent>(TEXT("RightArmBone"));
	RightArmBone->SetupAttachment(BodyBone);
	RightArmBone->SetRelativeLocation(FVector(0.0f, 55.0f, 35.0f));
	RightArmBone->SetRelativeRotation(FRotator(0.0f, 0.0f, 18.0f));

	TailBone = CreateDefaultSubobject<USceneComponent>(TEXT("TailBone"));
	TailBone->SetupAttachment(BodyBone);
	TailBone->SetRelativeLocation(FVector(-48.0f, 0.0f, -18.0f));
	TailBone->SetRelativeRotation(FRotator(0.0f, -25.0f, 32.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(BodyBone);
	BodyMesh->SetRelativeScale3D(FVector(0.65f, 0.45f, 0.85f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(HeadBone);
	HeadMesh->SetRelativeScale3D(FVector(0.42f, 0.42f, 0.42f));
	HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LeftArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftArmMesh"));
	LeftArmMesh->SetupAttachment(LeftArmBone);
	LeftArmMesh->SetRelativeScale3D(FVector(0.18f, 0.18f, 0.7f));
	LeftArmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RightArmMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightArmMesh"));
	RightArmMesh->SetupAttachment(RightArmBone);
	RightArmMesh->SetRelativeScale3D(FVector(0.18f, 0.18f, 0.7f));
	RightArmMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LeftEarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftEarMesh"));
	LeftEarMesh->SetupAttachment(HeadBone);
	LeftEarMesh->SetRelativeLocation(FVector(-12.0f, -30.0f, 38.0f));
	LeftEarMesh->SetRelativeScale3D(FVector(0.16f, 0.12f, 0.24f));
	LeftEarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RightEarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightEarMesh"));
	RightEarMesh->SetupAttachment(HeadBone);
	RightEarMesh->SetRelativeLocation(FVector(-12.0f, 30.0f, 38.0f));
	RightEarMesh->SetRelativeScale3D(FVector(0.16f, 0.12f, 0.24f));
	RightEarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SnoutMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SnoutMesh"));
	SnoutMesh->SetupAttachment(HeadBone);
	SnoutMesh->SetRelativeLocation(FVector(34.0f, 0.0f, -4.0f));
	SnoutMesh->SetRelativeScale3D(FVector(0.22f, 0.2f, 0.14f));
	SnoutMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TailMesh"));
	TailMesh->SetupAttachment(TailBone);
	TailMesh->SetRelativeLocation(FVector(-35.0f, 0.0f, 0.0f));
	TailMesh->SetRelativeScale3D(FVector(0.12f, 0.12f, 0.65f));
	TailMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BodyStripeA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStripeA"));
	BodyStripeA->SetupAttachment(BodyBone);
	BodyStripeA->SetRelativeLocation(FVector(35.0f, -18.0f, 25.0f));
	BodyStripeA->SetRelativeRotation(FRotator(0.0f, 0.0f, 18.0f));
	BodyStripeA->SetRelativeScale3D(FVector(0.055f, 0.12f, 0.5f));
	BodyStripeA->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BodyStripeB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStripeB"));
	BodyStripeB->SetupAttachment(BodyBone);
	BodyStripeB->SetRelativeLocation(FVector(36.0f, 10.0f, 5.0f));
	BodyStripeB->SetRelativeRotation(FRotator(0.0f, 0.0f, -14.0f));
	BodyStripeB->SetRelativeScale3D(FVector(0.055f, 0.12f, 0.45f));
	BodyStripeB->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BodyStripeC = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStripeC"));
	BodyStripeC->SetupAttachment(BodyBone);
	BodyStripeC->SetRelativeLocation(FVector(36.0f, 24.0f, 30.0f));
	BodyStripeC->SetRelativeRotation(FRotator(0.0f, 0.0f, -24.0f));
	BodyStripeC->SetRelativeScale3D(FVector(0.055f, 0.12f, 0.42f));
	BodyStripeC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HeadStripe = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadStripe"));
	HeadStripe->SetupAttachment(HeadBone);
	HeadStripe->SetRelativeLocation(FVector(32.0f, 0.0f, 28.0f));
	HeadStripe->SetRelativeScale3D(FVector(0.045f, 0.28f, 0.06f));
	HeadStripe->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
		LeftArmMesh->SetStaticMesh(CubeMesh.Object);
		RightArmMesh->SetStaticMesh(CubeMesh.Object);
		LeftEarMesh->SetStaticMesh(CubeMesh.Object);
		RightEarMesh->SetStaticMesh(CubeMesh.Object);
		TailMesh->SetStaticMesh(CubeMesh.Object);
		BodyStripeA->SetStaticMesh(CubeMesh.Object);
		BodyStripeB->SetStaticMesh(CubeMesh.Object);
		BodyStripeC->SetStaticMesh(CubeMesh.Object);
		HeadStripe->SetStaticMesh(CubeMesh.Object);
	}

	if (SphereMesh.Succeeded())
	{
		HeadMesh->SetStaticMesh(SphereMesh.Object);
		SnoutMesh->SetStaticMesh(SphereMesh.Object);
	}
}

void ATemporaryTigerCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultHeadLocation = HeadBone->GetRelativeLocation();
	DefaultLeftArmRotation = LeftArmBone->GetRelativeRotation();
	DefaultRightArmRotation = RightArmBone->GetRelativeRotation();

	const FLinearColor TigerOrange(0.95f, 0.42f, 0.08f);
	const FLinearColor WarmCream(0.98f, 0.84f, 0.58f);
	const FLinearColor StripeBlack(0.035f, 0.025f, 0.02f);
	TintTigerMesh(BodyMesh, TigerOrange);
	TintTigerMesh(HeadMesh, TigerOrange);
	TintTigerMesh(LeftArmMesh, TigerOrange);
	TintTigerMesh(RightArmMesh, TigerOrange);
	TintTigerMesh(LeftEarMesh, TigerOrange);
	TintTigerMesh(RightEarMesh, TigerOrange);
	TintTigerMesh(SnoutMesh, WarmCream);
	TintTigerMesh(TailMesh, TigerOrange);
	TintTigerMesh(BodyStripeA, StripeBlack);
	TintTigerMesh(BodyStripeB, StripeBlack);
	TintTigerMesh(BodyStripeC, StripeBlack);
	TintTigerMesh(HeadStripe, StripeBlack);
}

void ATemporaryTigerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PulseTimeRemaining <= 0.0f)
	{
		return;
	}

	PulseTimeRemaining = FMath::Max(0.0f, PulseTimeRemaining - DeltaSeconds);
	constexpr float PulseDuration = 0.28f;
	const float Progress = 1.0f - (PulseTimeRemaining / PulseDuration);
	const float Wave = FMath::Sin(Progress * PI);

	HeadBone->SetRelativeLocation(DefaultHeadLocation + FVector(0.0f, 0.0f, Wave * 16.0f));
	LeftArmBone->SetRelativeRotation(DefaultLeftArmRotation + FRotator(Wave * -16.0f, 0.0f, 0.0f));
	RightArmBone->SetRelativeRotation(DefaultRightArmRotation + FRotator(Wave * 16.0f, 0.0f, 0.0f));

	if (PulseTimeRemaining <= 0.0f)
	{
		HeadBone->SetRelativeLocation(DefaultHeadLocation);
		LeftArmBone->SetRelativeRotation(DefaultLeftArmRotation);
		RightArmBone->SetRelativeRotation(DefaultRightArmRotation);
	}
}

void ATemporaryTigerCharacter::PlayProductionPulse()
{
	PulseTimeRemaining = 0.28f;
}
