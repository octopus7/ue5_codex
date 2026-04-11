# Fixed Top-Down Camera + WASD Movement Structure Plan

## Document Purpose

Build a player-control structure that uses `WASD` movement from a non-rotating top-down camera view.
The camera composition assumes a true top-down framing where the top of the screen aligns with north, not a tilted diagonal perspective.
Input is built on `Enhanced Input`, and `IA_` / `IMC_` are created as real assets.
References to input assets are not embedded directly into the player character or player Blueprint. They are collected into a `DA_` asset, assigned to `BP_GameInstance`, and accessed by the player through that path.

This document is a draft for locking the structure before going straight into implementation code.

## Work Instructions

- If parallel processing is possible, split work across agents to reduce lead time.
- However, avoid conflict-prone parallelization that edits the same file or Blueprint at the same time.
- Do not hand asset generation off to the user through manual editor menus. Carry it through commandlet execution autonomously.
- Once all work reaches a testable state, open the project in the editor at the end.

## Core Constraints

1. `IA_` and `IMC_` must exist as real Content Browser assets.
2. The player character / player Blueprint must not hold `IA_` or `IMC_` directly as default values.
3. Real input references are collected into a `DA_` asset, and that `DA_` is assigned to `BP_GameInstance`.
4. The player queries the required input set through `GameInstance`.
5. At every connection point, use Blueprint-instantiated assets first instead of direct native-class references.
6. Asset generation is performed in an editor module, not in runtime code.
7. This editor module is for bootstrapping initial project setup, so treat it as a one-time generation tool rather than something frequently rerun.
8. The camera must not use a tilted `Yaw 45`-style diagonal composition.
9. The camera direction is fixed so that the top of the game screen matches north.
10. The player must have one additional `StaticMeshComponent` besides the skeletal mesh.
11. The default entry point for asset generation must be a directly runnable commandlet, not a user-triggered menu action.
12. Once the overall setup becomes testable, open the editor before finishing so the project is immediately ready for verification.

## Recommended Structure

### 1. Source of truth for input

`BP_GameInstance` becomes the entry point for input settings.

Reference flow:

`BP_GameInstance` -> `DA_TopDownInputConfig` -> `IMC_TopDown` / `IA_Move`

Player-side rules:

- The player reads `DA_TopDownInputConfig` from `GameInstance`.
- The player does not hardcode the `IA_Move` asset path.
- The player does not directly own `IMC_TopDown` as a default value.

### 2. Where input is applied

Because `IMC` should only be added through `Add Mapping Context` after `LocalPlayer` becomes valid, it is safer for the actual application step to live in `BP_PlayerController`.

However, split reference ownership like this:

- Ownership: `BP_GameInstance`
- Lookup: `BP_PlayerController`
- Consumption: `BP_PlayerController` or `BP_Character` that receives the input

In other words, `GameInstance` holds the input set, and the player only retrieves and applies it.

### 3. Responsibility for movement

Recommended split:

- `BP_PlayerController`
  handles `Enhanced Input` binding
- `BP_Character_TopDown`
  handles actual movement
- `BP_GameInstance`
  owns input-set references

Recommended flow:

1. `BP_PlayerController` BeginPlay
2. Acquire `DA_TopDownInputConfig` from `BP_GameInstance`
3. Add `IMC_TopDown` from the `DA_` to `EnhancedInputLocalPlayerSubsystem`
4. Bind `IA_Move` from the `DA_`
5. Forward the `Vector2D` input value to character movement logic

With this structure, the player Blueprint does not decide on its own which input assets to use.

## Camera / Movement Design

### Camera

- fixed top-down view
- no camera rotation input
- tilted isometric composition is prohibited
- the top of the screen must align with north
- `SpringArm` is fixed using `Absolute Rotation`
- `Use Pawn Control Rotation` disabled
- `Inherit Pitch/Yaw/Roll` disabled
- do not rotate the camera with mouse input

Recommended default values:

- Pitch: `-55`
- Yaw: fixed to the north-aligned direction
- Arm Length: tune to project feel

