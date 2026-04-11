# VOX Mesh Asset Generation Plan

## Document Purpose
This document defines the implementation plan and operating standards for preparing player/enemy test VOX meshes at `32 x 32 x 32` resolution and generating UE5 `StaticMesh` assets through the `CodexUMGBootstrapEditor` editor module and a commandlet.

This is not just an idea note. It is a baseline document that includes coordinate-system rules, winding, normals, pivot rules, file-naming rules, and verification procedures so the same problems do not repeat on the next run.

## Goals
1. All source VOX files use a unified `32^3` resolution.
2. Color is expressed only through vertex colors.
3. All meshes use only the single shared material `M_VoxVertexColor`.
4. Generation runs through a `Commandlet` while no editor UI session is open.
5. If there is any risk of conflict with a running editor session, notify the user immediately and stop the work.
6. Document prevention rules so future runs do not produce lying-down meshes, inverted normals, or broken file connections.
7. Keep the sample palette more saturated overall than before, and generate angled preview PNGs during the SourceArt stage as well.

## Target Asset List

### Characters / animals / creatures
- white chicken
- brown chicken
- chick
- pig

### Food / items
- apple
- milk bottle
- strawberry
- banana
- grape bunch
- raw meat
- cooked meat with bone

### Props / structures
- wooden signpost

### Repeat-placeable elements
- fence
- bush
- grass
- yellow flower
- white flower
- red flower
- sunflower
- pebble ground
- water lily

### Diagnostics / validation
- high-saturation rainbow + 16-step grayscale diagnostic mesh

## Deliverables

### SourceArt
- `.vox` files under `SourceArt/Vox/Sources/`
- sample generation script: `Scripts/GenerateSampleVoxSources.ps1`
- angled preview PNG generation script: `Scripts/GenerateVoxPreviewPngs.py`
- manifest: `SourceArt/Vox/VoxAssetManifest.json`
- preview output: `SourceArt/Vox/Previews/<Category>/SM_Vox_*.png`

### UE assets
- shared material
  - `/Game/Vox/Materials/M_VoxVertexColor`
- static meshes
  - `/Game/Vox/Meshes/Characters/`
  - `/Game/Vox/Meshes/Food/`
  - `/Game/Vox/Meshes/Props/`
    - `/Game/Vox/Meshes/Props/SM_Vox_WoodenSignpost`
    - `/Game/Vox/Meshes/Props/SM_Vox_RainbowDiagnostic`
  - `/Game/Vox/Meshes/Foliage/`
  - `/Game/Vox/Meshes/Ground/`

## Canonical Generated Asset Naming Rules
- Compare generated outputs by the manifest's `targetPackagePath + targetAssetName` plus the canonical rules below, not by raw filename alone.
- If a legacy alias still appears in a document or checklist, normalize it to the canonical path before verifying results.

| Target | Canonical asset path | Accepted alias / legacy reference | Notes |
| --- | --- | --- | --- |
| Milk bottle | `/Game/Vox/Meshes/Food/SM_Vox_MilkBottle` | `SM_Vox_BottleMilk` | fixed food category |
| Grape cluster | `/Game/Vox/Meshes/Food/SM_Vox_GrapeCluster` | `SM_Vox_Grapes` | fixed food category |
| Bone-in roast | `/Game/Vox/Meshes/Food/SM_Vox_BoneInRoast` | `SM_Vox_BoneInRoastedMeat` | fixed food category |
| Water lily | `/Game/Vox/Meshes/Foliage/SM_Vox_WaterLily` | `/Game/Vox/Meshes/Ground/SM_Vox_WaterLily`, `/Game/Vox/Meshes/Props/SM_Vox_WaterLily`, `Ground` / `Props` category refs | the canonical category and path for `SM_Vox_WaterLily` are `Foliage` |

## UE Asset Specification

### Color handling
- Record `.vox` palette colors into mesh vertex colors.
- Use a single shared material with the structure `Vertex Color RGB -> Base Color`.
- Do not generate multiple materials per color or let one mesh own many material slots.
- `XYZI.ColorIndex` is not a literal RGB value. It is a VOX palette index.
- Therefore the mesh builder must resolve actual colors by mapping `Voxel.ColorIndex -> Palette[ColorIndex]`.
- VOX palette RGB values are typically authored in sRGB space, so when writing into `MeshDescription`, convert them to linear before storing them as vertex colors.
- In other words, do not simply divide `FColor` by `255`. Convert `sRGB -> Linear` first, then store the result.

