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
#include "InputTriggers.h"
#include "InputCoreTypes.h"
#include "WaveProjectile.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

ACubePlayerPawn::ACubePlayerPawn()
{
    PrimaryActorTick.bCanEverTick = true;

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
    SpringArm->TargetArmLength = CameraArmLength;
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
    FloatingMovement->UpdatedComponent = MeshComponent;

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    // Enhanced Input assets are created at runtime (BeginPlay/ensure), not in constructor
    // Default projectile class
    ProjectileClass = AWaveProjectile::StaticClass();
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
                // Mapping is now added in BeginPlay/Setup when assets exist
            }
        }
    }
}

void ACubePlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    EnsureInputAssets();

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

        if (FireAction)
        {
            // Use Triggered with an explicit Pressed trigger on the action
            EIC->BindAction(FireAction, ETriggerEvent::Triggered, this, &ACubePlayerPawn::Fire);
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, TEXT("Bind FireAction: Triggered -> Fire()"));
            }
        }

        if (!bMappingApplied)
        {
            if (APlayerController* PC = Cast<APlayerController>(GetController()))
            {
                if (ULocalPlayer* LP = PC->GetLocalPlayer())
                {
                    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
                    {
                        if (DefaultMappingContext)
                        {
                            Subsystem->AddMappingContext(DefaultMappingContext, 0);
                            bMappingApplied = true;
                        }
                    }
                }
            }
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

void ACubePlayerPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bRotateToMovement)
    {
        return;
    }

    FVector V = GetVelocity();
    V.Z = 0.f;
    if (V.SizeSquared() > KINDA_SMALL_NUMBER)
    {
        const FRotator Target = V.Rotation();
        const FRotator Current = GetActorRotation();
        const FRotator NewRot = FMath::RInterpTo(Current, FRotator(0.f, Target.Yaw, 0.f), DeltaSeconds, RotationInterpSpeed);
        SetActorRotation(NewRot);
    }
}

void ACubePlayerPawn::BeginPlay()
{
    Super::BeginPlay();

    EnsureInputAssets();

    // Try adding mapping context here too (covers cases before SetupPlayerInputComponent binds)
    if (!bMappingApplied)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            if (ULocalPlayer* LP = PC->GetLocalPlayer())
            {
                if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
                {
                    if (DefaultMappingContext)
                    {
                        Subsystem->AddMappingContext(DefaultMappingContext, 0);
                        bMappingApplied = true;
                        if (GEngine)
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, TEXT("EnhancedInput: MappingContext added"));
                        }
                    }
                }
            }
        }
    }
}

void ACubePlayerPawn::EnsureInputAssets()
{
    if (!DefaultMappingContext)
    {
        DefaultMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Default"));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Silver, TEXT("Created Default MappingContext"));
        }
    }

    if (!MoveForwardAction)
    {
        MoveForwardAction = NewObject<UInputAction>(this, TEXT("IA_MoveForward"));
        MoveForwardAction->ValueType = EInputActionValueType::Axis1D;
        // Bind W/S
        if (DefaultMappingContext)
        {
            DefaultMappingContext->MapKey(MoveForwardAction, EKeys::W);
            FEnhancedActionKeyMapping& SMap = DefaultMappingContext->MapKey(MoveForwardAction, EKeys::S);
            if (UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(DefaultMappingContext))
            {
                Negate->bX = true; Negate->bY = false; Negate->bZ = false;
                SMap.Modifiers.Add(Negate);
            }
        }
    }

    if (!MoveRightAction)
    {
        MoveRightAction = NewObject<UInputAction>(this, TEXT("IA_MoveRight"));
        MoveRightAction->ValueType = EInputActionValueType::Axis1D;
        // Bind D/A
        if (DefaultMappingContext)
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

    if (!FireAction)
    {
        FireAction = NewObject<UInputAction>(this, TEXT("IA_Fire"));
        FireAction->ValueType = EInputActionValueType::Boolean;
        if (UInputTriggerPressed* Pressed = NewObject<UInputTriggerPressed>(FireAction))
        {
            FireAction->Triggers.Add(Pressed);
        }
        if (DefaultMappingContext)
        {
            DefaultMappingContext->MapKey(FireAction, EKeys::SpaceBar);
            DefaultMappingContext->MapKey(FireAction, EKeys::LeftMouseButton);
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Silver, TEXT("Created FireAction + TriggerPressed + mapped Space/LMB"));
            }
        }
    }
}

void ACubePlayerPawn::Fire(const FInputActionValue& Value)
{
    const bool bPressed = Value.Get<bool>();
    if (GEngine)
    {
        FString Msg = FString::Printf(TEXT("Fire() Triggered, pressed=%s"), bPressed ? TEXT("true") : TEXT("false"));
        GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, Msg);
    }
    // For Triggered event with Pressed trigger, we treat this as pressed regardless of Value

    if (!ProjectileClass)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("No ProjectileClass set"));
        }
        return;
    }

    FVector Dir = GetVelocity();
    Dir.Z = 0.f;
    if (Dir.SizeSquared() < KINDA_SMALL_NUMBER)
    {
        Dir = GetActorForwardVector();
        Dir.Z = 0.f;
    }
    if (!Dir.Normalize())
    {
        Dir = FVector::ForwardVector;
    }

    const FVector SpawnLoc = GetActorLocation() + Dir * ProjectileSpawnOffset;
    const FRotator SpawnRot = Dir.Rotation();

    if (UWorld* WorldForDebug = GetWorld())
    {
        DrawDebugSphere(WorldForDebug, SpawnLoc, 12.f, 16, FColor::Cyan, false, 1.0f, 0, 1.5f);
    }

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = Cast<APawn>(this);
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    if (UWorld* World = GetWorld())
    {
        if (AWaveProjectile* P = World->SpawnActor<AWaveProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params))
        {
            P->InitVelocity(Dir);
        }
    }
}
