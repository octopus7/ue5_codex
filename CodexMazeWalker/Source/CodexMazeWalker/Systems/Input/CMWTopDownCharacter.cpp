#include "Systems/Input/CMWTopDownCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Systems/Combat/CMWCombatComponent.h"
#include "Systems/Game/CMWGameDataAsset.h"
#include "Systems/Game/CMWGameInstance.h"

ACMWTopDownCharacter::ACMWTopDownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 1200.0f;
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCamera->bUsePawnControlRotation = false;

	CombatComponent = CreateDefaultSubobject<UCMWCombatComponent>(TEXT("CombatComponent"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector::UpVector);
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
}

void ACMWTopDownCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ACMWTopDownCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	FaceAimLocation();
}

void ACMWTopDownCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const UCMWGameInstance* GameInstance = UCMWGameInstance::Get(this);
	const UCMWGameDataAsset* GameData = GameInstance ? GameInstance->GetGameData() : nullptr;
	if (!GameData)
	{
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (GameData->MoveAction)
		{
			EnhancedInputComponent->BindAction(GameData->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::HandleMove);
		}

		if (GameData->AttackAction)
		{
			EnhancedInputComponent->BindAction(GameData->AttackAction, ETriggerEvent::Started, this, &ThisClass::HandleAttack);
		}

		if (GameData->ToggleAttackModeAction)
		{
			EnhancedInputComponent->BindAction(GameData->ToggleAttackModeAction, ETriggerEvent::Started, this, &ThisClass::HandleToggleAttackMode);
		}
	}
}

void ACMWTopDownCharacter::SetAimWorldLocation(const FVector& NewAimWorldLocation)
{
	AimWorldLocation = NewAimWorldLocation;
	bHasAimWorldLocation = true;
}

void ACMWTopDownCharacter::HandleMove(const FInputActionValue& InputActionValue)
{
	const FVector2D MoveVector = InputActionValue.Get<FVector2D>();
	if (MoveVector.IsNearlyZero())
	{
		return;
	}

	AddMovementInput(FVector::ForwardVector, MoveVector.Y);
	AddMovementInput(FVector::RightVector, MoveVector.X);
}

void ACMWTopDownCharacter::HandleAttack()
{
	if (!CombatComponent || !bHasAimWorldLocation)
	{
		return;
	}

	CombatComponent->PerformAttack(AimWorldLocation);
}

void ACMWTopDownCharacter::HandleToggleAttackMode()
{
	if (CombatComponent)
	{
		CombatComponent->ToggleAttackMode();
	}
}

void ACMWTopDownCharacter::FaceAimLocation()
{
	if (!bHasAimWorldLocation)
	{
		return;
	}

	FVector FacingDirection = AimWorldLocation - GetActorLocation();
	FacingDirection.Z = 0.0f;
	if (FacingDirection.IsNearlyZero())
	{
		return;
	}

	SetActorRotation(FacingDirection.Rotation());
}
