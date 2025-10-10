#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PSVRewardChest.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class APSVPlayerCharacter;
class USoundBase;

/**
 * Cosmetic upgrade chest actor that starts the roulette subsystem when the player overlaps.
 */
UCLASS()
class PINKSURVIVOR_API APSVRewardChest : public AActor
{
    GENERATED_BODY()

public:
    APSVRewardChest();

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
    void TriggerChest(APSVPlayerCharacter* PlayerCharacter);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Chest")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Chest")
    TObjectPtr<USphereComponent> TriggerComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chest")
    bool bConsumed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chest|Audio")
    TObjectPtr<USoundBase> ChestOpenSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Chest", meta=(ClampMin="0.1"))
    float RouletteDuration;
};
