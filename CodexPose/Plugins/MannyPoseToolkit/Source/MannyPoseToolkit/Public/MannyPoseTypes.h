#pragma once

#include "CoreMinimal.h"
#include "MannyPoseTypes.generated.h"

UENUM(BlueprintType)
enum class EMannyHandTargetSide : uint8
{
    Left,
    Right,
    BothSymmetric
};

USTRUCT(BlueprintType)
struct FMannyPoseVector
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FVector Value = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FMannyPoseControlTransform
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FVector Location = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FRotator Rotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FVector Scale = FVector(1.f, 1.f, 1.f);
};

USTRUCT(BlueprintType)
struct FMannyPoseBoneRotation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FRotator Rotation = FRotator::ZeroRotator;
};

USTRUCT(BlueprintType)
struct FMannyFingerPresetOverrides
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    TMap<FName, FRotator> Rotations;
};

USTRUCT(BlueprintType)
struct FMannyHandPresetData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FString Version = TEXT("1.0");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FString Skeleton = TEXT("UE5_Manny");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FString Name = TEXT("UnnamedHandPreset");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    TMap<FName, FRotator> LeftHandRotations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    TMap<FName, FRotator> RightHandRotations;
};

USTRUCT(BlueprintType)
struct FMannyPoseData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FString Version = TEXT("1.0");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FString Skeleton = TEXT("UE5_Manny");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FString ReferencePose = TEXT("A_Pose");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FString Name = TEXT("Unnamed");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    TMap<FName, FMannyPoseControlTransform> IKControls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    TMap<FName, FMannyPoseBoneRotation> FKBones;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FString LeftHandPreset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FString RightHandPreset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    TMap<FName, FRotator> FingerOffsets;
};
