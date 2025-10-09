#include "Characters/PSVPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"

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
    if (!Controller || !CameraBoom)
    {
        return;
    }

    const FVector2D MovementVector = Value.Get<FVector2D>();
    if (MovementVector.IsNearlyZero())
    {
        return;
    }

    const FRotator CameraYawOnly(0.f, CameraBoom->GetComponentRotation().Yaw, 0.f);
    const FVector ForwardDirection = FRotationMatrix(CameraYawOnly).GetUnitAxis(EAxis::X);
    const FVector RightDirection = FRotationMatrix(CameraYawOnly).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDirection, MovementVector.Y);
    AddMovementInput(RightDirection, MovementVector.X);
}
