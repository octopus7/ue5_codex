// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenPickupActor.h"

#include "CodexInvenOwnershipComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"

ACodexInvenPickupActor::ACodexInvenPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetCanBeDamaged(false);

	PickupSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphereComponent"));
	PickupSphereComponent->InitSphereRadius(PickupRadius);
	PickupSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	PickupSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupSphereComponent->SetGenerateOverlapEvents(true);
	RootComponent = PickupSphereComponent;

	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMeshComponent"));
	PickupMeshComponent->SetupAttachment(RootComponent);
	PickupMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		PickupMeshComponent->SetStaticMesh(CubeMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> BasicShapeMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial_Inst.BasicShapeMaterial_Inst"));
	if (BasicShapeMaterial.Succeeded())
	{
		PickupMeshComponent->SetMaterial(0, BasicShapeMaterial.Object);
	}

	PickupSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandlePickupSphereBeginOverlap);
}

void ACodexInvenPickupActor::OnConstruction(const FTransform& InTransform)
{
	Super::OnConstruction(InTransform);

	ApplyPickupDefinition();
}

void ACodexInvenPickupActor::InitializeFromPickupType(const ECodexInvenPickupType InPickupType)
{
	PickupType = InPickupType;
	ApplyPickupDefinition();
}

ECodexInvenPickupType ACodexInvenPickupActor::GetPickupType() const
{
	return PickupType;
}

void ACodexInvenPickupActor::ApplyPickupDefinition()
{
	static UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	static UStaticMesh* CylinderMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static UMaterialInterface* StandardPickupMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial_Inst.BasicShapeMaterial_Inst"));
	static UMaterialInterface* MetallicPickupMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/TemplateResources/MI_Template_BaseOrange_Metal.MI_Template_BaseOrange_Metal"));

	PickupSphereComponent->SetSphereRadius(PickupRadius);

	const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(PickupType);
	switch (Definition.MeshKind)
	{
	case ECodexInvenPickupMeshKind::Cube:
		if (CubeMesh != nullptr)
		{
			PickupMeshComponent->SetStaticMesh(CubeMesh);
		}
		PickupMeshComponent->SetRelativeScale3D(CubeVisualScale);
		break;

	case ECodexInvenPickupMeshKind::Cylinder:
		if (CylinderMesh != nullptr)
		{
			PickupMeshComponent->SetStaticMesh(CylinderMesh);
		}
		PickupMeshComponent->SetRelativeScale3D(CylinderVisualScale);
		break;
	}

	UMaterialInterface* BaseMaterial = Definition.bUseMetallicMaterial && MetallicPickupMaterial != nullptr
		? MetallicPickupMaterial
		: StandardPickupMaterial;

	if (BaseMaterial != nullptr)
	{
		PickupMaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		if (PickupMaterialInstance != nullptr)
		{
			PickupMeshComponent->SetMaterial(0, PickupMaterialInstance);
			PickupMaterialInstance->SetVectorParameterValue(TEXT("Color"), Definition.TintColor);
			PickupMaterialInstance->SetVectorParameterValue(TEXT("BaseColor"), Definition.TintColor);
			PickupMaterialInstance->SetVectorParameterValue(TEXT("Tint"), Definition.TintColor);
			PickupMaterialInstance->SetVectorParameterValue(TEXT("TintColor"), Definition.TintColor);
			PickupMaterialInstance->SetScalarParameterValue(TEXT("Metallic"), Definition.bUseMetallicMaterial ? 1.0f : 0.0f);
			PickupMaterialInstance->SetScalarParameterValue(TEXT("Roughness"), Definition.bUseMetallicMaterial ? 0.18f : 0.55f);
			PickupMaterialInstance->SetScalarParameterValue(TEXT("Specular"), Definition.bUseMetallicMaterial ? 0.85f : 0.5f);
			return;
		}
	}

	PickupMeshComponent->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Definition.TintColor.R, Definition.TintColor.G, Definition.TintColor.B));
}

void ACodexInvenPickupActor::HandlePickupSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	const int32 OtherBodyIndex,
	const bool bFromSweep,
	const FHitResult& SweepResult)
{
	static_cast<void>(OverlappedComponent);
	static_cast<void>(OtherComp);
	static_cast<void>(OtherBodyIndex);
	static_cast<void>(bFromSweep);
	static_cast<void>(SweepResult);

	if (!HasAuthority() || OtherActor == nullptr || OtherActor == this)
	{
		return;
	}

	const APawn* OverlappingPawn = Cast<APawn>(OtherActor);
	if (OverlappingPawn == nullptr || !OverlappingPawn->IsPlayerControlled())
	{
		return;
	}

	UCodexInvenOwnershipComponent* OwnershipComponent = OtherActor->FindComponentByClass<UCodexInvenOwnershipComponent>();
	if (OwnershipComponent == nullptr || !OwnershipComponent->AddPickup(PickupType))
	{
		return;
	}

	PickupSphereComponent->SetGenerateOverlapEvents(false);
	PickupSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}
