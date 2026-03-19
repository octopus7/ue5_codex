// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenPickupActor.h"

#include "CodexInvenOwnershipComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogCodexInvenPickupActor, Log, All);

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
	static UStaticMesh* FallbackMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));

	PickupSphereComponent->SetSphereRadius(PickupRadius);

	const FCodexInvenPickupDefinition& Definition = CodexInvenPickupData::GetPickupDefinitionChecked(PickupType);
	UStaticMesh* PickupMesh = Definition.WorldMesh.LoadSynchronous();
	if (PickupMesh == nullptr)
	{
		UE_LOG(LogCodexInvenPickupActor, Warning, TEXT("Pickup mesh asset is missing for %s. Falling back to Cube."), *Definition.DisplayName);
		PickupMesh = FallbackMesh;
	}

	PickupMeshComponent->SetStaticMesh(PickupMesh);
	PickupMeshComponent->SetRelativeLocation(FVector::ZeroVector);
	PickupMeshComponent->SetRelativeRotation(FRotator::ZeroRotator);
	PickupMeshComponent->SetRelativeScale3D(FVector::OneVector);
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
