#include "TpsCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"

ATpsCharacter::ATpsCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(42.f, 88.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->bOrientRotationToMovement = true;
    MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
    MoveComp->bUseControllerDesiredRotation = false;

    CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
    CubeMesh->SetupAttachment(RootComponent);
    CubeMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
    CubeMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
    CubeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CubeMesh->SetGenerateOverlapEvents(false);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMeshAsset.Succeeded())
    {
        CubeMesh->SetStaticMesh(CubeMeshAsset.Object);
    }

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 15.f;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    LookSensitivityYaw = 1.0f;
    LookSensitivityPitch = 1.0f;
}

void ATpsCharacter::BeginPlay()
{
    Super::BeginPlay();
    SetupEnhancedInput();
}

void ATpsCharacter::SetupEnhancedInput()
{
    if (IMC_TPS)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            if (ULocalPlayer* LP = PC->GetLocalPlayer())
            {
            if (UEnhancedInputLocalPlayerSubsystem* Subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
            {
                Subsys->AddMappingContext(IMC_TPS, 0);
                UE_LOG(LogTemp, Log, TEXT("EnhancedInput: Added MappingContext %s to LocalPlayer"), *GetNameSafe(IMC_TPS));
                if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
                        FString::Printf(TEXT("IMC Added: %s"), *GetNameSafe(IMC_TPS)));
                }
            }
            }
        }
    }
}

void ATpsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (IA_Move) EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATpsCharacter::Move2D);
        if (IA_Look) EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ATpsCharacter::Look2D);
        if (IA_Jump)
        {
            EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &ATpsCharacter::JumpStarted);
            EIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ATpsCharacter::JumpCompleted);
        }
    }
}

void ATpsCharacter::Move2D(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Controller)
    {
        const FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
        if (!FMath::IsNearlyZero(Axis.Y))
        {
            const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
            AddMovementInput(Forward, Axis.Y);
        }
        if (!FMath::IsNearlyZero(Axis.X))
        {
            const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
            AddMovementInput(Right, Axis.X);
        }
    }
}

void ATpsCharacter::Look2D(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (!FMath::IsNearlyZero(Axis.X))
    {
        AddControllerYawInput(Axis.X * LookSensitivityYaw);
    }
    if (!FMath::IsNearlyZero(Axis.Y))
    {
        AddControllerPitchInput(Axis.Y * LookSensitivityPitch);
    }
}

void ATpsCharacter::JumpStarted(const FInputActionValue& /*Value*/)
{
    Jump();
}

void ATpsCharacter::JumpCompleted(const FInputActionValue& /*Value*/)
{
    StopJumping();
}
