#include "EnemyConeCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

AEnemyConeCharacter::AEnemyConeCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // Add a visual static mesh (cone) attached to capsule
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(GetCapsuleComponent());
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // Ensure enemy capsule can overlap with projectile (WorldDynamic)
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
    if (ConeMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(ConeMesh.Object);
        // Flip upside-down: pitch 180 so tip points down to ground
        VisualMesh->SetRelativeRotation(FRotator(180.f, 0.f, 0.f));
        VisualMesh->SetRelativeLocation(FVector(0.f, 0.f, -44.f)); // pull mesh down a bit to match capsule height
        VisualMesh->SetRelativeScale3D(FVector(1.0f));
    }

    // HP text above head
    HPText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HPText"));
    HPText->SetupAttachment(GetCapsuleComponent());
    HPText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    HPText->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
    HPText->SetWorldSize(36.f);
    HPText->SetTextRenderColor(FColor::White);
    HPText->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
    UpdateHPText();
}

float AEnemyConeCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float SuperDealt = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    CurrentHits++;
    if (GEngine)
    {
        FString Msg = FString::Printf(TEXT("Enemy hit: %d / %d"), CurrentHits, HitsToDie);
        GEngine->AddOnScreenDebugMessage(reinterpret_cast<uint64>(this), 1.0f, FColor::Red, Msg);
    }

    if (CurrentHits >= HitsToDie)
    {
        HandleDeath();
    }
    else
    {
        UpdateHPText();
    }

    return SuperDealt + DamageAmount;
}

void AEnemyConeCharacter::HandleDeath()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT("Enemy died"));
    }
    Destroy();
}

void AEnemyConeCharacter::UpdateHPText()
{
    if (!HPText)
    {
        return;
    }
    const int32 Remaining = FMath::Max(0, HitsToDie - CurrentHits);
    HPText->SetText(FText::AsNumber(Remaining));
    if (Remaining <= 1)
    {
        HPText->SetTextRenderColor(FColor::Red);
    }
    else if (Remaining == 2)
    {
        HPText->SetTextRenderColor(FColor::Yellow);
    }
    else
    {
        HPText->SetTextRenderColor(FColor::White);
    }
}
