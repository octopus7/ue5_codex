#include "Pickups/PSVExperienceGem.h"

#include "Characters/PSVPlayerCharacter.h"
#include "Components/PSVExperienceComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

APSVExperienceGem::APSVExperienceGem()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    CollisionComponent->SetSphereRadius(50.f);

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CollisionComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ExperienceValue = 5;
    GemTier = 0;
    bCollected = false;

    SetCanBeDamaged(false);
}

void APSVExperienceGem::BeginPlay()
{
    Super::BeginPlay();

    if (CollisionComponent)
    {
        CollisionComponent->SetGenerateOverlapEvents(true);
    }
}

void APSVExperienceGem::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (bCollected || !OtherActor)
    {
        return;
    }

    if (APSVPlayerCharacter* PlayerCharacter = Cast<APSVPlayerCharacter>(OtherActor))
    {
        HandleCollected(PlayerCharacter);
    }
}

void APSVExperienceGem::SetExperienceValue(int32 NewValue)
{
    ExperienceValue = FMath::Max(0, NewValue);
}

void APSVExperienceGem::HandleCollected(APSVPlayerCharacter* PlayerCharacter)
{
    if (!PlayerCharacter || bCollected)
    {
        return;
    }

    bCollected = true;

    if (ExperienceValue > 0)
    {
        if (UPSVExperienceComponent* ExperienceComponent = PlayerCharacter->FindComponentByClass<UPSVExperienceComponent>())
        {
            ExperienceComponent->GainExperience(ExperienceValue);
        }
    }

    Destroy();
}
