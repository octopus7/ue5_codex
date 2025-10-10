#include "Pickups/PSVGoldCoin.h"

#include "Characters/PSVPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/PSVGameInstance.h"

APSVGoldCoin::APSVGoldCoin()
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

    GoldValue = 1;
    bCollected = false;

    SetCanBeDamaged(false);
}

void APSVGoldCoin::BeginPlay()
{
    Super::BeginPlay();

    if (CollisionComponent)
    {
        CollisionComponent->SetGenerateOverlapEvents(true);
    }
}

void APSVGoldCoin::NotifyActorBeginOverlap(AActor* OtherActor)
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

void APSVGoldCoin::SetGoldValue(int32 NewValue)
{
    GoldValue = FMath::Max(0, NewValue);
}

void APSVGoldCoin::HandleCollected(APSVPlayerCharacter* PlayerCharacter)
{
    if (bCollected)
    {
        return;
    }

    bCollected = true;

    UPSVGameInstance* PSVGameInstance = nullptr;

    if (UWorld* World = GetWorld())
    {
        PSVGameInstance = Cast<UPSVGameInstance>(World->GetGameInstance());
    }

    if (GoldValue > 0 && PSVGameInstance)
    {
        PSVGameInstance->AddPersistentGold(GoldValue);
    }

    if (PlayerCharacter && PSVGameInstance)
    {
        PlayerCharacter->HandlePersistentGoldChanged(PSVGameInstance->GetPersistentGold());
    }

    Destroy();
}