### Vertex-color color-space rules
- UE `MeshDescription` vertex-color attributes are treated in linear color space.
- Sample VOX palettes are treated as sRGB-authored colors.
- Therefore the implementation standard is:
  - input: `FColor` sRGB values from the `.vox` palette
  - storage: linear vertex colors converted with `FLinearColor::FromSRGBColor(...)`
  - material: `Vertex Color -> custom sRGB-to-linear decode -> Base Color`
- The VOX pipeline must include this material-side decode as well so UE static-mesh vertex colors match the perceived brightness of the authored sRGB palette.
- Because UE static-mesh build paths may quantize vertex colors back into sRGB bytes, removing this decode from the shared material can make midtones such as `FenceBrown` or middle gray appear brighter than authored.

### Absolutely prohibited
- Do not treat sRGB palette colors as linear vertex colors by merely dividing `R/255`, `G/255`, and `B/255`.
  - That causes gamma to be applied again on screen, making colors look washed out and pastel.
- Do not solve a brightness issue by removing `FLinearColor::FromSRGBColor(...)`.
  - Even if the impact is less obvious on pure primaries, that change can push midtones such as brown, gray, and ivory noticeably brighter.
- Do not misuse wide-gamut conversion nodes in the shared VOX material as if they were gamma-decode replacements.
  - When the default working color space is sRGB, the intended gamma decode may not happen.
- Do not accept a state where `Vertex Color` goes straight into `Base Color` with no correction and midtones appear too bright.
- Do not solve gamma issues by adding material slots or branching materials per color.
  - The root cause is the color-space conversion path, so the fix must align the shared VOX material and the vertex-color storage rules to the same standard.

### VOX palette serialization rules
- Sample `.vox` files follow the MagicaVoxel `RGBA` chunk rules as-is.
- `RGBA` raw entry `0` corresponds to palette index `1`.
- `RGBA` raw entry `255` remains an unused slot.
- Therefore the sample-generation script must not push in a dummy color at the front of the `RGBA` chunk.
- `XYZI.ColorIndex` behaves like a `1-based` palette index, so both sample authoring and parser implementation follow that rule.
- In the sample generator, the first color in the array must be the color that actually lands in palette index `1`.

### Palette regression-prevention rules
- When changing the sample-generation script, first verify that the `RGBA` chunk and `XYZI.ColorIndex` are not shifted by one.
- Use a clearly colored sample such as the strawberry for baseline validation.
  - the body of `SM_Vox_Strawberry` should be red
  - the cap of `SM_Vox_Strawberry` should be green
- Always generate a separate diagnostic mesh as well so primary-color reproduction and grayscale reproduction are validated together.
  - the top half of `SM_Vox_RainbowDiagnostic` uses stripes of pure sRGB `255/0/0`, `255/127/0`, `255/255/0`, `0/255/0`, `0/255/255`, `0/0/255`, and `255/0/255`
  - the bottom half uses a 16-step neutral gray ramp from `0, 17, 34, ..., 255`, laid out left to right
  - if the top primaries look vivid even in `Unlit`, low saturation in existing samples is probably a SourceArt choice
  - if the entire bottom gray ramp also looks lifted or pastel, inspect the `sRGB -> Linear` conversion or vertex-color storage path first
- Keep the current sample palette more saturated overall for test readability.
  - examples: `Red = (248, 48, 48)`, `LeafGreen = (84, 224, 78)`, `BananaYellow = (250, 224, 52)`
  - however, neutral colors such as `WhitePetal`, `Bone`, and `PebbleGray` should not be pushed into full primaries because of their material character
- When adjusting saturation further, do not touch the material or vertex-color storage path. Update the palette definition in `Scripts/GenerateSampleVoxSources.ps1`, then regenerate the `.vox` files and preview PNGs together.
- If a mesh looks blurry, pastel, or completely wrong in hue, suspect a palette-offset bug before the material.
- If the palette indices are correct but everything still looks bright and washed out, suspect the color-space path first.
  - representative symptoms:
  - red looks pink
  - green looks mint
  - the whole result looks pastel even in `Unlit`