The two key requirements are:

- the camera direction never changes during play
- north always stays at the top of the screen

### Movement

- `IA_Move` is `Axis2D`
- `WASD` maps to `IA_Move` inside `IMC_TopDown`
- compute movement direction by projecting the camera-based forward/right vectors onto the plane
- because the camera stays fixed to north, the controls always feel consistent

Recommended mapping:

- `W` -> `Y +1`
- `S` -> `Y -1`
- `A` -> `X -1`
- `D` -> `X +1`

Execution rules:

- the character only moves on the plane
- whether the character auto-rotates or keeps a fixed facing can be chosen separately
- the first goal is "natural movement under a north-aligned fixed top-down camera"

## Player Visibility Rules

- `BP_Character_TopDown` includes the default `SkeletalMeshComponent` plus one additional `StaticMeshComponent`.
- This static mesh is not just a debug aid. It is treated as a real visibility aid so the player can be recognized immediately during gameplay.
- Attach the static mesh to the root or another appropriate visual reference component, and give it a size and offset that reads well from the top-down viewpoint.
- If a character VOX mesh is authored so that its visual front points along local `+Y` (the right-hand direction) instead of local `+X`, do not reauthor the mesh asset itself. Align it to the `BP_Character_TopDown` forward `+X` by applying a relative `Yaw = -90` correction on the extra player-visibility `StaticMeshComponent` or its parent `SceneComponent`.
- This axis correction is handled only in the player-visual attachment layer, and this issue alone must not trigger changes to the shared VOX axis-conversion rules or general mesh-generation rules.
- In other words, do not stop at "the character already has a skeletal mesh, so that is enough."

## Asset Layout Proposal

### Folders

- `/Game/Blueprints/Core`
- `/Game/Blueprints/GameMode`
- `/Game/Blueprints/Player`
- `/Game/Data/Input`
- `/Game/Input/Actions`
- `/Game/Input/Contexts`

### Actual assets to create

- `IA_Move`
- `IMC_TopDown`
- `DA_TopDownInputConfig`
- `BP_GI_CodexUMG`
- `BP_GM_TopDown`
- `BP_PC_TopDown`
- `BP_Character_TopDown`

Optional later additions:

- `BPI_TopDownInputConsumer`
- `BP_CameraRig_TopDown`

Required internal components of `BP_Character_TopDown`:

- `SkeletalMeshComponent`
- separate `StaticMeshComponent`

### Values held by `DA_TopDownInputConfig`

- `IMC_Default`
- `IA_Move`
- `MappingPriority`

Possible future extensions:

- `IA_Zoom`
- `IA_ClickMove`
- `IA_Confirm`
- `IA_Cancel`

For the current scope, it is better to lock down only `Move` first.

## Blueprint-First Instantiation Rule

This project follows the rule that "code classes exist mostly as parent types, while real reference wiring is finished with Blueprint assets."

Examples:

- preferred: set the project's default game instance to `BP_GI_CodexUMG`
- discouraged: point project settings directly at a native `UYourGameInstance`
- preferred: set the game mode's `Default Pawn Class` to `BP_Character_TopDown`
- discouraged: set `ACodexTopDownCharacter` directly
- preferred: let `BP_GI_CodexUMG` reference the `DA_TopDownInputConfig` asset
- discouraged: hardcode input asset paths inside a native class using `ConstructorHelpers`

In short, even if code exists, the final connection must always terminate at real assets in the Content Browser.

## Editor Module Plan

### Module name

`CodexUMGBootstrapEditor`

Why:

- the name clearly communicates that it bootstraps initial project setup
- it fits the nature of a one-time starting asset generator more than a repeatedly used tool
- responsibility stays clearly separated from runtime modules

### Module responsibilities

1. create the required folders
2. create `IA_` / `IMC_` input assets
3. create the `DA_` input configuration asset
4. create `BP_GameInstance`, `BP_GameMode`, `BP_PlayerController`, and `BP_Character`
5. wire default references using Blueprint assets
6. optionally finish project default `GameMode` and `GameInstance` settings

