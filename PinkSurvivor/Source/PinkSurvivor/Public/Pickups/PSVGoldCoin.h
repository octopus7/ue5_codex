#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PSVGoldCoin.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class APSVPlayerCharacter;

/**
 * Persistent currency pickup that increases the player's gold balance even across sessions.
 */
UCLASS()
class PINKSURVIVOR_API APSVGoldCoin : public AActor
{
    GENERATED_BODY()

public:
    APSVGoldCoin();

    virtual void BeginPlay() override;
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    UFUNCTION(BlueprintCallable, Category="Currency")
    void SetGoldValue(int32 NewValue);

    UFUNCTION(BlueprintCallable, Category="Currency")
    int32 GetGoldValue() const { return GoldValue; }

protected:
    void HandleCollected(APSVPlayerCharacter* PlayerCharacter);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<USphereComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Currency", meta=(ClampMin="0"))
    int32 GoldValue;

    bool bCollected;
};