### Base rules for mesh generation
- Remove faces hidden inside the volume and generate mesh data only for outward-facing faces.
- The first implementation uses a simple face-extraction approach.
- Each face is generated with 4 corners and 2 triangles.
- Record the same VOX color into all 4 vertex instances of the same face.

## Coordinate System, Winding, And Pivot Rules

This section is the core regression-prevention baseline. If a mesh lies on its side, normals flip, or thumbnails look wrong, verify this section first.

### VOX source coordinate system
- Sample `.vox` content is authored in `Y-up`.
- That means the height axis in VOX source is `+Y`.
- Vertical objects such as chickens, pigs, and milk bottles use VOX `Y` as the up axis.
- The default length direction of a fence is `+X`.

### UE conversion coordinate system
- UE uses `Z-up`, so axis conversion must be applied during import.
- Position conversion formula:
  - `VOX (X, Y, Z) -> UE (X, -Z, Y) * VoxelSize`
- Direction-vector conversion formula:
  - `VOX (X, Y, Z) -> UE (X, -Z, Y)`
- Normals and tangents must use the same axis conversion as positions.

### UE winding rules
- UE static meshes use a left-handed coordinate system with `CCW front face`.
- Therefore after converting VOX corners into UE axes, record each face triangle with one winding reversal.
- Quad split example:
  - if the original order is `(0, 1, 2)` / `(0, 2, 3)`
  - the UE-recorded order becomes `(0, 2, 1)` / `(0, 3, 2)`
- If this rule is omitted, the mesh pose may look correct while front faces and normals are effectively reversed.

### Triangle-index regression-prevention rules
- In this project, the axis-conversion formula, per-face corner order, and final triangle-index order are treated as one set.
- As long as the current face-corner order is preserved, the baseline triangle indices are `(0, 2, 1)` / `(0, 3, 2)`.
- If `FaceDefinitions` or `GetFaceCorners` changes the corner order, triangle indices must be recalculated together.
- Do not keep the corner order unchanged and only change the default triangle indices back to `(0, 1, 2)` / `(0, 2, 3)`.
  - That can reintroduce the regression where all six face directions end up facing inward.
- Even if a conditional triangle-flip path is added, the rule must be based on whether the candidate normal computed from the transformed corners still matches the transformed outward face normal.
- Do not simultaneously redefine face-corner arrays and change triangle-flip logic just to simplify the implementation.
  - If both change together, the result can look superficially symmetric while the true front faces are flipped again.
- Whenever `CodexVoxMeshBuilder` is refactored, always review the four items below together:
  - `TransformPosition`
  - `TransformDirection`
  - per-face corner order
  - final index order passed to `AppendTriangle`

### Absolutely prohibited
- Do not use VOX coordinates directly as `UE (X, Y, Z)`.
  - That makes a `Y-up` source lie sideways in UE.
- Do not apply only axis conversion and omit the winding reversal.
  - Normals, shading, and culling direction can all become inverted.
- Do not reverse winding twice.
  - Only one reversal is required. A second reversal makes the direction wrong again.
- Do not assume VOX `Z` is the height axis when computing pivots.
  - In the sample VOX sources, the height axis is VOX `Y`.

### Pivot rules
- `GroundCentered`
  - pivot based on ground contact
  - align the lowest VOX `Y` so that it becomes UE `Z=0`
- `Centered`
  - pivot at the model center
  - use the center of the full bounding box including VOX `Y`

## Coordinate / Normal Regression Checklist
After regenerating meshes, always check the items below.

