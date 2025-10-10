#include "Characters/PSVPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PSVAutoFireComponent.h"
#include "Components/PSVExperienceComponent.h"
#include "Components/PSVHealthComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/PSVGameInstance.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "UI/PSVHUD.h"

APSVPlayerCharacter::APSVPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->bUsePawnControlRotation = false;
    CameraBoom->bInheritPitch = false;
    CameraBoom->bInheritYaw = false;
    CameraBoom->bInheritRoll = false;
    CameraBoom->bDoCollisionTest = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    AutoFireComponent = CreateDefaultSubobject<UPSVAutoFireComponent>(TEXT("AutoFireComponent"));

    HealthComponent = CreateDefaultSubobject<UPSVHealthComponent>(TEXT("HealthComponent"));

    ExperienceComponent = CreateDefaultSubobject<UPSVExperienceComponent>(TEXT("ExperienceComponent"));

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->bOrientRotationToMovement = true;
        Movement->RotationRate = FRotator(0.f, 720.f, 0.f);
        Movement->MaxWalkSpeed = 600.f;
        Movement->BrakingDecelerationWalking = 2048.f;
        Movement->bUseControllerDesiredRotation = false;
    }

    if (UCapsuleComponent* Capsule = GetCapsuleComponent())
    {
        Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    }
}

void APSVPlayerCharacter::HandleExperienceChanged(int32 CurrentExperienceValue, int32 CurrentLevelValue)
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (APSVHUD* PSVHUD = PlayerController->GetHUD<APSVHUD>())
        {
            const int32 ExperienceToNextLevel = ExperienceComponent ? ExperienceComponent->GetExperienceToNextLevel() : 0;
            PSVHUD->HandlePlayerExperienceChanged(CurrentExperienceValue, CurrentLevelValue, ExperienceToNextLevel);
        }
    }
}

void APSVPlayerCharacter::HandleLevelUp(int32 NewLevel, int32 TotalExperience)
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (APSVHUD* PSVHUD = PlayerController->GetHUD<APSVHUD>())
        {
            PSVHUD->HandlePlayerLevelUp(NewLevel, TotalExperience);
        }
    }
}

void APSVPlayerCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (CameraBoom)
    {
        CameraBoom->TargetArmLength = CameraBoomLength;
        CameraBoom->SetRelativeRotation(CameraRelativeRotation);
    }
}

void APSVPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitializeHealth();
    InitializeExperience();

    if (AController* LocalController = GetController())
    {
        if (APlayerController* PlayerController = Cast<APlayerController>(LocalController))
        {
            if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
            {
                if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
                {
                    if (DefaultInputMapping)
                    {
                        Subsystem->AddMappingContext(DefaultInputMapping, 0);
                    }
                }
            }

            const FRotator CameraYawOnly(0.f, CameraRelativeRotation.Yaw, 0.f);
            PlayerController->SetControlRotation(CameraYawOnly);
        }
    }

    if (UWorld* World = GetWorld())
    {
        if (UPSVGameInstance* PSVGameInstance = Cast<UPSVGameInstance>(World->GetGameInstance()))
        {
            HandlePersistentGoldChanged(PSVGameInstance->GetPersistentGold());
        }
    }
}

void APSVPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APSVPlayerCharacter::Move);
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APSVPlayerCharacter::Move);
        }
    }
}

void APSVPlayerCharacter::Move(const FInputActionValue& Value)
{
    if (!Controller || bIsDead)
    {
        return;
    }

    const FVector2D MovementVector = Value.Get<FVector2D>();
    if (MovementVector.IsNearlyZero())
    {
        return;
    }

    const float MovementYaw = bAlignMovementToCameraYaw
        ? (CameraBoom ? CameraBoom->GetComponentRotation().Yaw : CameraRelativeRotation.Yaw)
        : 0.f;

    const FRotator MovementRotation(0.f, MovementYaw, 0.f);
    const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
    const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);

    AddMovementInput(ForwardDirection, MovementVector.Y);
    AddMovementInput(RightDirection, MovementVector.X);
}

