# Multi-Plan Batch Execution Plan

## Document Purpose
- This document organizes the dependency graph, start conditions, and parallel execution boundaries of the five plans below into a single top-level execution plan.
- It clearly separates tracks that must run sequentially from tracks that can run in parallel.
- It fixes working principles for reducing conflicts in shared files, shared Blueprints, and shared editor modules.

## Target Plans
- `Docs/topdown_fixed_camera_wasd_plan_EN.md`
- `Docs/interaction_umg_component_plan_EN.md`
- `Docs/interaction_message_popup_plan_EN.md`
- `Docs/player_projectile_firing_plan_EN.md`
- `Docs/vox_mesh_asset_pipeline_plan_EN.md`

## Supporting Operation Documents
- Working copies
  - `Docs/multi_plan_batch_execution_status_EN.md`
  - `Docs/multi_plan_batch_execution_timeline_EN.md`
- Preserved Templates
  - `Docs/Template/multi_plan_batch_execution_status_EN.md`
  - `Docs/Template/multi_plan_batch_execution_timeline_EN.md`
- Work always proceeds from the working copies in the `Docs/` root, and the preserved originals in `Docs/Template/` are copied back only when the work must be reset and restarted from the beginning.

## Top-Level Execution Guidance
- Treat "analyze everything first and use agents autonomously for parallel work to reduce lead time" as the top-level instruction of this plan.
- The main worker first analyzes all five plans, then finalizes shared edit points and responsibility boundaries.
- Keep the Phase order written in this top-level document as the baseline, but the main worker must reread the detailed procedures, start conditions, blocking conditions, and shared edit points in each sub-plan and autonomously rearrange the detailed execution order.
- Even if the detailed execution order is rearranged, prerequisite gates, shared edit protection, and the rule against parallel conflicts must not be broken.
- Use parallel execution aggressively, but do not modify the same file, the same Blueprint, the same input asset, or the same commandlet entry point at the same time.
- To prevent state loss, the main worker maintains `Docs/multi_plan_batch_execution_status_EN.md` as the single official status board and updates it immediately whenever work starts, waits, blocks, resumes, or completes.
- Actual execution timestamps by phase, handoffs, and parallel start/pause/merge moments are recorded in `Docs/multi_plan_batch_execution_timeline_EN.md` using the `YYYY-MM-DD HH:mm:ss KST` format.
- Copy the preserved originals in `Docs/Template/` back into the `Docs/` root only when work truly must be restarted from scratch.
- If parallel worker agents exist, the main worker acts as the dispatcher and traffic controller, coordinating start approval, shared file ownership, artifact delivery, and merge order.
- Immediate critical-path judgment, merges, conflict resolution, and final verification remain the responsibility of the main worker.

## Principles For Reordering Detailed Execution
- The recommended execution order in this document is a top-level guide. The actual detailed order is finalized by the main worker only after reading the sub-plan documents as well.
- Use the following criteria when deciding how to rearrange the order.
  - Whether prerequisite types, input pipelines, assets, and test gates required by the sub-documents are ready
  - The risk of conflicts in shared files, shared Blueprints, shared `Commandlet`s, and shared editor modules
  - The number of agents that can be deployed in parallel and the timing when the main worker can merge their results
  - Whether a shared foundation task should be implemented first to make later verification easier
- Preserve the prerequisite-gate nature of `topdown_fixed_camera_wasd_plan` and the independent parallel-track nature of `vox_mesh_asset_pipeline_plan`.
- The relative order of `interaction_umg_component_plan` and `player_projectile_firing_plan`, the ownership order of shared edit points, and the verification timing are decided autonomously by the main worker after reflecting the sub-plan contents.
- `interaction_message_popup_plan` is fixed as a later plan that may begin only after both `interaction_umg_component_plan` and `vox_mesh_asset_pipeline_plan` are complete.
- If the rearrangement result changes, the main worker immediately records both the reason and the changed result in the status board and timeline.

## Dependency Summary