1. `SM_Vox_WhiteChicken` and `SM_Vox_Pig` thumbnails must not appear lying sideways.
2. Character-type meshes should have `Z extent` greater than or roughly similar to `Y extent`.
3. `SM_Vox_PebbleGroundTile` should be a flat ground mesh, so `Z extent` should be small.
4. If the surface looks like the inside instead of the outside, inspect winding / normal inversion first.
5. In Static Mesh Editor with face normals shown, normals must point outward.
6. In Static Mesh Editor with `Show > Advanced > Vertex Colors`, the colors should look correct.
7. In `Unlit`, `SM_Vox_Strawberry` should have a red body, not yellow or ivory.
8. The strawberry cap should be green, not mint.
9. In `Unlit`, the top half of `SM_Vox_RainbowDiagnostic` should show strong red, orange, yellow, green, cyan, blue, and magenta stripes without muddiness.
10. The bottom half of the same mesh should show a clearly separated 16-step gray ramp from black to white.
11. If only the strawberry looks soft while both the top and bottom diagnostic patterns of `SM_Vox_RainbowDiagnostic` are correct, the original sample palette is simply low-saturation by design.
12. If colors look shifted by one step overall, revisit `.vox` `RGBA` chunk serialization first.
13. If even the rainbow mesh or gray ramp looks bright and washed out overall, check whether `sRGB -> Linear` conversion is missing.
14. When opening `SM_Vox_Pig`, `SM_Vox_WhiteChicken`, and `SM_Vox_Fence`, there must be no feeling that only the inside faces are visible or that backface culling is wrong.
15. If the VOX mesh builder was modified, verify front-face direction at least once in both `Face Normal` view and `Unlit` view.

Current example expected extents:
- `SM_Vox_Pig`: `extent=(42.5, 22.5, 32.5)`
- `SM_Vox_WhiteChicken`: `extent=(32.5, 17.5, 37.5)`
- `SM_Vox_PebbleGroundTile`: `extent=(80.0, 80.0, 10.0)`

## Implementation Direction

