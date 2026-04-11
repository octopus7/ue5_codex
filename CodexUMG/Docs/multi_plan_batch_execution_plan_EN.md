# Multi-Plan Batch Execution Plan

## Document Purpose
- This document defines the top-level execution plan for the official batch-included implementation plans in the repository `Docs` root.
- It explicitly includes `basicmap_floor_stylized_grass_dirt_material_plan` as an official batch target and fixes the `BasicMap` save-conflict rules that otherwise create false-complete states.
- It separates `batch scope complete` from `repo docs fully complete` so the batch documents cannot be misread as meaning that every document in the repository is done.

## Batch Scope Definitions
- `batch scope complete`
  - Every plan explicitly included by this document has finished and passed batch-level rerun verification.
  - This does not automatically include `UIPlayground`, support documents, or learning documents.
- `repo docs fully complete`
  - All implementation plans in `Docs`, including batch-excluded tracks and non-batch support material, have been reviewed under their own criteria.
  - A `Done` state in this batch plan must never be interpreted as this broader repository-wide state.
- `UIPlayground`
  - Excluded from the current batch.
  - If it must be mentioned, treat it only as a `separate track` or `currently out of batch scope`.

## Official Batch Plan IDs
| Track | Plan ID | Documents | Notes |
| --- | --- | --- | --- |
| A | `topdown_fixed_camera_wasd_plan` | `Docs/topdown_fixed_camera_wasd_plan.md`, `Docs/topdown_fixed_camera_wasd_plan_EN.md` | leading gate |
| B | `vox_mesh_asset_pipeline_plan` | `Docs/vox_mesh_asset_pipeline_plan.md`, `Docs/vox_mesh_asset_pipeline_plan_EN.md` | independent parallel track |
| C | `interaction_umg_component_plan` | `Docs/interaction_umg_component_plan.md`, `Docs/interaction_umg_component_plan_EN.md` | may save `BasicMap` |
| D | `player_projectile_firing_plan` | `Docs/player_projectile_firing_plan.md`, `Docs/player_projectile_firing_plan_EN.md` | follow-up runtime track |
| E | `interaction_message_popup_plan` | `Docs/interaction_message_popup_plan.md`, `Docs/interaction_message_popup_plan_EN.md` | later popup track, may save `BasicMap` |
| F | `interaction_scroll_message_popup_plan` | `Docs/interaction_scroll_message_popup_plan.md` | later popup track, Korean-only plan doc, may save `BasicMap` |
| G | `interaction_dual_tile_transfer_popup_plan` | `Docs/interaction_dual_tile_transfer_popup_plan.md` | later popup track, Korean-only plan doc, may save `BasicMap` |
| H | `basicmap_floor_stylized_grass_dirt_material_plan` | `Docs/basicmap_floor_stylized_grass_dirt_material_plan.md`, `Docs/basicmap_floor_stylized_grass_dirt_material_plan_EN.md` | official floor track, directly saves `BasicMap` |

## Supporting Operation Documents
- Working copies
  - `Docs/multi_plan_batch_execution_status_EN.md`
  - `Docs/multi_plan_batch_execution_timeline_EN.md`
- Preserved Templates
  - `Docs/Template/multi_plan_batch_execution_status_EN.md`
  - `Docs/Template/multi_plan_batch_execution_timeline_EN.md`
- Work always proceeds from the working copies in the `Docs/` root, and the preserved originals in `Docs/Template/` are copied back only when the work truly must be reset and restarted from scratch.

## Shared Resource Serialization Rules

### 1. The `BasicMap` save window is an exclusive lock
- The tracks below can all modify or save `/Game/Maps/BasicMap`.
  - Track C `interaction_umg_component_plan`
  - Track E `interaction_message_popup_plan`
  - Track F `interaction_scroll_message_popup_plan`
  - Track G `interaction_dual_tile_transfer_popup_plan`
  - Track H `basicmap_floor_stylized_grass_dirt_material_plan`
- Therefore those tracks must not run at the same time during their real map-load / map-edit / map-save sections.
- In particular, the floor track directly updates and saves the `Floor` actor inside `BasicMap`, while the interaction asset build family can also save the same map during test-actor placement.
- Parallel document work or read-only review is fine, but the actual `BasicMap` write window must be serialized.

### 2. Floor-track lock targets
- Track H manages the resources below as shared lock items.
  - `/Game/Maps/BasicMap`
  - `/Game/Materials/T_Stylized_Grass_Dirt_01`
  - `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01`
  - the `UCodexBasicMapFloorBuildCommandlet` entry point
