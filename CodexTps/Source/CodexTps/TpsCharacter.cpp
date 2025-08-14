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
    IA_MoveForward = NewObject<UInputAction>(this, TEXT("IA_MoveForward"));
    IA_MoveForward->ValueType = EInputActionValueType::Axis1D;

    IA_MoveRight = NewObject<UInputAction>(this, TEXT("IA_MoveRight"));
    IA_MoveRight->ValueType = EInputActionValueType::Axis1D;

    IA_LookYaw = NewObject<UInputAction>(this, TEXT("IA_LookYaw"));
    IA_LookYaw->ValueType = EInputActionValueType::Axis1D;

    IA_LookPitch = NewObject<UInputAction>(this, TEXT("IA_LookPitch"));
    IA_LookPitch->ValueType = EInputActionValueType::Axis1D;

    IA_Jump = NewObject<UInputAction>(this, TEXT("IA_Jump"));
    IA_Jump->ValueType = EInputActionValueType::Boolean;

    IMC_TPS = NewObject<UInputMappingContext>(this, TEXT("IMC_TPS"));

    if (IMC_TPS)
    {
        // Movement WASD
        IMC_TPS->MapKey(IA_MoveForward, EKeys::W);
        {
            // S key with negative scale
            FEnhancedActionKeyMapping& Map = IMC_TPS->MapKey(IA_MoveForward, EKeys::S);
            UInputModifierScale* Scale = NewObject<UInputModifierScale>(IMC_TPS);
            Scale->Scalar = FVector( -1.f, 0.f, 0.f );
            Map.Modifiers.Add(Scale);
        }

        IMC_TPS->MapKey(IA_MoveRight, EKeys::D);
        {
            FEnhancedActionKeyMapping& Map = IMC_TPS->MapKey(IA_MoveRight, EKeys::A);
            UInputModifierScale* Scale = NewObject<UInputModifierScale>(IMC_TPS);
            Scale->Scalar = FVector( -1.f, 0.f, 0.f );
            Map.Modifiers.Add(Scale);
        }

        // Mouse look
        IMC_TPS->MapKey(IA_LookYaw, EKeys::MouseX);

        // Invert Y for typical camera if desired; here not inverted
        IMC_TPS->MapKey(IA_LookPitch, EKeys::MouseY);

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
        EnhancedIC->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &ATpsCharacter::MoveForward);
        EnhancedIC->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &ATpsCharacter::MoveRight);
        EnhancedIC->BindAction(IA_LookYaw, ETriggerEvent::Triggered, this, &ATpsCharacter::LookYaw);
        EnhancedIC->BindAction(IA_LookPitch, ETriggerEvent::Triggered, this, &ATpsCharacter::LookPitch);
        EnhancedIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &ATpsCharacter::JumpStarted);
        EnhancedIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ATpsCharacter::JumpCompleted);
    }
}

void ATpsCharacter::MoveForward(const FInputActionValue& Value)
{
    const float Axis = Value.Get<float>();
    if (Controller && Axis != 0.f)
    {
        const FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
        const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Axis);
    }
}

void ATpsCharacter::MoveRight(const FInputActionValue& Value)
{
    const float Axis = Value.Get<float>();
    if (Controller && Axis != 0.f)
    {
        const FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);
        const FVector Direction = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Axis);
    }
}

void ATpsCharacter::LookYaw(const FInputActionValue& Value)
{
    const float Axis = Value.Get<float>();
    AddControllerYawInput(Axis * LookSensitivityYaw);
}

void ATpsCharacter::LookPitch(const FInputActionValue& Value)
{
    const float Axis = Value.Get<float>();
    AddControllerPitchInput(Axis * LookSensitivityPitch);
}

void ATpsCharacter::JumpStarted(const FInputActionValue& /*Value*/)
{
    Jump();
}

void ATpsCharacter::JumpCompleted(const FInputActionValue& /*Value*/)
{
    StopJumping();
}

