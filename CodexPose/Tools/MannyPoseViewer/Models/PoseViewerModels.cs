using System.Text.Json.Serialization;

namespace MannyPoseViewer.Models;

public sealed class PoseViewerDataset
{
    [JsonPropertyName("version")]
    public string Version { get; set; } = string.Empty;

    [JsonPropertyName("datasetType")]
    public string DatasetType { get; set; } = string.Empty;

    [JsonPropertyName("skeleton")]
    public SkeletonDefinition Skeleton { get; set; } = new();

    [JsonPropertyName("poses")]
    public List<PoseDefinition> Poses { get; set; } = [];
}

public sealed class SkeletonDefinition
{
    [JsonPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    [JsonPropertyName("sourceSkeletalMesh")]
    public string SourceSkeletalMesh { get; set; } = string.Empty;

    [JsonPropertyName("sourcePoseDirectory")]
    public string SourcePoseDirectory { get; set; } = string.Empty;

    [JsonPropertyName("units")]
    public string Units { get; set; } = "cm";

    [JsonPropertyName("exportedAtUtc")]
    public string ExportedAtUtc { get; set; } = string.Empty;

    [JsonPropertyName("previewViewForward")]
    public float[] PreviewViewForward { get; set; } = [];

    [JsonPropertyName("previewUp")]
    public float[] PreviewUp { get; set; } = [];

    [JsonPropertyName("bones")]
    public List<BoneDefinition> Bones { get; set; } = [];
}

public sealed class BoneDefinition
{
    [JsonPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    [JsonPropertyName("parent")]
    public string? Parent { get; set; }

    [JsonPropertyName("localTranslation")]
    public float[] LocalTranslation { get; set; } = [];

    [JsonPropertyName("localRotationQuaternion")]
    public float[] LocalRotationQuaternion { get; set; } = [];

    [JsonPropertyName("localScale")]
    public float[] LocalScale { get; set; } = [];

    [JsonPropertyName("showInPreview")]
    public bool ShowInPreview { get; set; }

    [JsonPropertyName("previewColor")]
    public float[] PreviewColor { get; set; } = [];

    [JsonPropertyName("previewThickness")]
    public double PreviewThickness { get; set; } = 1.0;
}

public sealed class PoseDefinition
{
    [JsonPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    [JsonPropertyName("category")]
    public string Category { get; set; } = string.Empty;

    [JsonPropertyName("notes")]
    public string Notes { get; set; } = string.Empty;

    [JsonPropertyName("sourceFile")]
    public string SourceFile { get; set; } = string.Empty;

    [JsonPropertyName("leftHandPreset")]
    public string LeftHandPreset { get; set; } = string.Empty;

    [JsonPropertyName("rightHandPreset")]
    public string RightHandPreset { get; set; } = string.Empty;

    [JsonPropertyName("boneOffsets")]
    public Dictionary<string, BoneOffsetDefinition> BoneOffsets { get; set; } = new(StringComparer.OrdinalIgnoreCase);
}

public sealed class BoneOffsetDefinition
{
    [JsonPropertyName("location")]
    public float[] Location { get; set; } = [];

    [JsonPropertyName("rotationQuaternion")]
    public float[] RotationQuaternion { get; set; } = [];
}

public sealed class LegacyPoseBundle
{
    [JsonPropertyName("version")]
    public string Version { get; set; } = string.Empty;

    [JsonPropertyName("bundleType")]
    public string BundleType { get; set; } = string.Empty;

    [JsonPropertyName("skeleton")]
    public string Skeleton { get; set; } = string.Empty;

    [JsonPropertyName("sourceDirectory")]
    public string SourceDirectory { get; set; } = string.Empty;

    [JsonPropertyName("poseCount")]
    public int PoseCount { get; set; }

    [JsonPropertyName("generatedAtUtc")]
    public string GeneratedAtUtc { get; set; } = string.Empty;

    [JsonPropertyName("poses")]
    public List<LegacyPoseBundleEntry> Poses { get; set; } = [];
}

public sealed class LegacyPoseBundleEntry
{
    [JsonPropertyName("fileName")]
    public string FileName { get; set; } = string.Empty;

    [JsonPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    [JsonPropertyName("category")]
    public string Category { get; set; } = string.Empty;

    [JsonPropertyName("notes")]
    public string Notes { get; set; } = string.Empty;

    [JsonPropertyName("pose")]
    public LegacyRawPose Pose { get; set; } = new();
}

public sealed class LegacyRawPose
{
    [JsonPropertyName("version")]
    public string Version { get; set; } = string.Empty;

    [JsonPropertyName("skeleton")]
    public string Skeleton { get; set; } = string.Empty;

    [JsonPropertyName("referencePose")]
    public string ReferencePose { get; set; } = string.Empty;

    [JsonPropertyName("name")]
    public string Name { get; set; } = string.Empty;

    [JsonPropertyName("category")]
    public string Category { get; set; } = string.Empty;

    [JsonPropertyName("notes")]
    public string Notes { get; set; } = string.Empty;

    [JsonPropertyName("leftHandPreset")]
    public string LeftHandPreset { get; set; } = string.Empty;

    [JsonPropertyName("rightHandPreset")]
    public string RightHandPreset { get; set; } = string.Empty;

    [JsonPropertyName("fkBones")]
    public Dictionary<string, LegacyRawBoneTransform> FkBones { get; set; } = new(StringComparer.OrdinalIgnoreCase);
}

public sealed class LegacyRawBoneTransform
{
    [JsonPropertyName("location")]
    public float[] Location { get; set; } = [];

    [JsonPropertyName("rotation")]
    public float[] Rotation { get; set; } = [];
}