void APSVPlayerCharacter::InitializeHealth()
{
    bIsDead = false;

    if (!HealthComponent)
    {
        return;
    }

    HealthComponent->OnHealthChanged.AddDynamic(this, &APSVPlayerCharacter::HandleHealthChanged);
    HealthComponent->OnDeath.AddDynamic(this, &APSVPlayerCharacter::HandleDeath);
    HealthComponent->InitializeHealth(MaxHealth, true);
}

void APSVPlayerCharacter::InitializeExperience()
{
    if (!ExperienceComponent)
    {
        return;
    }

    ExperienceComponent->OnExperienceChanged.AddDynamic(this, &APSVPlayerCharacter::HandleExperienceChanged);
    ExperienceComponent->OnLevelUp.AddDynamic(this, &APSVPlayerCharacter::HandleLevelUp);
    ExperienceComponent->ResetProgression();
}

float APSVPlayerCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.f && HealthComponent && !bIsDead)
    {
        AActor* KnockbackSource = DamageCauser;
        if (!KnockbackSource && EventInstigator)
        {
            KnockbackSource = EventInstigator->GetPawn();
        }

        ApplyKnockbackFrom(KnockbackSource);
        HealthComponent->ApplyDamage(ActualDamage);
    }

    return ActualDamage;
}

void APSVPlayerCharacter::ApplyKnockbackFrom(AActor* DamageCauser)
{
    if (!DamageCauser || KnockbackStrength <= 0.f)
    {
        return;
    }

    const FVector Offset = GetActorLocation() - DamageCauser->GetActorLocation();
    FVector KnockbackDirection(Offset.X, Offset.Y, 0.f);

    if (!KnockbackDirection.Normalize())
    {
        return;
    }

    const FVector LaunchVelocity = KnockbackDirection * KnockbackStrength;
    LaunchCharacter(LaunchVelocity, true, false);
}

void APSVPlayerCharacter::StartDeathRagdoll()
{
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
        MeshComp->SetAllBodiesSimulatePhysics(true);
        MeshComp->SetSimulatePhysics(true);
        MeshComp->WakeAllRigidBodies();
        MeshComp->SetAllBodiesPhysicsBlendWeight(1.f);
        MeshComp->bBlendPhysics = true;

        const FVector ImpulseDirection = GetDeathImpulseDirection();
        if (DeathImpulseStrength > 0.f && !ImpulseDirection.IsNearlyZero())
        {
            MeshComp->AddImpulse(ImpulseDirection * DeathImpulseStrength, NAME_None, true);
        }
    }
}

FVector APSVPlayerCharacter::GetDeathImpulseDirection() const
{
    FVector Direction = GetActorRightVector();
    if (Direction.IsNearlyZero())
    {
        Direction = FVector::RightVector;
    }

    Direction = Direction.GetSafeNormal();

    if (!FMath::IsNearlyZero(DeathImpulseUpwardScale))
    {
        Direction.Z += FMath::Clamp(DeathImpulseUpwardScale, -1.f, 1.f);
        Direction = Direction.GetSafeNormal();
    }

    return Direction;
}

void APSVPlayerCharacter::HandleHealthChanged(float CurrentHealthValue, float MaxHealthValue)
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (APSVHUD* PSVHUD = PlayerController->GetHUD<APSVHUD>())
        {
            PSVHUD->HandlePlayerHealthChanged(CurrentHealthValue, MaxHealthValue);
        }
    }
}

void APSVPlayerCharacter::HandleDeath()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    if (ExperienceComponent)
    {
        ExperienceComponent->ResetProgression();
    }

    if (AutoFireComponent)
    {
        AutoFireComponent->StopFiring();
    }

    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->DisableMovement();
    }

    if (UCapsuleComponent* Capsule = GetCapsuleComponent())
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    StartDeathRagdoll();

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        DisableInput(PlayerController);

        if (APSVHUD* PSVHUD = PlayerController->GetHUD<APSVHUD>())
        {
            PSVHUD->HandlePlayerDeath();
        }
    }
}

void APSVPlayerCharacter::HandlePersistentGoldChanged(int32 NewTotalGold)
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (APSVHUD* PSVHUD = PlayerController->GetHUD<APSVHUD>())
        {
            PSVHUD->HandlePlayerGoldChanged(NewTotalGold);
        }
    }
}
