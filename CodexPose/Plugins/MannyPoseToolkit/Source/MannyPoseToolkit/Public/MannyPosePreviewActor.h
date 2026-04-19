#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MannyPoseTypes.h"
#include "MannyPosePreviewActor.generated.h"

class UPoseableMeshComponent;

UCLASS(Blueprintable)
class MANNYPOSETOOLKIT_API AMannyPosePreviewActor : public AActor
{
    GENERATED_BODY()

public:
    AMannyPosePreviewActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pose")
    TObjectPtr<UPoseableMeshComponent> PoseableMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pose")
    FMannyPoseData CurrentPose;

    UFUNCTION(CallInEditor, BlueprintCallable, Category="Pose")
    void ApplyCurrentPose();

    UFUNCTION(BlueprintCallable, Category="Pose")
    void ApplyPoseData(const FMannyPoseData& PoseData);

    UFUNCTION(BlueprintCallable, Category="Pose")
    void ApplyHandPreset(const FMannyHandPresetData& PresetData, EMannyHandTargetSide TargetSide);

protected:
    virtual void OnConstruction(const FTransform& Transform) override;

private:
    void ApplyFkBoneRotations(const FMannyPoseData& PoseData);
    void ApplyFingerOffsets(const FMannyPoseData& PoseData);
    void ApplyNamedRotations(const TMap<FName, FRotator>& Rotations);
};
