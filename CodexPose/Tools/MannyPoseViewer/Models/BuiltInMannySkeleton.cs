namespace MannyPoseViewer.Models;

internal static class BuiltInMannySkeleton
{
    public static SkeletonDefinition Create(string sourceDirectory, string exportedAtUtc)
    {
        return new SkeletonDefinition
        {
            Name = "SK_Mannequin",
            SourceSkeletalMesh = "/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple",
            SourcePoseDirectory = sourceDirectory,
            Units = "cm",
            ExportedAtUtc = string.IsNullOrWhiteSpace(exportedAtUtc) ? "2026-04-23T14:07:54.663Z" : exportedAtUtc,
            PreviewViewForward = [-1.0f, -1.3f, -0.45f],
            PreviewUp = [0.0f, 0.0f, 1.0f],
            Bones =
            [
                new BoneDefinition
                {
                    Name = "root",
                    Parent = null,
                    LocalTranslation = [0f, 0f, 0f],
                    LocalRotationQuaternion = [0f, 0f, 0f, 1f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = false,
                    PreviewColor = [0.18f, 0.2f, 0.24f, 0.55f],
                    PreviewThickness = 1
                },
                new BoneDefinition
                {
                    Name = "pelvis",
                    Parent = "root",
                    LocalTranslation = [-3.4315E-17f, 2.28086615f, 95.89678f],
                    LocalRotationQuaternion = [-0.02241495f, 0.7067514f, 0.02241495f, -0.7067514f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.8f, 0.84f, 0.9f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "spine_01",
                    Parent = "pelvis",
                    LocalTranslation = [3.67705345f, -7.10542736E-15f, -4.52069531E-16f],
                    LocalRotationQuaternion = [-3.75272528E-31f, 5.93702641E-32f, -0.1258295f, -0.9920519f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.8f, 0.84f, 0.9f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "spine_02",
                    Parent = "spine_01",
                    LocalTranslation = [6.7950573f, -2.13162821E-14f, 2.11333754E-16f],
                    LocalRotationQuaternion = [1.04457625E-31f, 3.19942342E-33f, 0.0302285943f, -0.999543f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.8f, 0.84f, 0.9f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "spine_03",
                    Parent = "spine_02",
                    LocalTranslation = [7.238228f, 2.30926389E-14f, 1.50663259E-16f],
                    LocalRotationQuaternion = [1.554162E-31f, -1.72127726E-32f, 0.09537736f, -0.9954412f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.8f, 0.84f, 0.9f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "spine_04",
                    Parent = "spine_03",
                    LocalTranslation = [8.523893f, 5.32907052E-15f, 2.84569745E-16f],
                    LocalRotationQuaternion = [3.91773756E-06f, -2.00760041E-07f, 0.0511767268f, -0.9986896f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.8f, 0.84f, 0.9f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "spine_05",
                    Parent = "spine_04",
                    LocalTranslation = [19.4398f, 1.781731E-07f, -3.21547259E-16f],
                    LocalRotationQuaternion = [-3.92281663E-06f, -2.33261339E-08f, 0.00594620872f, -0.9999823f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.8f, 0.84f, 0.9f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "neck_01",
                    Parent = "spine_05",
                    LocalTranslation = [11.8877659f, -1.77635684E-14f, 1.59943188E-15f],
                    LocalRotationQuaternion = [1.71361061E-27f, -4.97450221E-29f, -0.207300514f, -0.9782773f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.8f, 0.84f, 0.9f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "neck_02",
                    Parent = "neck_01",
                    LocalTranslation = [5.11025953f, 5.684342E-14f, -4.083213E-14f],
                    LocalRotationQuaternion = [-8.54784844E-07f, 1.42751118E-08f, 0.0166979115f, -0.9998606f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = false,
                    PreviewColor = [0.18f, 0.2f, 0.24f, 0.55f],
                    PreviewThickness = 1
                },
                new BoneDefinition
                {
                    Name = "head",
                    Parent = "neck_02",
                    LocalTranslation = [4.912971f, 6.39488462E-14f, 2.17160477E-14f],
                    LocalRotationQuaternion = [8.503216E-07f, 8.847034E-08f, 0.103488415f, -0.994630635f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.8f, 0.84f, 0.9f, 1f],
                    PreviewThickness = 3
                },
                new BoneDefinition
                {
                    Name = "clavicle_l",
                    Parent = "spine_05",
                    LocalTranslation = [5.51626873f, -1.314766f, -1.42790425f],
                    LocalRotationQuaternion = [0.08327832f, -0.7545604f, -0.0412900075f, -0.6496141f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.35f, 0.8f, 1f, 1f],
                    PreviewThickness = 2
                },
                new BoneDefinition
                {
                    Name = "upperarm_l",
                    Parent = "clavicle_l",
                    LocalTranslation = [17.8095226f, -2.264855E-14f, 5.684342E-14f],
                    LocalRotationQuaternion = [-0.0199478511f, -0.391730756f, -0.020190157f, -0.919642031f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.35f, 0.8f, 1f, 1f],
                    PreviewThickness = 2
                },
                new BoneDefinition
                {
                    Name = "lowerarm_l",
                    Parent = "upperarm_l",
                    LocalTranslation = [27.77114f, -1.0658141E-14f, -7.10542736E-14f],
                    LocalRotationQuaternion = [9.260166E-18f, -2.616527E-17f, -0.333632648f, -0.9427032f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.35f, 0.8f, 1f, 1f],
                    PreviewThickness = 2
                },
                new BoneDefinition
                {
                    Name = "hand_l",
                    Parent = "lowerarm_l",
                    LocalTranslation = [27.2510738f, 2.13162821E-14f, 5.684342E-14f],
                    LocalRotationQuaternion = [-0.557586849f, -0.001678249f, 0.02056061f, -0.829862237f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.35f, 0.8f, 1f, 1f],
                    PreviewThickness = 2
                },
                new BoneDefinition
                {
                    Name = "clavicle_r",
                    Parent = "spine_05",
                    LocalTranslation = [5.51622f, -1.31481123f, 1.42787266f],
                    LocalRotationQuaternion = [0.7545604f, 0.08327832f, -0.6496141f, 0.0412900075f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [1f, 0.72f, 0.35f, 1f],
                    PreviewThickness = 2
                },
                new BoneDefinition
                {
                    Name = "upperarm_r",
                    Parent = "clavicle_r",
                    LocalTranslation = [-17.8096256f, -2.87161447E-06f, 0.000438096933f],
                    LocalRotationQuaternion = [-0.0199478511f, -0.391730756f, -0.020190157f, -0.919642031f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [1f, 0.72f, 0.35f, 1f],
                    PreviewThickness = 2
                },
                new BoneDefinition
                {
                    Name = "lowerarm_r",
                    Parent = "upperarm_r",
                    LocalTranslation = [-27.7706947f, 3.73034936E-14f, 5.684342E-14f],
                    LocalRotationQuaternion = [2.616527E-17f, 9.260166E-18f, -0.333632648f, -0.9427032f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [1f, 0.72f, 0.35f, 1f],
                    PreviewThickness = 2
                },
                new BoneDefinition
                {
                    Name = "hand_r",
                    Parent = "lowerarm_r",
                    LocalTranslation = [-27.25101f, 7.10542736E-15f, -2.842171E-14f],
                    LocalRotationQuaternion = [-0.557586849f, -0.001678249f, 0.02056061f, -0.829862237f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [1f, 0.72f, 0.35f, 1f],
                    PreviewThickness = 2
                },
                new BoneDefinition
                {
                    Name = "thigh_r",
                    Parent = "pelvis",
                    LocalTranslation = [-2.36572623f, 0.119487792f, 9.969091f],
                    LocalRotationQuaternion = [0.0294620469f, -0.072405f, 0.9965191f, -0.0289694928f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [1f, 0.72f, 0.35f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "calf_r",
                    Parent = "thigh_r",
                    LocalTranslation = [43.3412628f, -3.55271368E-15f, 7.10542736E-15f],
                    LocalRotationQuaternion = [7.798811E-18f, 3.408338E-19f, -0.0436616242f, -0.9990464f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [1f, 0.72f, 0.35f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "foot_r",
                    Parent = "calf_r",
                    LocalTranslation = [42.21795f, 1.77635684E-15f, 1.77635684E-15f],
                    LocalRotationQuaternion = [-0.000665655651f, -0.026877109f, 0.0232392959f, -0.99936837f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [1f, 0.72f, 0.35f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "ball_r",
                    Parent = "foot_r",
                    LocalTranslation = [7.00943661f, -15.2375946f, -0.5389456f],
                    LocalRotationQuaternion = [-3.33611118E-19f, 1.13140286E-18f, -0.707106769f, -0.707106769f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [1f, 0.72f, 0.35f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "thigh_l",
                    Parent = "pelvis",
                    LocalTranslation = [-2.36571121f, 0.110043764f, -9.969203f],
                    LocalRotationQuaternion = [0.072405f, 0.0294620469f, -0.0289694928f, -0.9965191f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.35f, 0.8f, 1f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "calf_l",
                    Parent = "thigh_l",
                    LocalTranslation = [-43.34131f, -4.440892E-15f, 7.10542736E-15f],
                    LocalRotationQuaternion = [-7.798811E-18f, -3.408338E-19f, -0.0436616242f, -0.9990464f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.35f, 0.8f, 1f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "foot_l",
                    Parent = "calf_l",
                    LocalTranslation = [-42.2179146f, 1.66533454E-15f, 7.10542736E-15f],
                    LocalRotationQuaternion = [-0.000665655651f, -0.026877109f, 0.0232392959f, -0.99936837f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.35f, 0.8f, 1f, 1f],
                    PreviewThickness = 2.5
                },
                new BoneDefinition
                {
                    Name = "ball_l",
                    Parent = "foot_l",
                    LocalTranslation = [-7.00943661f, 15.2375889f, 0.538887441f],
                    LocalRotationQuaternion = [-1.41908252E-18f, 1.30168977E-18f, -0.707106769f, -0.707106769f],
                    LocalScale = [1f, 1f, 1f],
                    ShowInPreview = true,
                    PreviewColor = [0.35f, 0.8f, 1f, 1f],
                    PreviewThickness = 2.5
                }
            ]
        };
    }
}