### Generation method

The editor module creates assets using APIs such as:

- `AssetTools`
- `BlueprintFactory`
- a factory for DataAsset creation
- project-settings writes when needed

The execution form is fixed to a `Commandlet`.

- Do not hand a manual editor menu or toolbar button to the user as the primary execution path.
- During retries, autonomously finish the commandlet implementation, invocation path, and actual execution.
- Even if a menu or toolbar entry exists, treat it only as auxiliary debugging support, not as the default path.
- Once asset generation and default setup are done and the project is testable, open the editor immediately.

## Draft Implementation Order

1. Prepare runtime base types
   Create only the parent types in code for `GameInstance`, `PlayerController`, `Character`, and the `InputConfig DataAsset`
2. Add the `CodexUMGBootstrapEditor` module
3. Add a commandlet entry point for asset generation
4. In the commandlet, create the input assets `IA_Move` and `IMC_TopDown`
5. In the commandlet, create `DA_TopDownInputConfig` and wire the input assets
6. In the commandlet, create `BP_GI_CodexUMG`, `BP_GM_TopDown`, `BP_PC_TopDown`, and `BP_Character_TopDown`
7. In the commandlet, connect references between Blueprints and set the project's default `GameInstance` and default `GameMode` to Blueprint assets
8. Execute the commandlet directly during the retry process so asset generation finishes without user intervention
9. Set up the Blueprint graph so `BP_PlayerController` applies input through `GameInstance -> DA_`
10. Build the north-aligned fixed camera and planar movement logic inside `BP_Character_TopDown`
11. Set up both the skeletal mesh and the additional `StaticMeshComponent` in `BP_Character_TopDown`, and if the character mesh is authored with visual front along local `+Y`, apply a relative `Yaw -90` correction on the extra mesh
12. Once the whole configuration reaches a testable state, open the project in the editor and move directly into verification

## Recommended Verification Items

- After running the commandlet, do `IA_Move`, `IMC_TopDown`, and `DA_TopDownInputConfig` actually exist
- Is the default project `GameInstance` `BP_GI_CodexUMG`
- Is the default gameplay flow tied to `BP_GM_TopDown`
- Does the player Blueprint avoid holding `IA_` and `IMC_` directly as default values
- After BeginPlay, does it query the input set through `GameInstance`
- Does the camera remain unrotated during play
- Is the camera north-aligned instead of tilted diagonally
- Does `WASD` produce the intended planar movement
- Is the extra static mesh visible so the player can be recognized immediately from the top-down view
- Does the visual front of the player-visibility static mesh align with the forward / movement direction of `BP_Character_TopDown`
- Once the above conditions are met and the project is testable, does the editor actually open so play verification can begin immediately

## Notes

- A realistic structure is to use `GameInstance` as the storage point for input assets, while the `PlayerController` applies the actual `MappingContext` after `LocalPlayer` is ready.
- With this approach, the input baseline data stays in `GameInstance` even if the map changes.
- If more input presets are added later, it becomes easy to extend to a different control scheme by swapping only the `DA_`.

## Decisions In The Current Draft

- camera is fixed top-down
- camera direction is fixed to north
- input uses `Enhanced Input`
- `IA_` and `IMC_` are created as real assets
- input references are collected into `DA_`
- the `DA_` is assigned to `BP_GameInstance`
- the player accesses input through `GameInstance`
- final connections are Blueprint-asset based
- the asset-generation editor module is named `CodexUMGBootstrapEditor`
- asset generation is carried through commandlet execution, not user menu clicks
- open the editor once the project reaches a testable state
- the player has an additional static mesh besides the skeletal mesh

## Things To Tune Later Based On Preference

- camera Pitch
- whether the character rotates toward movement direction
- whether movement is based on world axes or camera-projected axes
- whether to use `Pawn` instead of `Character`
- whether `DA_` stays as a single asset or expands into an input-preset bundle structure
