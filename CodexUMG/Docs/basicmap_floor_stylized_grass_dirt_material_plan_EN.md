# BasicMap Floor Stylized Grass Dirt Material Application Plan

## Document Purpose
- This document defines the implementation standard for replacing the solid 50% gray on the `Floor` actor in `/Game/Maps/BasicMap` with a texture-based material imported from `SourceArt/T_Stylized_Grass_Dirt_01.png`.
- The base rule is to make the work repeatable through the editor-module code and `Commandlet` path already used in the project, not through manual editor clicking.
- If the same PNG is modified again and the build is rerun, the texture, material, and `BasicMap` assignment state must all update together.

## Multi-Plan Batch Integration
- Official batch documents:
  - [multi_plan_batch_execution_plan.md](./multi_plan_batch_execution_plan.md)
  - [multi_plan_batch_execution_plan_EN.md](./multi_plan_batch_execution_plan_EN.md)
- Track / phase placement:
  - Track H `basicmap_floor_stylized_grass_dirt_material_plan`
  - the recommended position is `Phase 3B. floor late map-write track`
- Prerequisites:
  - `SourceArt/T_Stylized_Grass_Dirt_01.png` must exist
  - `/Game/Maps/BasicMap` must exist
  - the current project editor must be closed
  - the `BasicMap` lock and the interaction-asset-build map-save window must both be free
- Shared resources / lock items:
  - `/Game/Maps/BasicMap`
  - `/Game/Materials/T_Stylized_Grass_Dirt_01`
  - `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01`
  - `UCodexBasicMapFloorBuildCommandlet`
  - the interaction asset build `BasicMap` save window
- No-parallel rule:
  - the floor task directly modifies and saves `BasicMap`
  - the interaction asset build family can also modify and save `BasicMap`
  - therefore Tracks C / E / F / G / H must not run their real map-save sections at the same time
- Owned commandlet:
  - `UCodexBasicMapFloorBuildCommandlet`
  - runtime token `CodexBasicMapFloorBuild`
- Final verification targets:
  - floor texture asset created or updated
  - floor material asset created or updated
  - `Floor` override persists after reopening `BasicMap`
  - repeat runs do not create duplicates or unnecessary resaves
- Relationship to other tracks:
  - `topdown_fixed_camera_wasd_plan` and `player_projectile_firing_plan` are not hard prerequisites
  - `vox_mesh_asset_pipeline_plan` only needs shared editor-module coordination
  - `interaction_umg_component_plan`, `interaction_message_popup_plan`, `interaction_scroll_message_popup_plan`, and `interaction_dual_tile_transfer_popup_plan` share `BasicMap` save-conflict risk
  - `UIPlayground` is currently out of batch scope and is not part of this floor-track parallelization plan

## Source File
- Original PNG: `SourceArt/T_Stylized_Grass_Dirt_01.png`
- Under this document's rules, that single file is the only source input.
- The implementation must not guess filenames. It must first verify that the file actually exists at the relative path above.

## Target State
1. `SourceArt/T_Stylized_Grass_Dirt_01.png` is imported or reimported as a real UE texture asset.
2. The texture asset `/Game/Materials/T_Stylized_Grass_Dirt_01` is created or updated.
3. The material asset `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01` is created or updated.
4. The material uses `TexCoord` tiled at `U/V 4.0`, runs that through the engine material function `TextureVariation` to break up repetition, connects the result to `Base Color`, and uses `Roughness = 1.0` and `Specular = 0.0`.
5. When `/Game/Maps/BasicMap` is opened, the `Floor` actor's material override is set to the new texture-based material.
6. Re-running the same build must not create duplicate assets and must always converge to the same result.
7. Do not touch actors other than `Floor`, the shared VOX mesh material, or the placement of interaction test actors.

## Current Project Touchpoints
- The material creation/update pattern already exists in [Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxMaterialBuilder.cpp](../Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxMaterialBuilder.cpp).
- The pattern of opening `BasicMap`, modifying it directly, and saving it already exists in [Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp](../Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp).
- Based on the serialized map string, `BasicMap` currently contains `PersistentLevel.Floor_0`, and the actor label is `Floor`. Therefore this task must find the target by actor label `Floor`, not by guessing something like "the first `StaticMeshActor`."
- The project already has a shared VOX-only material at `/Game/Vox/Materials/M_VoxVertexColor`, so this task must create a separate floor material instead of reusing or modifying that asset.

## Core Principles
- Create real texture and material assets under the project's `Content`. Do not overwrite engine default material instances.
- The texture source must come from `SourceArt/T_Stylized_Grass_Dirt_01.png`.
- Level assignment must be handled as a material override on the `Floor` actor instance inside `BasicMap`, not by modifying the source `StaticMesh` asset.
- If the `Floor` actor cannot be found, fail immediately instead of silently continuing.
- If the PNG source cannot be found or import fails, fail immediately.
- If there is a risk of save conflicts with an already open editor session, stop the work and notify the user.
- This task is limited strictly to `Floor` in `BasicMap`. Do not touch other maps, other floor actors, or other materials.