- The main worker must ensure that the interaction asset build path and the floor build path never hold the same `BasicMap` save window at the same time.

### 3. Canonical-name comparison rule
- Generated-asset comparison and verification must use canonical asset paths plus alias normalization, not raw filename-only comparison.
- The canonical and alias rules for projectile assets, the smile icon texture, VOX food assets, and `SM_Vox_WaterLily` live in `Docs/README_EN.md`.
- If two names differ only by an accepted alias mapping, the result is not treated as a missing asset. Normalize first, then compare.

## Top-Level Execution Guidance
- Read the full batch scope first, then fix shared edit points and ownership boundaries before placing parallel work.
- The Phase order in this document is the baseline, but the main worker may reread each detailed sub-plan and rearrange the fine-grained execution order.
- However, prerequisite gates and shared-resource protection rules must remain intact.
- Do not modify the same file, the same Blueprint, the same input asset, or the same commandlet entry point at the same time.
- The main worker maintains `Docs/multi_plan_batch_execution_status_EN.md` as the official status board and updates it immediately whenever work starts, waits, blocks, resumes, or completes.
- Actual execution timestamps, handoffs, and parallel start / stop / merge moments are recorded in `Docs/multi_plan_batch_execution_timeline_EN.md` using the `YYYY-MM-DD HH:mm:ss KST` format.
- Do not request user-facing mid-implementation validation before the whole batch reaches the final rerun-verification stage.
- Internal checks during the middle of the batch are allowed, but user-facing verification belongs only to the final complete batch state unless a real blocker forces otherwise.

## Principles For Reordering Detailed Execution
- First verify that the prerequisite input pipelines, assets, and test gates required by each sub-plan are ready.
- Evaluate conflicts in shared files, shared Blueprints, shared `Commandlet`s, shared editor modules, and shared map-save windows before placing parallel work.
- Consider how many agents can work in parallel and when the main worker can merge their outputs.
- If there is a shared foundation task that makes later verification easier, do that first.
- Preserve the prerequisite-gate nature of `topdown_fixed_camera_wasd_plan` and the independent parallel-track nature of `vox_mesh_asset_pipeline_plan`.
- Treat `interaction_message_popup_plan`, `interaction_scroll_message_popup_plan`, and `interaction_dual_tile_transfer_popup_plan` as independent plan units, but serialize their interaction-asset-build and `BasicMap` save sections.
- Treat `basicmap_floor_stylized_grass_dirt_material_plan` as an official batch track, but by default place it as a late map-write track to avoid `BasicMap` save conflicts.

## Dependency Summary

### 1. `topdown_fixed_camera_wasd_plan` comes first
- It must run before `interaction_umg_component_plan` and `player_projectile_firing_plan`.
- The reason is that interaction and projectile structures depend on the following foundation.
  - `GameInstance`-mediated data access
  - `DA_`-based reference structure
  - the existing `IMC_` / `IA_` input pipeline
  - the base flow of player `BP_Character`, `BP_PlayerController`, and `BP_GameMode`
- The start gate for later runtime plans is `character movement handling complete`.

### 2. `interaction_umg_component_plan` follows later
- It may begin after character movement handling in `topdown_fixed_camera_wasd_plan` is complete.
- The minimum start conditions are:
  - the player input pipeline works with real assets
  - the `GameInstance -> DA_` access structure is verified
  - player-related Blueprints are already testable
- If the existing `Enhanced Input` implementation does not exist, stop this plan immediately and require that prerequisite first.
- This track may save `BasicMap`, so it is part of the map-save lock set.

### 3. `player_projectile_firing_plan` follows later
- It may begin after character movement handling in `topdown_fixed_camera_wasd_plan` is complete.
- The minimum start conditions are:
  - the player input pipeline works with real assets
  - the `GameInstance -> DA_` access structure is verified
  - player-related Blueprints are already testable
- The relative priority versus `interaction_umg_component_plan` is not fixed.

### 4. `vox_mesh_asset_pipeline_plan` runs as an independent parallel track
- `vox_mesh_asset_pipeline_plan` runs separately from the runtime-player tracks above.
- It does not need to wait for `topdown_fixed_camera_wasd_plan`.
- However, it may still share `CodexUMGBootstrapEditor` and editor-automation patterns, so the main worker decides shared-file ownership first.

### 5. `interaction_message_popup_plan`, `interaction_scroll_message_popup_plan`, and `interaction_dual_tile_transfer_popup_plan` are later popup tracks
- These tracks may begin only after both of the plans below are complete:
  - `interaction_umg_component_plan`
  - `vox_mesh_asset_pipeline_plan`
