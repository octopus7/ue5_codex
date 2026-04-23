#include "MannyPosePreviewActor.h"

#include "Components/PoseableMeshComponent.h"
#include "Engine/SkeletalMesh.h"

namespace
{
    const FTransform* GetReferenceLocalBoneTransform(const UPoseableMeshComponent* PoseableMesh, const int32 BoneIndex)
    {
        if (!PoseableMesh)
        {
            return nullptr;
        }

        const USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(PoseableMesh->GetSkinnedAsset());
        if (!SkeletalMesh)
        {
            return nullptr;
        }

        const TArray<FTransform>& RefBonePose = SkeletalMesh->GetRefSkeleton().GetRefBonePose();
        return RefBonePose.IsValidIndex(BoneIndex) ? &RefBonePose[BoneIndex] : nullptr;
    }

    void ResetToReferencePose(UPoseableMeshComponent* PoseableMesh)
    {
        if (!PoseableMesh)
        {
            return;
        }

        const USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(PoseableMesh->GetSkinnedAsset());
        if (!SkeletalMesh)
        {
            return;
        }

        PoseableMesh->BoneSpaceTransforms = SkeletalMesh->GetRefSkeleton().GetRefBonePose();
        PoseableMesh->MarkRefreshTransformDirty();
    }

    void SetLocalBoneTransformFromReference(UPoseableMeshComponent* PoseableMesh, const FName BoneName, const FVector& LocationOffset, const FRotator& RotationOffset)
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

        const FTransform* ReferenceLocalBoneTransform = GetReferenceLocalBoneTransform(PoseableMesh, BoneIndex);
        if (!ReferenceLocalBoneTransform)
        {
            return;
        }

        const FQuat NewRotation = (FQuat(RotationOffset) * ReferenceLocalBoneTransform->GetRotation()).GetNormalized();
        const FVector NewLocation = ReferenceLocalBoneTransform->GetLocation() + LocationOffset;
        PoseableMesh->BoneSpaceTransforms[BoneIndex].SetLocation(NewLocation);
        PoseableMesh->BoneSpaceTransforms[BoneIndex].SetRotation(NewRotation);
        PoseableMesh->MarkRefreshTransformDirty();
    }

    void AddLocalBoneRotationOffset(UPoseableMeshComponent* PoseableMesh, const FName BoneName, const FRotator& RotationOffset)
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

        const FQuat NewRotation = (FQuat(RotationOffset) * PoseableMesh->BoneSpaceTransforms[BoneIndex].GetRotation()).GetNormalized();
        PoseableMesh->BoneSpaceTransforms[BoneIndex].SetRotation(NewRotation);
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
    ResetToReferencePose(PoseableMesh);
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
        SetLocalBoneTransformFromReference(PoseableMesh, Pair.Key, Pair.Value.Location, Pair.Value.Rotation);
    }
}

void AMannyPosePreviewActor::ApplyFingerOffsets(const FMannyPoseData& PoseData)
{
    for (const TPair<FName, FRotator>& Pair : PoseData.FingerOffsets)
    {
        AddLocalBoneRotationOffset(PoseableMesh, Pair.Key, Pair.Value);
    }
}

void AMannyPosePreviewActor::ApplyNamedRotations(const TMap<FName, FRotator>& Rotations)
{
    for (const TPair<FName, FRotator>& Pair : Rotations)
    {
        SetLocalBoneTransformFromReference(PoseableMesh, Pair.Key, FVector::ZeroVector, Pair.Value);
    }
}
