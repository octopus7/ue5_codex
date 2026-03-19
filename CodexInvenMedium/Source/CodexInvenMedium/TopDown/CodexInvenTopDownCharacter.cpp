#include "CodexInvenTopDownCharacter.h"

#include "Camera/CameraComponent.h"
#include "../CodexInvenMediumGameInstance.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "../Input/CodexInvenMediumInputConfigDataAsset.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "CodexInvenTopDownPlayerController.h"

ACodexInvenTopDownCharacter::ACodexInvenTopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->MaxWalkSpeed = 650.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	GetMesh()->SetHiddenInGame(true);
	GetMesh()->SetVisibility(false);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1200.0f;
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	CubeVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeVisual"));
	CubeVisual->SetupAttachment(RootComponent);
	CubeVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CubeVisual->SetRelativeLocation(FVector(0.0f, 0.0f, -44.0f));
	CubeVisual->SetWorldScale3D(FVector(1.0f, 1.0f, 1.8f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		CubeVisual->SetStaticMesh(CubeMeshAsset.Object);
	}
}

void ACodexInvenTopDownCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ACodexInvenTopDownCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RotateTowardCursor();
}

void ACodexInvenTopDownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const UCodexInvenMediumGameInstance* TypedGameInstance = GetGameInstance<UCodexInvenMediumGameInstance>();
	const UCodexInvenMediumInputConfigDataAsset* InputConfig = TypedGameInstance != nullptr ? TypedGameInstance->DefaultInputConfig : nullptr;
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInput == nullptr || InputConfig == nullptr)
	{
		return;
	}

	if (InputConfig->MoveAction != nullptr)
	{
		EnhancedInput->BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ACodexInvenTopDownCharacter::Move);
		EnhancedInput->BindAction(InputConfig->MoveAction, ETriggerEvent::Completed, this, &ACodexInvenTopDownCharacter::Move);
	}

	if (InputConfig->LookAction != nullptr)
	{
		EnhancedInput->BindAction(InputConfig->LookAction, ETriggerEvent::Triggered, this, &ACodexInvenTopDownCharacter::Look);
		EnhancedInput->BindAction(InputConfig->LookAction, ETriggerEvent::Completed, this, &ACodexInvenTopDownCharacter::Look);
	}

	if (InputConfig->JumpAction != nullptr)
	{
		EnhancedInput->BindAction(InputConfig->JumpAction, ETriggerEvent::Started, this, &ACodexInvenTopDownCharacter::StartJump);
		EnhancedInput->BindAction(InputConfig->JumpAction, ETriggerEvent::Completed, this, &ACodexInvenTopDownCharacter::StopJumpingAction);
	}

	if (InputConfig->FireAction != nullptr)
	{
		EnhancedInput->BindAction(InputConfig->FireAction, ETriggerEvent::Started, this, &ACodexInvenTopDownCharacter::FirePressed);
	}
}

void ACodexInvenTopDownCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D InputVector = Value.Get<FVector2D>();
	if (Controller == nullptr || InputVector.IsNearlyZero())
	{
		return;
	}

	const FRotator CameraYaw(0.0f, CameraBoom != nullptr ? CameraBoom->GetComponentRotation().Yaw : GetActorRotation().Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(CameraYaw).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(CameraYaw).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InputVector.Y);
	AddMovementInput(RightDirection, InputVector.X);
}

void ACodexInvenTopDownCharacter::Look(const FInputActionValue& Value)
{
	LastLookInput = Value.Get<FVector2D>();
}

void ACodexInvenTopDownCharacter::StartJump()
{
	Jump();
}

void ACodexInvenTopDownCharacter::StopJumpingAction()
{
	StopJumping();
}

void ACodexInvenTopDownCharacter::FirePressed()
{
	UE_LOG(LogTemp, Log, TEXT("Fire input triggered on %s"), *GetName());
}

void ACodexInvenTopDownCharacter::RotateTowardCursor()
{
	ACodexInvenTopDownPlayerController* TopDownController = Cast<ACodexInvenTopDownPlayerController>(GetController());
	if (TopDownController == nullptr)
	{
		return;
	}

	FHitResult HitResult;
	if (!TopDownController->GetHitResultUnderCursor(ECC_Visibility, true, HitResult))
	{
		return;
	}

	const FVector TargetDirection = HitResult.ImpactPoint - GetActorLocation();
	RotateTowardWorldDirection(TargetDirection);
}

void ACodexInvenTopDownCharacter::RotateTowardWorldDirection(const FVector& WorldDirection)
{
	FVector FlatDirection = WorldDirection;
	FlatDirection.Z = 0.0f;
	if (FlatDirection.IsNearlyZero())
	{
		return;
	}

	SetActorRotation(FlatDirection.Rotation());
}