- They depend on the following shared foundations:
  - the existing interaction component and global interaction subsystem
  - the existing interaction input-extension path through `IMC_` / `IA_` / `DA_`
  - the wooden-sign mesh generated by the VOX pipeline
- Tracks E, F, and G are independent at the plan level, but their interaction asset build windows and `BasicMap` save windows must be serialized.

### 6. `basicmap_floor_stylized_grass_dirt_material_plan` is the official floor track
- Track H is mandatory batch scope, not an optional side task.
- Its minimum prerequisites are:
  - `SourceArt/T_Stylized_Grass_Dirt_01.png` exists
  - `/Game/Maps/BasicMap` exists
  - the current project editor is closed
  - the `BasicMap` lock is free and the interaction asset build path is not holding the same map-save window
- The recommended placement is a dedicated late map-write phase after the last interaction track that may save `BasicMap`, and before final integration verification.
- Top-down and projectile completion are not hard prerequisites for the floor task itself, but in batch operation the `BasicMap` save lock takes precedence over earlier parallel placement.

## Recommended Execution Order

### Phase 0. Overall analysis and task breakdown
1. Read all 8 official batch plan documents and identify shared dependencies, potential conflict files, blocking conditions, and real map-write windows.
2. Check whether `Docs/multi_plan_batch_execution_status_EN.md` and `Docs/multi_plan_batch_execution_timeline_EN.md` already exist, and initialize them from `Docs/Template/` only when needed.
3. Split the work into the tracks below.
   - Track A: `topdown_fixed_camera_wasd_plan`
   - Track B: `vox_mesh_asset_pipeline_plan`
   - Track C: `interaction_umg_component_plan`
   - Track D: `player_projectile_firing_plan`
   - Track E: `interaction_message_popup_plan`
   - Track F: `interaction_scroll_message_popup_plan`
   - Track G: `interaction_dual_tile_transfer_popup_plan`
   - Track H: `basicmap_floor_stylized_grass_dirt_material_plan`
4. Fix ownership of shared files, Blueprints, assets, and the `BasicMap` save window before implementation starts.
5. Autonomously rearrange the detailed order of Tracks C and D and the order of shared edit windows.
6. Reserve Tracks E, F, and G as later popup tracks after Tracks B and C.
7. Reserve Track H as the late `BasicMap` floor track and keep its real start time tied to the `BasicMap` lock state.

### Phase 1A. Execute the top-down foundation plan
1. Run `topdown_fixed_camera_wasd_plan` first.
2. The goal is to bring the following foundations into a testable state:
   - `IA_`, `IMC_`, `DA_` input assets
   - `BP_GI_*`, `BP_GM_*`, `BP_PC_*`, `BP_Character_*`
   - fixed top-down camera
   - `WASD` movement
3. Once the `character movement handling complete` gate passes, later interaction and projectile tracks may begin.
4. Record the gate result immediately in the status board and timeline.

### Phase 1B. Run the VOX plan in parallel
1. `vox_mesh_asset_pipeline_plan` may start immediately after Phase 0 as a separate parallel track.
2. The goal is to progress the following independently:
   - `.vox` parser
   - VOX manifest
   - `StaticMesh` generation pipeline
   - shared VOX material
   - VOX-specific commandlet
3. This track must not block the start of `interaction_umg_component_plan` or `player_projectile_firing_plan`.
4. If edits to a shared editor module or a shared commandlet entry point become necessary, the main worker fixes ownership first.

### Phase 2. Branch into the interaction and projectile follow-up tracks
1. After character movement handling in `topdown_fixed_camera_wasd_plan` is complete, evaluate whether `interaction_umg_component_plan` and `player_projectile_firing_plan` are both ready to start.
2. `interaction_umg_component_plan` reuses the existing input structure, `Enhanced Input`, `GameInstance -> DA_`, and player/controller Blueprints.
3. If the existing `Enhanced Input` implementation does not exist, stop the interaction plan immediately.
4. `player_projectile_firing_plan` also reuses the existing input structure, `GameInstance -> DA_`, and player/controller Blueprints.
5. The relative priority of Tracks C and D is not fixed.
6. Tracks C and D may run in parallel only when ownership of shared files and shared edit windows has already been settled.
7. When Track C enters a real `BasicMap` save section, it must not overlap that window with Tracks E, F, G, or H.

