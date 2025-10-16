#include "Characters/StaticMeshCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "UI/AmmoHealthWidget.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
AStaticMeshCharacter::AStaticMeshCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    CapsuleComponent->InitCapsuleSize(42.f, 96.f);
    RootComponent = CapsuleComponent;

    CharacterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CharacterMesh"));
    CharacterMesh->SetupAttachment(RootComponent);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    MaxAmmo = 30;
    MaxHealth = 100.f;
    AmmoCostPerFire = 1;
    HealthCostPerBreath = 5.f;

    CurrentAmmo = MaxAmmo;
    CurrentHealth = MaxHealth;

    StatusWidgetClass = nullptr;
    StatusWidget = nullptr;
    bHasWarnedMissingWidgetClass = false;

    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AStaticMeshCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentAmmo = FMath::Clamp(CurrentAmmo, 0, MaxAmmo);
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
            {
                if (DefaultMappingContext)
                {
                    InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
                }
            }
        }

        CreateStatusWidget();
    }

    BroadcastAmmoChanged();
    BroadcastHealthChanged();
}

void AStaticMeshCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    CreateStatusWidget();
}

void AStaticMeshCharacter::OnRep_Controller()
{
    Super::OnRep_Controller();
    CreateStatusWidget();
}

void AStaticMeshCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (FireAction)
        {
            EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, this, &AStaticMeshCharacter::HandleFireInput);
        }

        if (BreatheAction)
        {
            EnhancedInput->BindAction(BreatheAction, ETriggerEvent::Started, this, &AStaticMeshCharacter::HandleBreatheInput);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("StaticMeshCharacter %s: PlayerInputComponent is not an EnhancedInputComponent."), *GetName());
    }
}

void AStaticMeshCharacter::AddAmmo(int32 AmmoAmount)
{
    if (AmmoAmount <= 0)
    {
        return;
    }

    const int32 NewAmmo = FMath::Clamp(CurrentAmmo + AmmoAmount, 0, MaxAmmo);

    if (NewAmmo != CurrentAmmo)
    {
        CurrentAmmo = NewAmmo;
        BroadcastAmmoChanged();
    }
}

void AStaticMeshCharacter::AddHealth(float HealthAmount)
{
    if (!FMath::IsFinite(HealthAmount) || HealthAmount <= 0.f)
    {
        return;
    }

    const float NewHealth = FMath::Clamp(CurrentHealth + HealthAmount, 0.f, MaxHealth);

    if (!FMath::IsNearlyEqual(NewHealth, CurrentHealth))
    {
        CurrentHealth = NewHealth;
        BroadcastHealthChanged();
    }
}

void AStaticMeshCharacter::HandleFireInput(const FInputActionValue& Value)
{
    //if (Value.Get<bool>())
    {
        ConsumeAmmo(AmmoCostPerFire);
    }
}

void AStaticMeshCharacter::HandleBreatheInput(const FInputActionValue& Value)
{
    //if (Value.Get<bool>())
    {
        ConsumeHealth(HealthCostPerBreath);
    }
}

void AStaticMeshCharacter::ConsumeAmmo(int32 Amount)
{
    if (Amount <= 0 || CurrentAmmo <= 0)
    {
        if (CurrentAmmo <= 0)
        {
            UE_LOG(LogTemp, Verbose, TEXT("StaticMeshCharacter %s tried to fire without ammo."), *GetName());
        }
        return;
    }

    const int32 PreviousAmmo = CurrentAmmo;
    const int32 NewAmmo = FMath::Clamp(CurrentAmmo - Amount, 0, MaxAmmo);

    if (NewAmmo != CurrentAmmo)
    {
        CurrentAmmo = NewAmmo;

        if (GEngine && NewAmmo < PreviousAmmo)
        {
            const FString DebugMessage = FString::Printf(TEXT("Ammo decreased: %d -> %d"), PreviousAmmo, NewAmmo);
            GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Yellow, DebugMessage);
        }

        BroadcastAmmoChanged();
    }
}

void AStaticMeshCharacter::ConsumeHealth(float Amount)
{
    if (!FMath::IsFinite(Amount) || Amount <= 0.f || CurrentHealth <= 0.f)
    {
        if (CurrentHealth <= 0.f)
        {
            UE_LOG(LogTemp, Verbose, TEXT("StaticMeshCharacter %s tried to breathe without health remaining."), *GetName());
        }
        return;
    }

    const float PreviousHealth = CurrentHealth;
    const float NewHealth = FMath::Clamp(CurrentHealth - Amount, 0.f, MaxHealth);

    if (!FMath::IsNearlyEqual(NewHealth, CurrentHealth))
    {
        CurrentHealth = NewHealth;

        if (GEngine && NewHealth < PreviousHealth)
        {
            const FString DebugMessage = FString::Printf(TEXT("Health decreased: %.0f -> %.0f"), PreviousHealth, NewHealth);
            GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Red, DebugMessage);
        }

        BroadcastHealthChanged();
    }
}

void AStaticMeshCharacter::CreateStatusWidget()
{
    if (StatusWidget)
    {
        return;
    }

    APlayerController* PlayerController = Cast<APlayerController>(GetController());
    if (!PlayerController || !PlayerController->IsLocalController())
    {
        return;
    }

    if (!StatusWidgetClass)
    {
        if (!bHasWarnedMissingWidgetClass)
        {
            bHasWarnedMissingWidgetClass = true;
            UE_LOG(LogTemp, Warning, TEXT("StaticMeshCharacter %s has no StatusWidgetClass set; assign a widget blueprint derived from UAmmoHealthWidget."), *GetName());
        }
        return;
    }

    if (UAmmoHealthWidget* NewWidget = CreateWidget<UAmmoHealthWidget>(PlayerController, StatusWidgetClass))
    {
        bHasWarnedMissingWidgetClass = false;
        StatusWidget = NewWidget;
        StatusWidget->AddToPlayerScreen();
        StatusWidget->SetAmmo(CurrentAmmo, MaxAmmo);
        StatusWidget->SetHealth(CurrentHealth, MaxHealth);
    }
}

void AStaticMeshCharacter::BroadcastAmmoChanged()
{
    OnAmmoChanged.Broadcast(CurrentAmmo, MaxAmmo);

    if (!StatusWidget)
    {
        CreateStatusWidget();
    }

    if (StatusWidget)
    {
        StatusWidget->SetAmmo(CurrentAmmo, MaxAmmo);
    }
}

void AStaticMeshCharacter::BroadcastHealthChanged()
{
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

    if (!StatusWidget)
    {
        CreateStatusWidget();
    }

    if (StatusWidget)
    {
        StatusWidget->SetHealth(CurrentHealth, MaxHealth);
    }
}
