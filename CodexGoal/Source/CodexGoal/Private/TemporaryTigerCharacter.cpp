// Copyright Epic Games, Inc. All Rights Reserved.

#include "TemporaryTigerCharacter.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

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

	if (CubeMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeMesh.Object);
		LeftArmMesh->SetStaticMesh(CubeMesh.Object);
		RightArmMesh->SetStaticMesh(CubeMesh.Object);
	}

	if (SphereMesh.Succeeded())
	{
		HeadMesh->SetStaticMesh(SphereMesh.Object);
	}
}

void ATemporaryTigerCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultHeadLocation = HeadBone->GetRelativeLocation();
	DefaultLeftArmRotation = LeftArmBone->GetRelativeRotation();
	DefaultRightArmRotation = RightArmBone->GetRelativeRotation();
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
