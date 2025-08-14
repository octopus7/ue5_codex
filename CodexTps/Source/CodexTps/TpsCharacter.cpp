#include "TpsCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

ATpsCharacter::ATpsCharacter()
{
    // Collision size similar to default character
    GetCapsuleComponent()->InitCapsuleSize(42.f, 88.0f);

    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;

    // Character movement config for TPS
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->bOrientRotationToMovement = true;
    MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
    MoveComp->bConstrainToPlane = false;
    MoveComp->bUseControllerDesiredRotation = false;

    // Visual: Blue cube as placeholder
    CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
    CubeMesh->SetupAttachment(RootComponent);
    CubeMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f)); // center cube within capsule
    CubeMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
    CubeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CubeMesh->SetGenerateOverlapEvents(false);

    // Load engine cube mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMeshAsset.Succeeded())
    {
        CubeMesh->SetStaticMesh(CubeMeshAsset.Object);
    }

    // Try to tint cube blue using the basic shape material (if available)
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BasicMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (BasicMat.Succeeded())
    {
        UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BasicMat.Object, this);
        if (MID)
        {
            MID->SetVectorParameterValue(FName("Color"), FLinearColor::Blue);
            CubeMesh->SetMaterial(0, MID);
        }
    }

    // Camera boom
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 15.f;

    // Follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    LookSensitivityYaw = 1.0f;
    LookSensitivityPitch = 1.0f;
}

void ATpsCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Create Enhanced Input actions and mapping context at runtime
    IA_Move = NewObject<UInputAction>(this, TEXT("IA_Move"));
    IA_Move->ValueType = EInputActionValueType::Axis2D;

    IA_Look = NewObject<UInputAction>(this, TEXT("IA_Look"));
    IA_Look->ValueType = EInputActionValueType::Axis2D;

    IA_Jump = NewObject<UInputAction>(this, TEXT("IA_Jump"));
    IA_Jump->ValueType = EInputActionValueType::Boolean;

    IMC_TPS = NewObject<UInputMappingContext>(this, TEXT("IMC_TPS"));

    if (IMC_TPS)
    {
        // Movement as Axis2D using WASD
        // W -> +X
        IMC_TPS->MapKey(IA_Move, EKeys::W);
        // S -> -X
        {
            FEnhancedActionKeyMapping& Map = IMC_TPS->MapKey(IA_Move, EKeys::S);
            UInputModifierScalar* Scalar = NewObject<UInputModifierScalar>(IMC_TPS);
            Scalar->Scalar = FVector(-1.f, 1.f, 1.f);
            Map.Modifiers.Add(Scalar);
        }
        // D -> +Y (swizzle X->Y)
        {
            FEnhancedActionKeyMapping& Map = IMC_TPS->MapKey(IA_Move, EKeys::D);
            UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(IMC_TPS);
            Swizzle->Order = EInputAxisSwizzle::YXZ; // move 1D X into Y
            Map.Modifiers.Add(Swizzle);
        }
        // A -> -Y (swizzle X->Y, then scale -1)
        {
            FEnhancedActionKeyMapping& Map = IMC_TPS->MapKey(IA_Move, EKeys::A);
            UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(IMC_TPS);
            Swizzle->Order = EInputAxisSwizzle::YXZ;
            Map.Modifiers.Add(Swizzle);
            UInputModifierScalar* Scale = NewObject<UInputModifierScalar>(IMC_TPS);
            Scale->Scalar = FVector(1.f, -1.f, 1.f);
            Map.Modifiers.Add(Scale);
        }

        // Look as Axis2D using mouse
        // MouseX -> +X
        IMC_TPS->MapKey(IA_Look, EKeys::MouseX);
        // MouseY -> +Y (swizzle X->Y)
        {
            FEnhancedActionKeyMapping& Map = IMC_TPS->MapKey(IA_Look, EKeys::MouseY);
            UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(IMC_TPS);
            Swizzle->Order = EInputAxisSwizzle::YXZ;
            Map.Modifiers.Add(Swizzle);
        }

        // Jump
        IMC_TPS->MapKey(IA_Jump, EKeys::SpaceBar);
    }

    // Register mapping context to local player subsystem
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsys =
                    ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
            {
                Subsys->AddMappingContext(IMC_TPS, 0);
            }
        }
    }
}

void ATpsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATpsCharacter::Move2D);
        EnhancedIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ATpsCharacter::Look2D);
        EnhancedIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &ATpsCharacter::JumpStarted);
        EnhancedIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ATpsCharacter::JumpCompleted);
    }
}

void ATpsCharacter::Move2D(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (Controller)
    {
        if (!FMath::IsNearlyZero(Axis.X))
        {
            const FRotator ControlRot = Controller->GetControlRotation();
            const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
            const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
            AddMovementInput(Forward, Axis.X);
        }
        if (!FMath::IsNearlyZero(Axis.Y))
        {
            const FRotator ControlRot = Controller->GetControlRotation();
            const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
            const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
            AddMovementInput(Right, Axis.Y);
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
