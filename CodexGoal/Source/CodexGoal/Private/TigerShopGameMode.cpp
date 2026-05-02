// Copyright Epic Games, Inc. All Rights Reserved.

#include "TigerShopGameMode.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "RiceCakeWorkstation.h"
#include "TemporaryTigerCharacter.h"
#include "TigerShopCameraRig.h"
#include "TigerShopPlayerController.h"

ATigerShopGameMode::ATigerShopGameMode()
{
	PlayerControllerClass = ATigerShopPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
}

void ATigerShopGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnPrototypeScene();

	ATigerShopPlayerController* TigerController = Cast<ATigerShopPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (TigerController)
	{
		if (CameraRig)
		{
			TigerController->SetViewTarget(CameraRig);
		}

		if (TemporaryTiger)
		{
			TigerController->RegisterTemporaryTiger(TemporaryTiger);
		}
	}
}

void ATigerShopGameMode::SpawnPrototypeScene()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	CameraRig = World->SpawnActor<ATigerShopCameraRig>(ATigerShopCameraRig::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	Workstation = World->SpawnActor<ARiceCakeWorkstation>(ARiceCakeWorkstation::StaticClass(), FVector(0.0f, 0.0f, 55.0f), FRotator::ZeroRotator);
	TemporaryTiger = World->SpawnActor<ATemporaryTigerCharacter>(ATemporaryTigerCharacter::StaticClass(), FVector(-220.0f, -35.0f, 0.0f), FRotator(0.0f, 35.0f, 0.0f));

	UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	SpawnStaticMeshProp(PlaneMesh, FVector(0.0f, 0.0f, -2.0f), FRotator::ZeroRotator, FVector(7.0f, 7.0f, 1.0f));
}

void ATigerShopGameMode::SpawnStaticMeshProp(UStaticMesh* Mesh, const FVector& Location, const FRotator& Rotation, const FVector& Scale)
{
	UWorld* World = GetWorld();
	if (!World || !Mesh)
	{
		return;
	}

	AStaticMeshActor* Prop = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, Rotation);
	if (!Prop)
	{
		return;
	}

	Prop->SetActorScale3D(Scale);
	UStaticMeshComponent* MeshComponent = Prop->GetStaticMeshComponent();
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(Mesh);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}
