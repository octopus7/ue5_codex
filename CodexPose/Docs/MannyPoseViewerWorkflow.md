# Manny Pose Viewer Workflow

`MannyPoseToolkit` now supports the full pipeline:

- 20 bundled body pose JSON files in `Plugins/MannyPoseToolkit/Content/Poses/Bodies`
- UE editor-side skeleton + pose export to one viewer dataset JSON
- a standalone WPF line viewer at `Tools/MannyPoseViewer`

## UE Export

1. Place or select `BP_MannyPosePreview` in the level.
2. Assign the real `SKM_Manny` skeletal mesh to the actor's `PoseableMesh`.
3. Open `Window > Manny Pose Toolkit`.
4. Use `Export Viewer Dataset`.

The exporter writes:

- `Saved/MannyPoseToolkit/Exports/manny_pose_viewer_dataset.json`

The export uses the selected actor's actual skeletal mesh reference skeleton, then scans every body pose JSON in:

- `Plugins/MannyPoseToolkit/Content/Poses/Bodies`

## WPF Viewer

Build or run:

```powershell
dotnet build Tools\MannyPoseViewer\MannyPoseViewer.csproj
dotnet run --project Tools\MannyPoseViewer\MannyPoseViewer.csproj
```

The viewer will:

- auto-load `Saved/MannyPoseToolkit/Exports/manny_pose_viewer_dataset.json` when it can resolve the repo root
- watch the opened JSON file and auto-reload when UE exports again
- render Manny as a line skeleton using the exported reference pose and local pose offsets

## Dataset Shape

The exported dataset contains:

- `skeleton`
  - exported Manny bones with parent chain, local translation, local quaternion, scale
  - preview metadata such as line color, thickness, and the shared projection vector
- `poses`
  - one entry per body pose JSON
  - per-bone local location offsets
  - per-bone local rotation offsets already normalized to quaternions for the viewer

The original body pose JSON files remain the editable source of truth. The viewer dataset is a derived export optimized for external preview.