## Deliverables

### Editor / C++ deliverables
- `Source/CodexUMGBootstrapEditor/Private/Maps/CodexBasicMapFloorBuilder.h`
- `Source/CodexUMGBootstrapEditor/Private/Maps/CodexBasicMapFloorBuilder.cpp`
- `Source/CodexUMGBootstrapEditor/Public/Commandlets/CodexBasicMapFloorBuildCommandlet.h`
- `Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexBasicMapFloorBuildCommandlet.cpp`

### Real content deliverables
- `/Game/Materials/T_Stylized_Grass_Dirt_01`
- `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01`
- updated material override on the `Floor` actor inside `/Game/Maps/BasicMap`

## Texture Spec

### Source path
- File path: `SourceArt/T_Stylized_Grass_Dirt_01.png`

### UE asset path
- Package path: `/Game/Materials`
- Asset name: `T_Stylized_Grass_Dirt_01`

### Import rules
- Import the source PNG as a new asset, or reimport / replace it if an asset with the same name already exists.
- `sRGB = true`
- Use the texture only for color albedo.
- Use `Wrap` as the default texture address mode.
- Do not use UI compression or icon-style settings without a real reason.

## Material Spec

### Asset path
- Package path: `/Game/Materials`
- Asset name: `M_BasicMapFloor_StylizedGrassDirt01`

### Graph rules
- `Blend Mode = Opaque`
- `Shading Model = Default Lit`
- Use `TexCoord(UTiling = 4.0, VTiling = 4.0)` as the base UVs.
- Use `/Engine/Functions/Engine_MaterialFunctions03/Texturing/TextureVariation`.
- Connect `TextureVariation.UVs <- TexCoord(4x)`.
- Connect `TextureVariation.Shifted UVs -> TextureSample(T_Stylized_Grass_Dirt_01).UVs`.
- Connect `TextureVariation.Raw UVs -> DDX/DDY -> TextureSample` derivative inputs, and set `TextureSample.MipValueMode = Derivative`.
- `TextureSample(T_Stylized_Grass_Dirt_01).RGB -> Base Color`
- `Roughness <- Constant(1.0)`
- `Specular <- Constant(0.0)`

### UV rules
- Use a `TexCoord` node based on UV channel 0 of the `Floor` mesh.
- Set both `UTiling` and `VTiling` on `TexCoord` to `4.0`.
- To avoid an obviously repetitive pattern, use the `Shifted UVs` computed by the engine-provided `TextureVariation` function.
- Use the transformed UVs and the compensating `Raw UVs` exactly as produced by `TextureVariation`. Do not rebuild a second external offset-blend scheme.

### Prohibited
- locking `Base Color` to `Constant3Vector(0.5, 0.5, 0.5)`
- creating the texture like a temporary UI asset
- introducing a separate parent-material chain just to make a material instance
- modifying `/Game/Vox/Materials/M_VoxVertexColor` so the floor also shares it
- sampling UVs only once at 1x and exposing the repetition pattern directly
- reimplementing manual double-sampling plus arbitrary noise blending instead of using `TextureVariation`

## Level Application Method

### Target lookup rules
- Use only `/Game/Maps/BasicMap` as the target map.
- Find the target actor by actor label `Floor`.
- The object name `PersistentLevel.Floor_0` is only for current-state confirmation. Because the object name may change when the level is saved, the implementation should use the label `Floor` as the primary key.

### Application rules
- Load `BasicMap`, then iterate all level actors.
- Find the actor where `GetActorLabel() == "Floor"`.
- The actor must be an `AStaticMeshActor`.
- Get `UStaticMeshComponent`, then check `GetNumMaterials()`.
- If the slot count is at least 1, assign `M_BasicMapFloor_StylizedGrassDirt01` to every slot from `0..NumMaterials-1`.
- If the slot count is 0, fail instead of silently continuing.
- After application, mark the actor, texture, material, and world package dirty and save them.

### Scope limits
- Modify only the material override of the `Floor` actor.
- Do not modify the default slot material on the source `StaticMesh` asset referenced by `Floor`.
- Do not modify apples, strawberries, SkySphere, lights, or the player start in `BasicMap`.

## Recommended Implementation Order
1. In a new builder utility, verify the existence of `SourceArt/T_Stylized_Grass_Dirt_01.png`.
2. In the same builder utility, import or reimport the PNG as `/Game/Materials/T_Stylized_Grass_Dirt_01`.
3. Create an update function that builds `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01` referencing the imported texture and authors a `4x` tiling + `TextureVariation` graph.
4. In the same builder utility, implement map load, `Floor` lookup, material application, and save.
5. Match the map-modification flow to the same level as the existing `BasicMap` load/save pattern in [Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp](../Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp).
6. The `Commandlet` calls the three steps above in order and exits immediately on any intermediate failure.
7. Print a success message only when the texture asset, material asset, and map save have all succeeded.

## Recommended Implementation Details

