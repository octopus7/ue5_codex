#include "CubePlayerPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "UObject/ConstructorHelpers.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputCoreTypes.h"

ACubePlayerPawn::ACubePlayerPawn()
{
    PrimaryActorTick.bCanEverTick = false;

    // Root: Cube mesh
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(MeshComponent);
    MeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
    MeshComponent->SetSimulatePhysics(false);

    // Load engine cube mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        MeshComponent->SetStaticMesh(CubeMesh.Object);
        MeshComponent->SetWorldScale3D(FVector(1.0f));
    }

    // Top-down spring arm + camera
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(MeshComponent);
    SpringArm->TargetArmLength = 800.f;
    SpringArm->bDoCollisionTest = false;
    SpringArm->SetUsingAbsoluteRotation(true);
    SpringArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritRoll = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    // Simple floating movement component for WASD translation
    FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
    FloatingMovement->MaxSpeed = 600.f;

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    // Create Enhanced Input assets in-memory
    DefaultMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Default"));

    MoveForwardAction = NewObject<UInputAction>(this, TEXT("IA_MoveForward"));
    if (MoveForwardAction)
    {
        MoveForwardAction->ValueType = EInputActionValueType::Axis1D;
    }

    MoveRightAction = NewObject<UInputAction>(this, TEXT("IA_MoveRight"));
    if (MoveRightAction)
    {
        MoveRightAction->ValueType = EInputActionValueType::Axis1D;
    }

    if (DefaultMappingContext)
    {
        // Forward/Back: W (+1), S (-1)
        if (MoveForwardAction)
        {
            DefaultMappingContext->MapKey(MoveForwardAction, EKeys::W);

            FEnhancedActionKeyMapping& SMap = DefaultMappingContext->MapKey(MoveForwardAction, EKeys::S);
            if (UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext))
            {
                // Negate 1D value (X axis of the action value)
                Negate->bX = true; Negate->bY = false; Negate->bZ = false;
                SMap.Modifiers.Add(Negate);
            }
        }

        // Right/Left: D (+1), A (-1)
        if (MoveRightAction)
        {
            DefaultMappingContext->MapKey(MoveRightAction, EKeys::D);

            FEnhancedActionKeyMapping& AMap = DefaultMappingContext->MapKey(MoveRightAction, EKeys::A);
            if (UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext))
            {
                Negate->bX = true; Negate->bY = false; Negate->bZ = false;
                AMap.Modifiers.Add(Negate);
            }
        }
    }
}

void ACubePlayerPawn::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // Add mapping context to the local player's Enhanced Input subsystem
    if (APlayerController* PC = Cast<APlayerController>(NewController))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
            {
                if (DefaultMappingContext)
                {
                    Subsystem->AddMappingContext(DefaultMappingContext, /*Priority*/0);
                }
            }
        }
    }
}

void ACubePlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveForwardAction)
        {
            EIC->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &ACubePlayerPawn::MoveForward);
            EIC->BindAction(MoveForwardAction, ETriggerEvent::Completed, this, &ACubePlayerPawn::MoveForward);
        }

        if (MoveRightAction)
        {
            EIC->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ACubePlayerPawn::MoveRight);
            EIC->BindAction(MoveRightAction, ETriggerEvent::Completed, this, &ACubePlayerPawn::MoveRight);
        }
    }
}

void ACubePlayerPawn::MoveForward(const FInputActionValue& Value)
{
    const float Axis = Value.Get<float>();
    AddMovementInput(FVector::ForwardVector, Axis);
}

void ACubePlayerPawn::MoveRight(const FInputActionValue& Value)
{
    const float Axis = Value.Get<float>();
    AddMovementInput(FVector::RightVector, Axis);
}
