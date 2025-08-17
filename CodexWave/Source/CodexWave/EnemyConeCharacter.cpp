#include "EnemyConeCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemyConeCharacter::AEnemyConeCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // Add a visual static mesh (cone) attached to capsule
    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(GetCapsuleComponent());
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
    if (ConeMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(ConeMesh.Object);
        // Flip upside-down: pitch 180 so tip points down to ground
        VisualMesh->SetRelativeRotation(FRotator(180.f, 0.f, 0.f));
        VisualMesh->SetRelativeLocation(FVector(0.f, 0.f, -44.f)); // pull mesh down a bit to match capsule height
        VisualMesh->SetRelativeScale3D(FVector(1.0f));
    }
}