### 1. Texture import
- Compute the absolute source path as `ProjectDir/SourceArt/T_Stylized_Grass_Dirt_01.png`.
- Fail immediately if the file does not exist.
- Import into `/Game/Materials` using `AssetTools` auto-import or an equivalent editor API.
- If an asset with the same name already exists, replacement update must be supported.
- Immediately reload the resulting `Texture2D` after import and use it for the material-generation step.

### 2. Material creation
- Prepare the package `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01`.
- If an existing asset is present, load and update it. Otherwise create a new one.
- Clear the existing graph with `UMaterialEditingLibrary::DeleteAllMaterialExpressions()`.
- Create `UMaterialExpressionTextureCoordinate`, `UMaterialExpressionMaterialFunctionCall`, `UMaterialExpressionDDX`, `UMaterialExpressionDDY`, `UMaterialExpressionTextureSample`, and two `UMaterialExpressionConstant`s.
- Set `TextureCoordinate.UTiling = 4.0` and `TextureCoordinate.VTiling = 4.0`.
- Set the `MaterialFunctionCall` to `/Engine/Functions/Engine_MaterialFunctions03/Texturing/TextureVariation`.
- Connect `TextureCoordinate -> TextureVariation.UVs`.
- Connect `TextureVariation.Shifted UVs` to `TextureSample.UVs`.
- Connect `TextureVariation.Raw UVs` to `DDX` and `DDY`, then connect those outputs to the derivative inputs of `TextureSample` (`DDX(UVs)`, `DDY(UVs)`).
- Set `TextureSample.MipValueMode` to `Derivative`.
- Connect the `TextureSample` result to `Base Color`.
- Connect constants to `Roughness` and `Specular`.
- Call `LayoutMaterialExpressions()` and `RecompileMaterial()`.
- Mark the asset and package dirty.

### 3. `BasicMap` load and application
- Convert `/Game/Maps/BasicMap` to a real file path and load it.
- Query level actors through `UEditorActorSubsystem`.
- Find the actor labeled `Floor`.
- Call `StaticMeshComponent->SetMaterial(SlotIndex, NewMaterial)` for each slot.
- After application, call `World->MarkPackageDirty()`.
- Save with `UEditorLoadingAndSavingUtils::SaveMap()`.

### 4. Failure message standards
- `SourceArt/T_Stylized_Grass_Dirt_01.png was not found.`
- `Failed to import or load /Game/Materials/T_Stylized_Grass_Dirt_01.`
- `Failed to create or load /Game/Materials/M_BasicMapFloor_StylizedGrassDirt01.`
- `Failed to load material function /Engine/Functions/Engine_MaterialFunctions03/Texturing/TextureVariation.TextureVariation.`
- `Failed to author material graph for /Game/Materials/M_BasicMapFloor_StylizedGrassDirt01.`
- `Failed to load /Game/Maps/BasicMap.`
- `Failed to find actor labeled Floor in BasicMap.`
- `Actor labeled Floor is not a StaticMeshActor.`
- `Floor StaticMeshComponent has no material slots to override.`
- `Failed to save /Game/Maps/BasicMap after applying floor material.`

## Recommended Command Example
```powershell
UnrealEditor-Cmd.exe "<ProjectDir>\\CodexUMG.uproject" -run=CodexBasicMapFloorBuild -unattended -nop4 -nosplash
```

## Verification Checklist
- Does `SourceArt/T_Stylized_Grass_Dirt_01.png` actually exist
- Is `/Game/Materials/T_Stylized_Grass_Dirt_01` imported as a real asset
- Is `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01` created as a real asset
- Is the material graph composed as `TexCoord 4x` + `TextureVariation` + `DDX/DDY derivative sampling` + `Roughness 1.0` + `Specular 0.0`
- After loading `BasicMap`, does it find the `Floor` actor correctly
- Does the `Floor` actor have at least one material slot
- Are all slots overwritten with the new material
- After saving and reopening the level, does the new material remain on `Floor`
- If the same commandlet is run again, are duplicate assets avoided
- If the PNG source is replaced and the build is rerun, do the imported texture and floor material both update
- Are no material changes made to actors other than `Floor`
- Are existing shared materials such as `M_VoxVertexColor` left untouched

## Prohibited
- keeping the solid 50% gray material as-is
- directly modifying engine content like `MI_ProcGrid` or other default materials
- changing the default material on the `SM_Template_Map_Floor` static mesh itself and causing global side effects
- assuming the first `StaticMeshActor` is `Floor` and overwriting it
- treating the build as successful even when `Floor` cannot be found
- importing the texture only through manual editor clicks and omitting the code path

## Notes
- This document is a work-instruction plan, not implementation code.
- Actual class names and file paths may be adjusted slightly to fit project rules.
- However, the standards below must remain:
  - the source input must be `SourceArt/T_Stylized_Grass_Dirt_01.png`
  - the texture and material assets must exist under the real project `Content`
  - application must target the `Floor` actor label in `BasicMap` exactly
  - the scope of application must remain limited to actor-instance overrides
  - the repeatable editor-module code + `Commandlet` path must remain intact
