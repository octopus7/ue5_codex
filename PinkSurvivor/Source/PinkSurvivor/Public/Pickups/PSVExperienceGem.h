#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PSVExperienceGem.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class APSVPlayerCharacter;

/**
 * Experience gem actor dropped by defeated enemies.
 * Tier and experience value are exposed so that Blueprint variants can define multiple grades.
 * When the player overlaps, the stored experience is awarded automatically.
 */
UCLASS()
class PINKSURVIVOR_API APSVExperienceGem : public AActor
{
    GENERATED_BODY()

public:
    APSVExperienceGem();

    virtual void BeginPlay() override;
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    UFUNCTION(BlueprintCallable, Category="Experience")
    void SetExperienceValue(int32 NewValue);

    UFUNCTION(BlueprintCallable, Category="Experience")
    int32 GetExperienceValue() const { return ExperienceValue; }

    UFUNCTION(BlueprintCallable, Category="Experience")
    int32 GetGemTier() const { return GemTier; }

protected:
    void HandleCollected(APSVPlayerCharacter* PlayerCharacter);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<USphereComponent> CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    /** Experience amount awarded when collected. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Experience", meta=(ClampMin="0"))
    int32 ExperienceValue;

    /** Optional tier identifier for Blueprint-driven presentation. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Experience", meta=(ClampMin="0"))
    int32 GemTier;

    bool bCollected;
};
