#include "UI/AmmoHealthWidget.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Blueprint/WidgetTree.h"

void UAmmoHealthWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!WidgetTree)
    {
        return;
    }

    RootLayout = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    WidgetTree->RootWidget = RootLayout;

    AmmoTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    HealthTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());

    if (RootLayout)
    {
        if (UVerticalBoxSlot* AmmoSlot = RootLayout->AddChildToVerticalBox(AmmoTextBlock))
        {
            AmmoSlot->SetPadding(FMargin(8.f, 8.f));
        }

        if (UVerticalBoxSlot* HealthSlot = RootLayout->AddChildToVerticalBox(HealthTextBlock))
        {
            HealthSlot->SetPadding(FMargin(8.f, 8.f));
        }
    }

    SetAmmo(0, 0);
    SetHealth(0.f, 0.f);
}

void UAmmoHealthWidget::SetAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (!AmmoTextBlock)
    {
        return;
    }

    const FString AmmoString = FString::Printf(TEXT("Ammo: %d / %d"), CurrentAmmo, MaxAmmo);
    AmmoTextBlock->SetText(FText::FromString(AmmoString));
}

void UAmmoHealthWidget::SetHealth(float CurrentHealth, float MaxHealth)
{
    if (!HealthTextBlock)
    {
        return;
    }

    const int32 RoundedCurrent = FMath::RoundToInt(CurrentHealth);
    const int32 RoundedMax = FMath::RoundToInt(MaxHealth);

    const FString HealthString = FString::Printf(TEXT("Health: %d / %d"), RoundedCurrent, RoundedMax);
    HealthTextBlock->SetText(FText::FromString(HealthString));
}
