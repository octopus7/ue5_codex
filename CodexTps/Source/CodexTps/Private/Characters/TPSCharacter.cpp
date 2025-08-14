// TPSCharacter.cpp

#include "Characters/TPSCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "UObject/ConstructorHelpers.h"

#include "Projectiles/TPSProjectile.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

ATPSCharacter::ATPSCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Do not rotate character with controller directly; orient to movement for TPS feel
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    if (MoveComp)
    {
        MoveComp->bOrientRotationToMovement = true; // Character rotates towards movement direction
        MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
        MoveComp->JumpZVelocity = 600.f;
        MoveComp->AirControl = 0.2f;
        MoveComp->bUseControllerDesiredRotation = false;
    }

    // Create camera boom (pulls in towards the character if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 350.0f; // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
    CameraBoom->bEnableCameraLag = true;
    CameraBoom->CameraLagSpeed = 10.0f;

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Add a simple cube placeholder as the visual representation
    PlaceholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
    PlaceholderMesh->SetupAttachment(GetCapsuleComponent());
    PlaceholderMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Capsule handles collision
    PlaceholderMesh->SetGenerateOverlapEvents(false);
    PlaceholderMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f)); // Pull down roughly to align with capsule
    PlaceholderMesh->SetRelativeScale3D(FVector(0.9f));

    // Try set default engine cube
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
    if (CubeMesh.Succeeded())
    {
        PlaceholderMesh->SetStaticMesh(CubeMesh.Object);
    }

    // Muzzle spawn point attached to character (tweak in editor as needed)
    MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
    MuzzlePoint->SetupAttachment(GetCapsuleComponent());
    MuzzlePoint->SetRelativeLocation(FVector(60.f, 0.f, 40.f));
}

void ATPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Add default mapping context to local player subsystem
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (DefaultMappingContext)
                {
                    Subsystem->AddMappingContext(DefaultMappingContext, /*Priority*/0);
                }
            }
        }
    }
}

void ATPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (IA_Move)
        {
            EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATPSCharacter::Input_Move);
        }

        if (IA_Look)
        {
            EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ATPSCharacter::Input_Look);
        }

        if (IA_Jump)
        {
            EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &ATPSCharacter::Input_JumpStarted);
            EIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ATPSCharacter::Input_JumpCompleted);
            EIC->BindAction(IA_Jump, ETriggerEvent::Canceled, this, &ATPSCharacter::Input_JumpCompleted);
        }

        if (IA_Fire)
        {
            EIC->BindAction(IA_Fire, ETriggerEvent::Started, this, &ATPSCharacter::Input_Fire);
        }
    }
}

void ATPSCharacter::Input_Move(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();
    if (!Controller || Axis.IsNearlyZero())
    {
        return;
    }

    // Get controller yaw rotation only
    const FRotator ControlRot = Controller->GetControlRotation();
    const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

    // Forward/Right vectors
    const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
    const FVector RightDir   = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDir, Axis.Y);
    AddMovementInput(RightDir, Axis.X);
}

void ATPSCharacter::Input_Look(const FInputActionValue& Value)
{
    const FVector2D LookAxis = Value.Get<FVector2D>();
    AddControllerYawInput(LookAxis.X * LookYawScale);
    AddControllerPitchInput(LookAxis.Y * LookPitchScale);
}

void ATPSCharacter::Input_JumpStarted(const FInputActionValue& /*Value*/)
{
    Jump();
}

void ATPSCharacter::Input_JumpCompleted(const FInputActionValue& /*Value*/)
{
    StopJumping();
}

void ATPSCharacter::Input_Fire(const FInputActionValue& /*Value*/)
{
    if (!ProjectileClass)
    {
        return;
    }
    // Determine spawn location from character-attached muzzle (fallback to actor front)
    const FVector CharForward = GetActorForwardVector();
    const FVector MuzzleLoc = (MuzzlePoint ? MuzzlePoint->GetComponentLocation()
                                           : GetActorLocation() + CharForward * MuzzleOffset);

    // Aim: use camera line trace to find target point, then shoot from muzzle to that point
    FVector AimPoint = MuzzleLoc + CharForward * FireTraceRange;
    if (FollowCamera)
    {
        const FVector CamLoc = FollowCamera->GetComponentLocation();
        const FVector CamDir = FollowCamera->GetComponentRotation().Vector();
        const FVector TraceEnd = CamLoc + CamDir * FireTraceRange;

        FHitResult Hit;
        FCollisionQueryParams Params(SCENE_QUERY_STAT(TPS_FireTrace), false, this);
        if (GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, TraceEnd, ECC_Visibility, Params))
        {
            AimPoint = Hit.ImpactPoint;
        }
        else
        {
            AimPoint = TraceEnd;
        }
    }

    const FVector ShootDir = (AimPoint - MuzzleLoc).GetSafeNormal();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    if (UWorld* World = GetWorld())
    {
        if (ATPSProjectile* Proj = World->SpawnActor<ATPSProjectile>(ProjectileClass, MuzzleLoc, ShootDir.Rotation(), SpawnParams))
        {
            Proj->FireInDirection(ShootDir);
        }
    }
}