### 1. `topdown_fixed_camera_wasd_plan` comes first
- It must run before `interaction_umg_component_plan` and `player_projectile_firing_plan`.
- The reason is that interaction and projectile structures depend on the following foundation:
  - `GameInstance`-mediated data access
  - `DA_`-based reference structure
  - the existing `IMC_` / `IA_` input pipeline
  - the base flow of player `BP_Character`, `BP_PlayerController`, and `BP_GameMode`
- Therefore, `topdown_fixed_camera_wasd_plan` is the foundation plan for later plans.
- In particular, the sub-gate for starting later plans is defined as "character movement handling complete."
  - Fixed-camera movement must actually work.
  - The existing `Enhanced Input` path must be confirmed.
  - Player-related Blueprints must be in a testable state.

### 2. `interaction_umg_component_plan` follows later
- It can begin after character movement handling in `topdown_fixed_camera_wasd_plan` is complete.
- The minimum start conditions are:
  - the player input pipeline must work using real assets
  - the `GameInstance -> DA_` access structure must be verified
  - player-related Blueprints must already be testable
- The existing `Enhanced Input` implementation must be checked first to confirm that it actually exists.
- If no existing `Enhanced Input` implementation exists, stop this plan immediately and require the user to implement that prerequisite first.
- Input expansion must strictly follow the existing `IMC_` / `IA_` / `DA_` structure.

### 3. `player_projectile_firing_plan` follows later
- It can begin after character movement handling in `topdown_fixed_camera_wasd_plan` is complete.
- The minimum start conditions are:
  - the player input pipeline must work using real assets
  - the `GameInstance -> DA_` access structure must be verified
  - player-related Blueprints must already be testable
- Do not begin full projectile firing work before these conditions are met.
- The priority relative to `interaction_umg_component_plan` is not fixed.
  - the number of currently conflicting files
  - the main worker's current ownership of shared files
  - the number of agents available for parallel deployment
  - the overall critical path
  The main worker decides autonomously based on these factors.
- If shared file conflicts can be reduced, it is acceptable to run in parallel with `interaction_umg_component_plan`.

### 4. `vox_mesh_asset_pipeline_plan` runs as a separate parallel track
- `vox_mesh_asset_pipeline_plan` runs as a separate parallel track from the two plans above.
- Because the VOX plan has no dependency on runtime player structure, it does not need to wait for `topdown_fixed_camera_wasd_plan` to finish.
- However, because it may share the `CodexUMGBootstrapEditor` module and editor automation patterns, use the conflict-avoidance rules below.
  - The VOX track owns `Vox/`, VOX-specific `Commandlet`s, and VOX material/mesh build files first.
  - The top-down, interaction, and projectile tracks own input assets, player Blueprints, game instance/game mode, and top-down bootstrap assets first.
  - The main worker must lock shared module registration files and shared type files first, or explicitly designate merge points.

### 5. `interaction_message_popup_plan` follows later
- `interaction_message_popup_plan` can start only after both of the plans below are complete:
  - `interaction_umg_component_plan`
  - `vox_mesh_asset_pipeline_plan`
- The reason is that the popup plan depends on both of the following foundations:
  - the existing interaction component and global interaction subsystem
  - the existing interaction input extension path through `IMC_` / `IA_` / `DA_`
  - the wooden-sign mesh generated by the VOX pipeline
- Therefore, the popup plan is treated as a later integration plan that reuses the completed artifacts of both the interaction track and the VOX track.
- This plan does not need to wait for `player_projectile_firing_plan`, but the main worker still decides ownership order for shared input assets and shared interaction subsystem edits.

## Recommended Execution Order

### Phase 0. Overall analysis and task breakdown
1. Read all five plan documents and identify shared dependencies, potential conflict files, detailed execution procedures inside sub-documents, and blocking conditions.
2. The main worker checks whether the working copies `Docs/multi_plan_batch_execution_status_EN.md` and `Docs/multi_plan_batch_execution_timeline_EN.md` exist, overwrites them from the preserved originals in `Docs/Template/` only when initialization is needed, and then writes the first log entry.
3. The main worker splits the work tracks as follows:
   - Track A: `topdown_fixed_camera_wasd_plan`
   - Track B: `vox_mesh_asset_pipeline_plan`
   - Track C: `interaction_umg_component_plan`
   - Track D: `player_projectile_firing_plan`
   - Track E: `interaction_message_popup_plan`
