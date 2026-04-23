#pragma once

#include "CoreMinimal.h"

class USkeletalMesh;

namespace MannyPoseViewerExport
{
    struct FExportSummary
    {
        int32 BoneCount = 0;
        int32 PoseCount = 0;
    };

    bool ExportViewerDataset(
        const USkeletalMesh& SkeletalMesh,
        const FString& BodyPoseDirectory,
        const FString& OutputPath,
        FExportSummary& OutSummary,
        FString& OutError);
}
