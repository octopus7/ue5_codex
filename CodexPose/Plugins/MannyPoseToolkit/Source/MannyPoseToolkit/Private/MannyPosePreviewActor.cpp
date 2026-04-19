#include "MannyPosePreviewActor.h"

#include "Components/PoseableMeshComponent.h"

namespace
{
    void SetLocalBoneRotation(UPoseableMeshComponent* PoseableMesh, const FName BoneName, const FRotator& Rotation)
    {
        if (!PoseableMesh || !PoseableMesh->RequiredBones.IsValid())
        {
            return;
        }

        const int32 BoneIndex = PoseableMesh->GetBoneIndex(BoneName);
        if (!PoseableMesh->BoneSpaceTransforms.IsValidIndex(BoneIndex))
        {
            return;
        }

        PoseableMesh->BoneSpaceTransforms[BoneIndex].SetRotation(FQuat(Rotation));
        PoseableMesh->MarkRefreshTransformDirty();
    }
}

AMannyPosePreviewActor::AMannyPosePreviewActor()
{
    PrimaryActorTick.bCanEverTick = false;

    PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PoseableMesh"));
    SetRootComponent(PoseableMesh);
}

void AMannyPosePreviewActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    ApplyCurrentPose();
}

void AMannyPosePreviewActor::ApplyCurrentPose()
{
    ApplyPoseData(CurrentPose);
}

void AMannyPosePreviewActor::ApplyPoseData(const FMannyPoseData& PoseData)
{
    if (!PoseableMesh)
    {
        return;
    }

    CurrentPose = PoseData;
    ApplyFkBoneRotations(PoseData);
    ApplyFingerOffsets(PoseData);

    PoseableMesh->RefreshBoneTransforms();
    PoseableMesh->MarkRenderStateDirty();
}

void AMannyPosePreviewActor::ApplyHandPreset(const FMannyHandPresetData& PresetData, EMannyHandTargetSide TargetSide)
{
    if (!PoseableMesh)
    {
        return;
    }

    switch (TargetSide)
    {
        case EMannyHandTargetSide::Left:
            ApplyNamedRotations(PresetData.LeftHandRotations);
            CurrentPose.LeftHandPreset = PresetData.Name;
            break;
        case EMannyHandTargetSide::Right:
            ApplyNamedRotations(PresetData.RightHandRotations);
            CurrentPose.RightHandPreset = PresetData.Name;
            break;
        case EMannyHandTargetSide::BothSymmetric:
            ApplyNamedRotations(PresetData.LeftHandRotations);
            ApplyNamedRotations(PresetData.RightHandRotations);
            CurrentPose.LeftHandPreset = PresetData.Name;
            CurrentPose.RightHandPreset = PresetData.Name;
            break;
        default:
            break;
    }

    PoseableMesh->RefreshBoneTransforms();
    PoseableMesh->MarkRenderStateDirty();
}

void AMannyPosePreviewActor::ApplyFkBoneRotations(const FMannyPoseData& PoseData)
{
    for (const TPair<FName, FMannyPoseBoneRotation>& Pair : PoseData.FKBones)
    {
        SetLocalBoneRotation(PoseableMesh, Pair.Key, Pair.Value.Rotation);
    }
}

void AMannyPosePreviewActor::ApplyFingerOffsets(const FMannyPoseData& PoseData)
{
    for (const TPair<FName, FRotator>& Pair : PoseData.FingerOffsets)
    {
        SetLocalBoneRotation(PoseableMesh, Pair.Key, Pair.Value);
    }
}

void AMannyPosePreviewActor::ApplyNamedRotations(const TMap<FName, FRotator>& Rotations)
{
    for (const TPair<FName, FRotator>& Pair : Rotations)
    {
        SetLocalBoneRotation(PoseableMesh, Pair.Key, Pair.Value);
    }
}