4. Fix file/BP/asset ownership per agent before implementation starts.
5. Based on sub-document analysis, autonomously rearrange the detailed order of Track C and Track D, the ownership order of shared files, and the timing of parallel starts.
6. Reserve Track E as a later integration track that starts only after Track B and Track C are complete.

### Phase 1A. Execute the top-down foundation plan
1. Run `topdown_fixed_camera_wasd_plan` first.
2. The goal is to bring the following foundations into a testable state:
   - `IA_`, `IMC_`, `DA_` input assets
   - `BP_GI_*`, `BP_GM_*`, `BP_PC_*`, `BP_Character_*`
   - fixed top-down camera
   - `WASD` movement
3. Once this phase passes the "character movement handling complete" gate, the later interaction and projectile plans may begin.
4. The main worker immediately records the gate result and actual timestamp in the status board and timeline.

### Phase 1B. Run the VOX plan in parallel
1. `vox_mesh_asset_pipeline_plan` can start immediately as a separate parallel track right after Phase 0.
2. The goal is to progress the following independently:
   - `.vox` parser
   - VOX manifest
   - `StaticMesh` generation pipeline
   - shared VOX material
   - VOX-specific commandlet
3. This track must not block the start of `interaction_umg_component_plan` or `player_projectile_firing_plan`.
4. However, if edits to a shared editor module or a shared `Commandlet` entry point become necessary, the main worker decides ownership first and records it in the timeline.

### Phase 2. Branch into the interaction/projectile follow-up plans
1. After character movement handling in `topdown_fixed_camera_wasd_plan` is complete, evaluate whether `interaction_umg_component_plan` and `player_projectile_firing_plan` are both ready to start.
2. `interaction_umg_component_plan` reuses the following foundations:
   - the existing player input structure
   - the existing `Enhanced Input` implementation
   - the `GameInstance -> DA_` reference structure
   - player and controller Blueprints
3. If the existing `Enhanced Input` implementation does not exist, stop the interaction plan immediately and require the user to implement that prerequisite first.
4. `player_projectile_firing_plan` reuses the following foundations:
   - the existing player input structure
   - the `GameInstance -> DA_` reference structure
   - player and controller Blueprints
5. The relative priority of the two plans is not fixed.
6. `interaction_umg_component_plan` and `player_projectile_firing_plan` may run in parallel agent tracks when all of the following are true:
   - ownership of shared files has already been settled
   - shared edit points such as input assets, player controller, and `GameInstance` have already been organized first
   - the same Blueprint and the same commandlet entry point are not edited simultaneously
7. Even if additional input handling is needed, both plans must extend the existing `IMC_` first.
8. The main worker decides and records in the status board which of Track C or Track D gets the shared input/controller/`GameInstance` edit window first.
9. If parallel agents are used, each agent reports status changes and handoffs to the main worker, and the main worker updates the official status board and timeline.

### Phase 3. Execute the later popup plan
1. Once both `interaction_umg_component_plan` and `vox_mesh_asset_pipeline_plan` are complete, `interaction_message_popup_plan` becomes eligible to start.
2. Track E reuses the following completed foundations:
   - Track C's interaction component, interaction subsystem, and input-extension result
   - Track B's VOX wooden-sign mesh result
3. Because Track E extends Track C's interaction system, the main worker must decide ownership order first for shared interaction subsystem files, shared input assets, and shared interaction commandlet edit windows.
4. Track E must not begin before both Track B and Track C finish.
5. `player_projectile_firing_plan` is not a prerequisite for Track E, so the main worker may place Track D and Track E either in parallel or in sequence.