### Phase 3. Execute the later popup tracks
1. Once both `interaction_umg_component_plan` and `vox_mesh_asset_pipeline_plan` are complete, Tracks E, F, and G become eligible to start.
2. Track E is the base message-popup track.
3. Track F is the scroll-message-popup track.
4. Track G is the dual-tile-transfer popup track.
5. Tracks E, F, and G are independent at the plan level and may run in parallel where safe.
6. However, their interaction-asset-build windows and `BasicMap` save windows are serialized by the main worker.

### Phase 3B. Execute the floor late map-write track
1. Track H `basicmap_floor_stylized_grass_dirt_material_plan` is official batch scope and must finish before Phase 4.
2. The default recommendation is to run Track H only after the last interaction track that may save `BasicMap` has completed its map-write section.
3. Right before Track H starts, the main worker records lock ownership for:
   - `/Game/Maps/BasicMap`
   - `/Game/Materials/T_Stylized_Grass_Dirt_01`
   - `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01`
   - `UCodexBasicMapFloorBuildCommandlet`
4. Because Track H directly edits and saves the `Floor` actor inside `BasicMap`, it must never run in parallel with the `BasicMap` save sections of Tracks C, E, F, or G.
5. After Track H finishes, freeze the floor-texture, floor-material, and `BasicMap` results into a verification-ready state.

### Phase 4. Integration and final verification
1. Verify the integrated results of Tracks C, D, E, F, G, and H on top of the foundation from Track A.
2. Verify Track B's VOX results separately and confirm that the popup tracks reference those artifacts correctly.
3. Verify that Track H's floor texture, floor material, and `BasicMap` floor override persist after reopening the map.
4. Resolve final conflicts in shared modules, shared settings, shared types, shared input assets, and shared map-save windows.
5. Before the final merge, confirm that no `Not Started`, `Waiting`, or `Blocked` entries remain on the status board, then record the final verification and merge times in the timeline.

## Status Board And Timeline Operating Rules
- Keep the official working copies `Docs/multi_plan_batch_execution_status_EN.md` and `Docs/multi_plan_batch_execution_timeline_EN.md` in the repository.
- Keep the initialization templates `Docs/Template/multi_plan_batch_execution_status_EN.md` and `Docs/Template/multi_plan_batch_execution_timeline_EN.md` separately as preserved originals.
- Copy the templates into the working copies only when the working copies are corrupted or the work truly must restart from scratch.
- The status board must use at least `Not Started`, `In Progress`, `Waiting`, `Blocked`, `Verifying`, and `Done`.
- A `Done` state means the track or phase is complete within this batch scope. It does not mean the full repository document set is complete.
- The main worker updates the status board whenever work starts, pauses, resumes, completes, begins verification, becomes blocked, becomes unblocked, or the execution order changes.
- The timeline records start, completion, blocked, handoff, merge, verification, and lock acquire / release moments for shared files, shared Blueprints, shared `Commandlet`s, shared editor modules, the `BasicMap` save window, and floor-specific assets.
- `Verifying` means batch-level or final rerun verification, not a casual mid-implementation check.

## Recommended Agent Operating Model
- Main worker
  - overall analysis
  - finalizing the reordered execution order
  - official status-board and timeline updates
  - dependency judgment
  - shared-file and `BasicMap` lock ownership
  - agent dispatch and traffic control
  - merge and final verification
- Agent A
  - `topdown_fixed_camera_wasd_plan`
- Agent B
  - `vox_mesh_asset_pipeline_plan`
- Agent C
  - `interaction_umg_component_plan`
- Agent D
  - `player_projectile_firing_plan`
- Agent E
  - `interaction_message_popup_plan`
- Agent F
  - `interaction_scroll_message_popup_plan`
- Agent G
  - `interaction_dual_tile_transfer_popup_plan`
- Agent H
  - `basicmap_floor_stylized_grass_dirt_material_plan`
- Agent I
  - final verification or review

## Execution Principles Summary
- Leading gate: `character movement handling complete` in `topdown_fixed_camera_wasd_plan`
- Follow-up runtime tracks: `interaction_umg_component_plan`, `player_projectile_firing_plan`
- Independent parallel track: `vox_mesh_asset_pipeline_plan`
- Later popup tracks: `interaction_message_popup_plan`, `interaction_scroll_message_popup_plan`, `interaction_dual_tile_transfer_popup_plan`
- Official floor track: `basicmap_floor_stylized_grass_dirt_material_plan`
- The floor track is explicitly included in this batch scope.
- Tracks C, E, F, G, and H are all treated as potential `BasicMap` writers, and their real map-save windows are serialized.
- `Done` on the status board means `batch scope complete`, not `repo docs fully complete`.
- `UIPlayground` remains a separate track and is currently out of this batch scope.
