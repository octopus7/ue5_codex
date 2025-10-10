#include "Pickups/PSVRewardChest.h"

#include "Characters/PSVPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Core/PSVChestRouletteSubsystem.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

APSVRewardChest::APSVRewardChest()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    SetRootComponent(MeshComponent);

    TriggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerComponent"));
    TriggerComponent->SetupAttachment(MeshComponent);
    TriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerComponent->InitSphereRadius(120.f);
    TriggerComponent->SetGenerateOverlapEvents(true);

    bConsumed = false;
    RouletteDuration = 2.0f;
}

void APSVRewardChest::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    if (APSVPlayerCharacter* PlayerCharacter = Cast<APSVPlayerCharacter>(OtherActor))
    {
        TriggerChest(PlayerCharacter);
    }
}

void APSVRewardChest::TriggerChest(APSVPlayerCharacter* PlayerCharacter)
{
    if (bConsumed || !PlayerCharacter)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    if (UGameInstance* GameInstance = World->GetGameInstance())
    {
        if (UPSVChestRouletteSubsystem* RouletteSubsystem = GameInstance->GetSubsystem<UPSVChestRouletteSubsystem>())
        {
            if (RouletteSubsystem->StartRoulette(PlayerCharacter, RouletteDuration))
            {
                bConsumed = true;

                if (ChestOpenSound)
                {
                    UGameplayStatics::PlaySoundAtLocation(this, ChestOpenSound, GetActorLocation());
                }

                SetActorHiddenInGame(true);
                SetActorEnableCollision(false);
            }
        }
    }
}
