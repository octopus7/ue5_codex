#include "MannyPoseExportCommandlet.h"

#include "Engine/SkeletalMesh.h"
#include "HAL/FileManager.h"
#include "Interfaces/IPluginManager.h"
#include "MannyPoseViewerExport.h"
#include "Misc/MessageDialog.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"

UMannyPoseExportCommandlet::UMannyPoseExportCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
    ShowErrorCount = true;
}

int32 UMannyPoseExportCommandlet::Main(const FString& Params)
{
    FString MeshPath;
    FString OutputPath;
    FString PoseDirectory;

    FParse::Value(*Params, TEXT("Mesh="), MeshPath);
    FParse::Value(*Params, TEXT("Out="), OutputPath);
    FParse::Value(*Params, TEXT("PoseDir="), PoseDirectory);

    if (MeshPath.IsEmpty())
    {
        MeshPath = TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple");
    }

    if (OutputPath.IsEmpty())
    {
        OutputPath = FPaths::ProjectSavedDir() / TEXT("MannyPoseToolkit/Exports/manny_pose_viewer_dataset.json");
    }

    if (PoseDirectory.IsEmpty())
    {
        const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("MannyPoseToolkit"));
        if (!Plugin.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to resolve MannyPoseToolkit plugin directory."));
            return 1;
        }

        PoseDirectory = Plugin->GetBaseDir() / TEXT("Content/Poses/Bodies");
    }

    UE_LOG(LogTemp, Display, TEXT("Loading skeletal mesh: %s"), *MeshPath);
    USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath);
    if (!SkeletalMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load skeletal mesh at %s"), *MeshPath);
        return 1;
    }

    MannyPoseViewerExport::FExportSummary ExportSummary;
    FString Error;
    if (!MannyPoseViewerExport::ExportViewerDataset(*SkeletalMesh, PoseDirectory, OutputPath, ExportSummary, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("Export failed: %s"), *Error);
        return 1;
    }

    UE_LOG(
        LogTemp,
        Display,
        TEXT("Exported %d bones and %d poses to %s"),
        ExportSummary.BoneCount,
        ExportSummary.PoseCount,
        *OutputPath);

    return 0;
}
