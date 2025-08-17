#include "EnemyConeCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyConeCharacter::AEnemyConeCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

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
    HPText->SetWorldSize(108.f);
    HPText->SetTextRenderColor(FColor::White);
    HPText->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
    UpdateHPText();

    // Lifetime text above HP (50% size)
    LifeText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LifeText"));
    LifeText->SetupAttachment(GetCapsuleComponent());
    LifeText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    LifeText->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
    LifeText->SetWorldSize(54.f);
    LifeText->SetTextRenderColor(FColor::Cyan);
    LifeText->SetRelativeLocation(FVector(0.f, 0.f, 190.f));
    LifeText->SetHiddenInGame(true); // will unhide if AutoDeathTime > 0 in BeginPlay

    // AI setup
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = AEnemyAIController::StaticClass();

    // Movement setup
    bUseControllerRotationYaw = false;
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->bOrientRotationToMovement = true;
        MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
        MoveComp->bConstrainToPlane = true;
        MoveComp->SetPlaneConstraintNormal(FVector::UpVector);
    }
}

void AEnemyConeCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 스폰 후 자동 사망 시간 적용 (0이면 비활성)
    if (AutoDeathTime > 0.f)
    {
        SetLifeSpan(AutoDeathTime);
        if (LifeText)
        {
            LifeText->SetHiddenInGame(false);
            UpdateLifetimeText();
        }
    }
    else
    {
        SetLifeSpan(0.f);
        if (LifeText)
        {
            LifeText->SetHiddenInGame(true);
        }
    }
}

void AEnemyConeCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    CachedAIController = Cast<AEnemyAIController>(NewController);
    UpdateLifetimeText();
}

void AEnemyConeCharacter::UnPossessed()
{
    Super::UnPossessed();
    CachedAIController = nullptr;
    UpdateLifetimeText();
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

    // State text (English). If AI is null, show "null"
    FString StateStr;
    AEnemyAIController* AI = CachedAIController ? CachedAIController : Cast<AEnemyAIController>(GetController());
    if (AI)
    {
        switch (AI->GetState())
        {
        case EEnemyState::Idle:   StateStr = TEXT("Idle");   break;
        case EEnemyState::Patrol: StateStr = TEXT("Patrol"); break;
        case EEnemyState::Chase:  StateStr = TEXT("Chase");  break;
        default:                  StateStr = TEXT("Idle");   break;
        }
    }
    else
    {
        StateStr = TEXT("null");
    }

    // Append extra info when Idle: remaining idle seconds (if any)
    FString Extra;
    if (AI && AI->GetState() == EEnemyState::Idle)
    {
        const float LeftIdle = AI->GetIdleTimeRemaining();
        if (LeftIdle > 0.f)
        {
            Extra = FString::Printf(TEXT(" (%.1fs)"), LeftIdle);
        }
    }
    const FString Txt = FString::Printf(TEXT("%d | %s%s"), Remaining, *StateStr, *Extra);
    HPText->SetText(FText::FromString(Txt));

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

void AEnemyConeCharacter::UpdateLifetimeText()
{
    if (!LifeText)
    {
        return;
    }
    const float Left = GetLifeSpan();
    if (Left <= 0.f)
    {
        LifeText->SetHiddenInGame(true);
        return;
    }
    LifeText->SetHiddenInGame(false);
    const int32 Secs = FMath::Max(0, FMath::CeilToInt(Left));
    LifeText->SetText(FText::AsNumber(Secs));
}

void AEnemyConeCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (AutoDeathTime > 0.f)
    {
        UpdateLifetimeText();
    }
    // 상태는 HP 텍스트에 표시되므로 매 틱 갱신
    UpdateHPText();

    // 머리 위 텍스트를 카메라 방향으로 정렬
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (PC->PlayerCameraManager)
        {
            const FRotator CamRot = PC->PlayerCameraManager->GetCameraRotation();
            // 텍스트가 카메라를 바라보도록 Yaw만 맞춤(+180도로 반전)
            const FRotator FaceRot(0.f, CamRot.Yaw + 180.f, 0.f);
            if (HPText)
            {
                HPText->SetWorldRotation(FaceRot);
            }
            if (LifeText && !LifeText->bHiddenInGame)
            {
                LifeText->SetWorldRotation(FaceRot);
            }
        }
    }
}
