#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PSVAutoFireComponent.generated.h"

class APSVProjectile;

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class PINKSURVIVOR_API UPSVAutoFireComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPSVAutoFireComponent();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category="AutoFire")
    void StartFiring();

    UFUNCTION(BlueprintCallable, Category="AutoFire")
    void StopFiring();

protected:
    void HandleFire();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AutoFire", meta=(ClampMin="0.05"))
    float FireInterval = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AutoFire")
    TSubclassOf<APSVProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AutoFire")
    FVector SpawnOffset = FVector(100.f, 0.f, 80.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AutoFire")
    float ProjectileSpeedOverride = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AutoFire")
    bool bStartOnBeginPlay = true;

private:
    FTimerHandle FireTimerHandle;
};
