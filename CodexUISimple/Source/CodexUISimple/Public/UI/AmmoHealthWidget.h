#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "AmmoHealthWidget.generated.h"

class UVerticalBox;
class UTextBlock;

/**
 * Minimal HUD widget built entirely in C++ that displays ammo and health stats.
 */
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
    /** Container created at runtime to hold text labels. */
    UPROPERTY(Transient)
    UVerticalBox* RootLayout;

    /** Text block showing ammo values. */
    UPROPERTY(Transient)
    UTextBlock* AmmoTextBlock;

    /** Text block showing health values. */
    UPROPERTY(Transient)
    UTextBlock* HealthTextBlock;
};
