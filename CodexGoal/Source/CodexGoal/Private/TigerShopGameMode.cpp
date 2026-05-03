// Copyright Epic Games, Inc. All Rights Reserved.

#include "TigerShopGameMode.h"

#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextRenderActor.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
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
	TemporaryTiger = World->SpawnActor<ATemporaryTigerCharacter>(ATemporaryTigerCharacter::StaticClass(), FVector(-230.0f, -75.0f, 0.0f), FRotator(0.0f, 28.0f, 0.0f));

	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	SpawnStaticMeshProp(CubeMesh, FVector(0.0f, 0.0f, -8.0f), FRotator::ZeroRotator, FVector(8.0f, 7.0f, 0.08f), FLinearColor(0.48f, 0.36f, 0.24f));
	SpawnStaticMeshProp(CubeMesh, FVector(0.0f, 330.0f, 165.0f), FRotator::ZeroRotator, FVector(8.0f, 0.12f, 3.3f), FLinearColor(0.78f, 0.62f, 0.42f));
	SpawnStaticMeshProp(CubeMesh, FVector(-430.0f, 0.0f, 165.0f), FRotator::ZeroRotator, FVector(0.12f, 7.0f, 3.3f), FLinearColor(0.68f, 0.48f, 0.30f));
	SpawnStaticMeshProp(CubeMesh, FVector(-130.0f, 320.0f, 325.0f), FRotator::ZeroRotator, FVector(3.5f, 0.16f, 0.6f), FLinearColor(0.40f, 0.15f, 0.06f));
	SpawnTextProp(TEXT("Tiger Tteok Shop"), FVector(-310.0f, 304.0f, 318.0f), FRotator(0.0f, 0.0f, 0.0f), 34.0f, FColor(255, 230, 162));

	SpawnStaticMeshProp(CubeMesh, FVector(185.0f, 160.0f, 55.0f), FRotator::ZeroRotator, FVector(1.2f, 0.45f, 0.35f), FLinearColor(0.30f, 0.15f, 0.06f));
	SpawnStaticMeshProp(SphereMesh, FVector(155.0f, 160.0f, 118.0f), FRotator::ZeroRotator, FVector(0.22f, 0.22f, 0.22f), FLinearColor(0.95f, 0.42f, 0.08f));
	SpawnStaticMeshProp(SphereMesh, FVector(210.0f, 160.0f, 118.0f), FRotator::ZeroRotator, FVector(0.22f, 0.22f, 0.22f), FLinearColor(0.98f, 0.84f, 0.58f));
	SpawnStaticMeshProp(CylinderMesh, FVector(280.0f, -145.0f, 55.0f), FRotator::ZeroRotator, FVector(0.32f, 0.32f, 0.6f), FLinearColor(0.46f, 0.20f, 0.08f));
	SpawnStaticMeshProp(CylinderMesh, FVector(320.0f, -145.0f, 55.0f), FRotator::ZeroRotator, FVector(0.26f, 0.26f, 0.5f), FLinearColor(0.78f, 0.57f, 0.34f));
}

void ATigerShopGameMode::SpawnStaticMeshProp(UStaticMesh* Mesh, const FVector& Location, const FRotator& Rotation, const FVector& Scale, const FLinearColor& Color)
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

		UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		if (BaseMaterial)
		{
			MeshComponent->SetMaterial(0, BaseMaterial);
		}

		UMaterialInstanceDynamic* DynamicMaterial = MeshComponent->CreateDynamicMaterialInstance(0);
		if (DynamicMaterial)
		{
			DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
		}
	}
}

void ATigerShopGameMode::SpawnTextProp(const FString& Text, const FVector& Location, const FRotator& Rotation, float WorldSize, const FColor& Color)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ATextRenderActor* TextActor = World->SpawnActor<ATextRenderActor>(ATextRenderActor::StaticClass(), Location, Rotation);
	if (!TextActor || !TextActor->GetTextRender())
	{
		return;
	}

	UTextRenderComponent* TextComponent = TextActor->GetTextRender();
	TextComponent->SetHorizontalAlignment(EHTA_Center);
	TextComponent->SetText(FText::FromString(Text));
	TextComponent->SetTextRenderColor(Color);
	TextComponent->SetWorldSize(WorldSize);
}