### Module layout
- `Source/CodexUMGBootstrapEditor/Public/Commandlets/CodexVoxAssetBuildCommandlet.h`
- `Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexVoxAssetBuildCommandlet.cpp`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxTypes.h`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxParser.h`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxMeshBuilder.h`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxMaterialBuilder.h`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxAssetGenerator.h`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxParser.cpp`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxMeshBuilder.cpp`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxMaterialBuilder.cpp`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxAssetGenerator.cpp`

### Component responsibilities
- `CodexVoxParser`
  - reads `SIZE`, `XYZI`, and `RGBA` from `.vox`
  - uses only a single model
  - validates `32^3` resolution
  - follows MagicaVoxel indexing rules exactly for `XYZI.ColorIndex` and the `RGBA` palette
- `CodexVoxMeshBuilder`
  - extracts only outward faces
  - records vertex colors
  - converts VOX palette sRGB colors into linear vertex colors before storage
  - handles `VOX -> UE` axis conversion, winding reversal, and pivot correction
  - keeps face-corner order and triangle-index order from drifting independently
  - after refactoring, verifies that the baseline indices `(0, 2, 1)` / `(0, 3, 2)` still preserve outward-facing surfaces
- `CodexVoxMaterialBuilder`
  - creates or updates the shared material `M_VoxVertexColor`
- `CodexVoxAssetGenerator`
  - loads the manifest
  - checks whether an editor session is running
  - generates / saves meshes
  - wires the shared material
  - supports an exception build path that adds only new meshes without touching the existing shared material or existing meshes
- `CodexVoxAssetBuildCommandlet`
  - headless execution entry point
  - handles `-Manifest=`, `-Verbose`, `-NoOverwrite`, and `-SkipMaterialUpdate`

### Principles for per-asset exception paths
- Keep the baseline path centered on the shared parser, shared mesh builder, shared material, and manifest-based generation flow.
- However, if only one specific mesh must be added, or if the shared VOX material is under user edits and must not be overwritten, support a localized exception path for that case.
- Exceptions must be localized as options or asset-level branches with clear impact boundaries, not by flipping global behavior.
- In the sample SourceArt stage, split shape exceptions by adding per-asset builder functions such as `Build-<AssetName>`.
- In the UE build stage, options such as `-NoOverwrite` and `-SkipMaterialUpdate` must allow creation of only missing new assets while preserving existing meshes and the shared material.
- An exception implemented for one mesh must not change the default generation rules, color path, or shared-material rule for all other VOX meshes.

## Manifest Rules

The manifest lives at `SourceArt/Vox/VoxAssetManifest.json`.

### Fields
- `id`
- `displayName`
- `sourceVoxFile`
- `targetPackagePath`
- `targetAssetName`
- `category`
- `repeatable`
- `pivotRule`
- `collisionType`
- `notes`

### `sourceVoxFile` rules
- The sample-generation script creates files using the `SourceArt/Vox/Sources/SM_Vox_*.vox` pattern.
- Therefore `sourceVoxFile` must exactly match the real generated filename.
- Wrong example:
  - `Sources/white_chicken.vox`
- Correct example:
  - `Sources/SM_Vox_WhiteChicken.vox`

If this rule is broken, the commandlet may run but still fail because it cannot find the `.vox` file.

### SourceArt palette rules
- If sample `.vox` files are regenerated, do not stop at SourceArt only. Regenerate the UE meshes through the commandlet as well.
- If the palette or voxel shape changes, also regenerate the angled PNGs under `SourceArt/Vox/Previews/` so color and silhouette can be checked first at the SourceArt stage.
- If colors diverge between SourceArt and UE assets, first check the `.vox` palette and `XYZI.ColorIndex` mapping.
- Example palette validation criteria:
  - `ColorIndex=5` used for the strawberry body should be a red family color
  - `ColorIndex=12` used for the leaves should be a green family color
  - the top stripes of `SM_Vox_RainbowDiagnostic` must appear in the order red, orange, yellow, green, cyan, blue, magenta
  - the bottom stripes of that same mesh must be a 16-step gray range from black to white left to right

## Angled Preview PNGs

### Purpose
- Quickly check color, silhouette, and palette-offset issues during the SourceArt stage before opening the UE editor.
- Compare `SM_Vox_Strawberry` and `SM_Vox_RainbowDiagnostic` immediately so authored-palette issues and color-space issues can be separated early.

### Generation method
- `Scripts/GenerateVoxPreviewPngs.py` reads `VoxAssetManifest.json` and parses each `.vox` file.
- Rendering uses an angled orthographic voxel preview with a transparent background.
- Draw only the exposed `Top`, `+X`, and `+Z` faces to produce lightweight inspection images.
- Preview PNGs do not add per-face shadowing or voxel grid lines and show the authored sRGB colors as flatly as possible.
- In other words, the goal is not a stylized thumbnail. The goal is a baseline image that is as close as possible to UE `Unlit` color inspection.
- Output is organized as `SourceArt/Vox/Previews/<Category>/SM_Vox_*.png`.

### Example usage
```powershell
.\Scripts\GenerateSampleVoxSources.ps1 -GeneratePreviews
```

```powershell
python .\Scripts\GenerateVoxPreviewPngs.py --manifest .\SourceArt\Vox\VoxAssetManifest.json --source-root .\SourceArt\Vox --output-root .\SourceArt\Vox\Previews
```

## Commandlet Design

### Class name
- `UCodexVoxAssetBuildCommandlet`

### Runtime token
- `CodexUMGBootstrapEditor.CodexVoxAssetBuildCommandlet`

### Example execution
```powershell
UnrealEditor-Cmd.exe "<ProjectDir>\\CodexUMG.uproject" -run=CodexUMGBootstrapEditor.CodexVoxAssetBuildCommandlet -Manifest="<ProjectDir>\\SourceArt\\Vox\\VoxAssetManifest.json" -unattended -nop4 -nosplash
```

### Example execution for per-asset exception builds
Use the combination below when existing meshes and the shared material must remain untouched and only missing new meshes should be added.

```powershell
.\Scripts\RunVoxAssetBuild.ps1 -ManifestPath .\SourceArt\Vox\VoxAssetManifest.json -NoOverwrite -SkipMaterialUpdate -VerboseBuild
```

### Runtime order
1. Validate input arguments and the manifest path.
2. Check whether an `UnrealEditor.exe` process for the current project is running.
3. If the editor is running, notify the user and stop immediately.
4. Create or update the shared material `M_VoxVertexColor`. However, if `-SkipMaterialUpdate` is used for a per-asset exception build, reuse the existing shared material as-is.
5. Iterate the manifest and load each `.vox` file.
6. Convert VOX voxel data into an external-face mesh.
7. Apply vertex colors and the shared material slot.
8. Create or update the `StaticMesh` asset. If `-NoOverwrite` is enabled, skip existing assets and only create missing ones.
9. If any fatal error occurs, exit immediately with a failure code.

## Conflict Handling While The Editor Is Running
This feature assumes the editor is closed.

### Runner script
- `Scripts/RunVoxAssetBuild.ps1`
- Responsibilities:
  - handle sample `.vox` generation options
  - check for a running `UnrealEditor.exe`
  - pass `-NoOverwrite` and `-SkipMaterialUpdate` for per-asset exception builds
  - call the commandlet

### Stop policy
- If an editor session for the same project is detected, stop the work.
- Example message:
  - `An Unreal Editor session for this project is running. Stop the editor and rerun the VOX asset build.`

## Shared Material Design

### Goal
- Every VOX mesh uses the same single material.
- Visual differences are expressed only through vertex colors.

### Material composition
- `Base Color <- Vertex Color`
- `Blend Mode = Opaque`
- `Roughness = 1.0`
- `Specular = 0.0`

## Mesh Build Direction

### First implementation
- For each voxel, inspect all 6 neighbor directions.
- If there is no neighbor or the neighbor lies outside the model, create that face.
- Generate 4 vertex instances and 2 triangles per face.
- Use the same vertex-color value for the full face.
- Keep only shared default UVs and solve shading issues through winding and normal rules.
- If the face-corner arrays are unchanged, use `(0, 2, 1)` / `(0, 3, 2)` as the baseline triangle order.
- If the face-corner arrays change, do not reuse the old indices blindly. Revalidate outward normals on all six face directions.

### Build settings
- `bRecomputeNormals = false`
- `bRecomputeTangents = false`
- `bGenerateLightmapUVs = false`
- `bRemoveDegenerates = false`

Because directly authored normals/tangents and the winding rules are the standard, do not depend on recomputation unless that standard itself changes.
Likewise, directly stored vertex colors are treated as linear values, so when feeding sRGB palettes, apply the linear conversion first.

## Parallel Work And Agent Use
When possible, use agents to parallelize work and reduce lead time. However, parallelization is allowed only when there are no write conflicts.

### Good candidates for parallelization
- organizing the sample `.vox` generation rules in SourceArt
- manifest validation
- organizing target lists by asset category
- strengthening documents and checklists
- writing read-only validation scripts

### Not allowed for parallelization
- editing the same file at the same time
- generating / saving the same `StaticMesh` asset at the same time
- updating the single shared material asset at the same time

### Recommended operating model
1. The main worker implements the commandlet and mesh builder.
2. Helper agents handle supporting work such as documents, manifest review, and sample-source-generation rules.
3. The main worker performs result integration in one place.

## Player Blueprint Integration Rules
- In this document, applying the brown-chicken mesh to the player is not part of the pure VOX generation stage. It is treated as an integration point where Track A and Track B meet.
- After Track B finishes generating `/Game/Vox/Meshes/Characters/SM_Vox_BrownChicken` and Track A finishes generating `/Game/Blueprints/Player/BP_Character_TopDown`, assign the mesh to the player BP in the next step.
- The target is the extra `StaticMeshComponent` used for player visibility in `BP_Character_TopDown`.
- Fix the mesh assignment to `/Game/Vox/Meshes/Characters/SM_Vox_BrownChicken`.
- If the character VOX mesh is authored so that its visual front points along local `+Y` (the right-hand direction) rather than local `+X`, align it to the `BP_Character_TopDown` forward `+X` by applying a relative `Yaw = -90` offset on the `StaticMeshComponent` or its parent `SceneComponent` when wiring it into the player BP.
- Do not solve this alignment issue by changing the shared VOX `VOX -> UE` axis-conversion rules or by rewriting the authored forward axis of the mesh asset itself just for this case.
- If either side does not exist yet, do not force the work to run early. Wait.
- Perform this connection only after both "brown chicken mesh generated" and "player BP generated" are satisfied, not immediately after VOX mesh generation alone.

## Actual Execution Order
1. Prepare `32^3` sample `.vox` files with `Scripts/GenerateSampleVoxSources.ps1`.
2. In the same stage, generate angled PNG previews with `-GeneratePreviews` or `Scripts/GenerateVoxPreviewPngs.py`.
3. Write or regenerate `SourceArt/Vox/VoxAssetManifest.json`.
4. Check `SM_Vox_Strawberry.png` and `SM_Vox_RainbowDiagnostic.png` first to confirm there is no color or palette-offset issue.
5. Implement the parser / mesh builder / material builder / generator / commandlet inside the `CodexUMGBootstrapEditor` module.
6. Use `Scripts/RunVoxAssetBuild.ps1` to check first whether the editor is running.
7. If the editor is closed, run the commandlet and generate the assets.
8. If both `SM_Vox_BrownChicken` and `BP_Character_TopDown` exist, assign the brown-chicken mesh to the extra `StaticMeshComponent` of `BP_Character_TopDown`, and apply a relative `Yaw -90` correction so the mesh's authored local `+Y` front faces the player BP forward `+X`.
9. Open the resulting meshes in Static Mesh Editor and verify pose, normal direction, and vertex colors.
10. Also open the player BP with the brown-chicken mesh assigned and verify visibility, size, and offset from the top-down view.
11. If only `SM_Vox_Strawberry` looks muted while both the top primaries and the bottom gray ramp of `SM_Vox_RainbowDiagnostic` are correct, treat it as a SourceArt palette choice.
12. If the top primaries or bottom gray ramp of `SM_Vox_RainbowDiagnostic` also look pink, mint, or pastel, inspect the vertex-color color-space path.
13. If any case appears where coordinate rules, winding, pivot, palette, or color-space rules are violated, update this document first and then reflect the fix in code.

## Final Verification Checklist
1. All `.vox` sources use `32^3` resolution.
2. All meshes use only the shared material `M_VoxVertexColor`.
3. Mesh colors appear through vertex colors, not through material-slot branching.
4. The white chicken, brown chicken, chick, and pig stand upright correctly.
5. Repeat-placeable elements such as fences, pebble ground, and water lilies have proportions appropriate to their use.
6. Thumbnails and previews look correct with no normal inversion.
7. If an editor session is running, the commandlet fails immediately and tells the user why.
8. The manifest path matches the real `.vox` filenames.
9. The body of `SM_Vox_Strawberry` appears red and the cap appears green.
10. The angled PNGs under `SourceArt/Vox/Previews/` are regenerated together with the latest `.vox` files.
11. The top of `SM_Vox_RainbowDiagnostic` shows strong saturated primary-color stripes, and the bottom shows a separated 16-step gray ramp from black to white.
12. Colors do not look shifted by one palette step overall.
13. Colors do not look pastel overall.
14. If `BP_Character_TopDown` exists, its extra visibility `StaticMeshComponent` references `/Game/Vox/Meshes/Characters/SM_Vox_BrownChicken`.
15. When the brown-chicken mesh is wired to the player BP, a relative `Yaw -90` correction is applied so the authored local `+Y` front aligns with the `BP_Character_TopDown` forward `+X`.
16. After wiring the brown-chicken mesh to the player BP, the silhouette remains recognizable from the top-down camera.

## Maintenance Notes
- First verify that the VOX sample generation script still uses `Y-up`.
- First verify that the VOX sample generation script serializes the `RGBA` chunk according to MagicaVoxel rules.
- If the `VOX -> UE` transform or winding rules change, revalidate character thumbnails, ground tiles, and fence results.
- If meshes start lying down again or normals flip, first inspect axis conversion, triangle order, and pivot rules in `CodexVoxMeshBuilder`.
- If the default triangle indices change or the conditional inversion logic is simplified, first verify whether the code still assumes the existing face-corner order.
- If "corner-order change" and "index-inversion logic change" landed together, roll one of them back first to isolate the cause, then verify again.
- If colors look wrong, inspect the palette-entry placement in `GenerateSampleVoxSources.ps1` and `CodexVoxParser`'s `RGBA` interpretation before the material.
- When colors look muddy, open `SM_Vox_Strawberry` and `SM_Vox_RainbowDiagnostic` together first to separate authored-palette issues from color-space issues.
- If the palette or voxel silhouette changes, regenerate angled PNGs with `GenerateVoxPreviewPngs.py` before looking at UE so SourceArt differences are checked first.
- If colors are in the right family but still look washed out and bright, first verify that both `FromSRGBColor(...)` in `CodexVoxMeshBuilder` and the custom `sRGB -> linear` decode in the shared VOX material are still present.
- If fences, gray ramps, or ivory tones look especially brighter than the authored palette, do not remove `FromSRGBColor(...)`. First verify that the shared VOX material wiring is still `Vertex Color -> custom sRGB-to-linear decode -> Base Color`.
- If the brown-chicken mesh attached to the player BP looks sideways, verify first that the relative `Yaw -90` correction on the player-visibility `StaticMeshComponent` is still in place before touching the shared VOX axis-conversion rules.