### Phase 4. Integration and final verification
1. Verify the integrated results of Track C, Track D, and Track E on top of the foundation from Track A.
2. Verify Track B's VOX results separately, then confirm that the popup track references those artifacts correctly.
3. The main worker resolves final conflicts in shared modules, shared settings, shared types, and shared input assets.
4. Before the final merge, the main worker checks any `Not Started`, `Waiting`, or `Blocked` entries on the status board, then records the final verification time and merge time in the timeline.

## Status Board And Timeline Operating Rules
- Keep the official working copies `Docs/multi_plan_batch_execution_status_EN.md` and `Docs/multi_plan_batch_execution_timeline_EN.md` in the repository.
- Keep the initialization templates `Docs/Template/multi_plan_batch_execution_status_EN.md` and `Docs/Template/multi_plan_batch_execution_timeline_EN.md` separately as preserved originals.
- Copy the Template originals into the root working copies only when the working copies are corrupted or the work truly must restart from scratch.
- `Docs/multi_plan_batch_execution_status_EN.md` is the official progress board for this plan.
- The status board must use at least the states `Not Started`, `In Progress`, `Waiting`, `Blocked`, `Verifying`, and `Done`.
- The main worker updates the status board whenever work starts, pauses, resumes, completes, begins verification, becomes blocked, becomes unblocked, or the execution order is rearranged.
- `Docs/multi_plan_batch_execution_timeline_EN.md` is the chronological log. It records start times, completion times, blocked times, handoffs, merge points, and verification events for each Phase and Track.
- Times for acquiring and releasing shared files, shared Blueprints, shared `Commandlet`s, and shared editor modules are also recorded in the timeline.
- Parallel agents only report their task status to the main worker. Only the main worker updates the official status board and timeline.

## Recommended Agent Operating Model
- Main worker
  - overall analysis
  - finalizing the reordered execution order based on detailed sub-plan procedures
  - official status board and timeline updates
  - dependency judgment
  - shared file ownership assignment
  - parallel agent dispatch and traffic control
  - merge and final verification
- Agent A
  - dedicated to `topdown_fixed_camera_wasd_plan`
- Agent B
  - dedicated to `vox_mesh_asset_pipeline_plan`
- Agent C
  - dedicated to `interaction_umg_component_plan`
  - starts only after Agent A passes the character movement handling gate
- Agent D
  - dedicated to `player_projectile_firing_plan`
  - starts only after Agent A passes the character movement handling gate
- Agent E
  - dedicated to `interaction_message_popup_plan`
  - starts only after Agent C completes and Agent B completes
- Agent F
  - dedicated to final verification or review
- All parallel agents
  - report status changes, lock requests, and completion reports to the main worker
  - do not edit the official status board or timeline directly; changes go through the main worker

## Execution Principles Summary
- Leading gate: character movement handling complete in `topdown_fixed_camera_wasd_plan`
- Follow-up tracks: `interaction_umg_component_plan`, `player_projectile_firing_plan`
- The relative priority of the two follow-up tracks is not fixed and is decided autonomously by the main worker.
- The detailed execution order is autonomously rearranged by the main worker after rereading the sub-plan documents.
- Parallel track: `vox_mesh_asset_pipeline_plan`
- Conditionally parallel tracks: `interaction_umg_component_plan` + `player_projectile_firing_plan`
- Later integration track: `interaction_message_popup_plan`
- Popup-track start condition: `interaction_umg_component_plan` complete + `vox_mesh_asset_pipeline_plan` complete
- Status board working copy: `Docs/multi_plan_batch_execution_status_EN.md`
- Timeline working copy: `Docs/multi_plan_batch_execution_timeline_EN.md`
- Template originals: `Docs/Template/multi_plan_batch_execution_status_EN.md`, `Docs/Template/multi_plan_batch_execution_timeline_EN.md`
- If parallel agents exist, the main worker acts as the dispatcher and traffic controller.
- The main worker owns shared edit points first, and detailed implementation may be decomposed across agents in parallel.
- Reducing lead time is the goal, but parallel work that edits the same file and the same Blueprint at the same time is prohibited.
