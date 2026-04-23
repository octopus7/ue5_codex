using System.Numerics;
using System.Windows.Media;
using MannyPoseViewer.Models;

namespace MannyPoseViewer.Rendering;

internal static class PosePreviewMath
{
    internal readonly record struct PreviewSegment(Vector3 Start, Vector3 End, Color Color, double Thickness);

    private readonly record struct BoneWorldState(Vector3 Position, Quaternion Rotation, Vector3 Scale);

    public static IReadOnlyList<PreviewSegment> BuildSegments(PoseViewerDataset dataset, PoseDefinition pose)
    {
        if (dataset.Skeleton.Bones.Count == 0)
        {
            return [];
        }

        var boneDefinitions = dataset.Skeleton.Bones.ToDictionary(b => b.Name, StringComparer.OrdinalIgnoreCase);
        var worldStates = new Dictionary<string, BoneWorldState>(dataset.Skeleton.Bones.Count, StringComparer.OrdinalIgnoreCase);

        foreach (var bone in dataset.Skeleton.Bones)
        {
            var localTranslation = ReadVector3(bone.LocalTranslation, Vector3.Zero);
            var localRotation = Normalize(ReadQuaternion(bone.LocalRotationQuaternion, Quaternion.Identity));
            var localScale = ReadVector3(bone.LocalScale, Vector3.One);

            if (pose.BoneOffsets.TryGetValue(bone.Name, out var offset))
            {
                localTranslation += ReadVector3(offset.Location, Vector3.Zero);
                localRotation = Normalize(Multiply(ReadQuaternion(offset.RotationQuaternion, Quaternion.Identity), localRotation));
            }

            if (!string.IsNullOrWhiteSpace(bone.Parent) && worldStates.TryGetValue(bone.Parent, out var parentState))
            {
                var scaledTranslation = MultiplyComponents(localTranslation, parentState.Scale);
                var worldPosition = parentState.Position + RotateVector(scaledTranslation, parentState.Rotation);
                var worldRotation = Normalize(Multiply(parentState.Rotation, localRotation));
                var worldScale = MultiplyComponents(parentState.Scale, localScale);
                worldStates[bone.Name] = new BoneWorldState(worldPosition, worldRotation, worldScale);
            }
            else
            {
                worldStates[bone.Name] = new BoneWorldState(localTranslation, localRotation, localScale);
            }
        }

        var segments = new List<PreviewSegment>(dataset.Skeleton.Bones.Count);
        foreach (var bone in dataset.Skeleton.Bones)
        {
            if (!bone.ShowInPreview || !worldStates.TryGetValue(bone.Name, out var state))
            {
                continue;
            }

            var parentName = bone.Parent;
            while (!string.IsNullOrWhiteSpace(parentName))
            {
                if (!boneDefinitions.TryGetValue(parentName, out var parentBone) ||
                    !worldStates.TryGetValue(parentName, out var parentState))
                {
                    break;
                }

                if (parentBone.ShowInPreview)
                {
                    segments.Add(new PreviewSegment(
                        parentState.Position,
                        state.Position,
                        ReadColor(bone.PreviewColor),
                        bone.PreviewThickness));
                    break;
                }

                parentName = parentBone.Parent;
            }
        }

        return segments;
    }

    public static Vector3 ReadVector3(IReadOnlyList<float>? values, Vector3 fallback)
    {
        return values is { Count: >= 3 }
            ? new Vector3(values[0], values[1], values[2])
            : fallback;
    }

    public static Quaternion ReadQuaternion(IReadOnlyList<float>? values, Quaternion fallback)
    {
        return values is { Count: >= 4 }
            ? new Quaternion(values[0], values[1], values[2], values[3])
            : fallback;
    }

    public static Color ReadColor(IReadOnlyList<float>? values)
    {
        if (values is not { Count: >= 4 })
        {
            return Color.FromArgb(255, 210, 214, 224);
        }

        return Color.FromArgb(
            ClampToByte(values[3] * 255f),
            ClampToByte(values[0] * 255f),
            ClampToByte(values[1] * 255f),
            ClampToByte(values[2] * 255f));
    }

    public static Vector2 ProjectPoint(
        Vector3 point,
        Vector3 viewForward,
        Vector3 viewUp)
    {
        var forward = Normalize(viewForward);
        var up = Normalize(viewUp);
        var right = Normalize(Vector3.Cross(up, forward));
        var correctedUp = Normalize(Vector3.Cross(forward, right));
        return new Vector2(Vector3.Dot(point, right), Vector3.Dot(point, correctedUp));
    }

    private static Quaternion Multiply(Quaternion a, Quaternion b)
    {
        return new Quaternion(
            a.W * b.X + a.X * b.W + a.Y * b.Z - a.Z * b.Y,
            a.W * b.Y - a.X * b.Z + a.Y * b.W + a.Z * b.X,
            a.W * b.Z + a.X * b.Y - a.Y * b.X + a.Z * b.W,
            a.W * b.W - a.X * b.X - a.Y * b.Y - a.Z * b.Z);
    }

    private static Vector3 MultiplyComponents(Vector3 a, Vector3 b)
    {
        return new Vector3(a.X * b.X, a.Y * b.Y, a.Z * b.Z);
    }

    private static Vector3 RotateVector(Vector3 vector, Quaternion rotation)
    {
        return Vector3.Transform(vector, rotation);
    }

    private static Quaternion Normalize(Quaternion quaternion)
    {
        return quaternion.LengthSquared() > 0.0f ? Quaternion.Normalize(quaternion) : Quaternion.Identity;
    }

    private static Vector3 Normalize(Vector3 vector)
    {
        return vector.LengthSquared() > 0.0f ? Vector3.Normalize(vector) : Vector3.UnitZ;
    }

    private static byte ClampToByte(float value)
    {
        return (byte)Math.Clamp((int)Math.Round(value), 0, 255);
    }
}
