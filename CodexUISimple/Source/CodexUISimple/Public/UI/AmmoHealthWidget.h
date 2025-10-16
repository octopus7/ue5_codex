#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "AmmoHealthWidget.generated.h"

class UTextBlock;

/** Minimal HUD widget that exposes ammo/health labels for a designer-authored Widget Blueprint. */
UCLASS()
class CODEXUISIMPLE_API UAmmoHealthWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    /** Update the ammo read-out. */
    void SetAmmo(int32 CurrentAmmo, int32 MaxAmmo);

    /** Update the health read-out. */
    void SetHealth(float CurrentHealth, float MaxHealth);

private:
    /** Text block showing ammo values. Populated via the Widget Blueprint hierarchy. */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* AmmoTextBlock;

    /** Text block showing health values. Populated via the Widget Blueprint hierarchy. */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthTextBlock;
};
