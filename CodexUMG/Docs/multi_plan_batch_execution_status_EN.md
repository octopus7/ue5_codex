# Multi-Plan Batch Execution Status Board

## Document Purpose
- This is the official status board for preventing state loss during multi-plan batch execution and for checking current progress, pending work, and blockers at a glance.
- Even when parallel agents are involved, only the main agent updates official state changes and shared-lock ownership.

## Status Definitions
- `Not Started`: work has not begun yet
- `In Progress`: the task is currently being worked on
- `Waiting`: waiting for a prerequisite gate, shared edit slot, user response, or similar dependency
- `Blocked`: cannot proceed under the current conditions
- `Verifying`: implementation is finished and batch-scope verification or review is in progress
- `Done`: the track or phase has completed within this batch scope
- `Done` does not mean the full repository document set is complete.

## Update Rules
- Update immediately when work starts, pauses, resumes, completes, begins verification, becomes blocked, or becomes unblocked.
- If the execution order is rearranged, update the order, status, and notes on the board together.
- The main agent also manages lock ownership of shared files, shared Blueprints, shared assets, shared `Commandlet`s, and shared map-save windows.
- Even if all official batch tracks are marked `Done`, do not claim repository-wide document completion until the batch-excluded, support, and learning documents listed in `Docs/README_EN.md` have been checked separately.

## Top-Level Execution Board
| Priority | ID | Type | Task | Owner | Prerequisite | Status | Last Updated | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | P0 | Phase | Overall analysis and task breakdown | Main agent | None | Not Started | 2026-04-11 06:59:14 KST | Includes reordering sub-plan procedures |
| 1 | A | Track | `topdown_fixed_camera_wasd_plan` | Agent A or main agent | P0 complete | Not Started | 2026-04-11 06:59:14 KST | Leading gate: character movement handling complete |
| 1 | B | Track | `vox_mesh_asset_pipeline_plan` | Agent B or main agent | P0 complete | Not Started | 2026-04-11 06:59:14 KST | Independent parallel track |
| 2 | C | Track | `interaction_umg_component_plan` | Agent C or main agent | Track A gate passed | Not Started | 2026-04-11 06:59:14 KST | May save `BasicMap` |
| 2 | D | Track | `player_projectile_firing_plan` | Agent D or main agent | Track A gate passed | Not Started | 2026-04-11 06:59:14 KST | Relative order versus Track C can be rearranged |
| 3 | E | Track | `interaction_message_popup_plan` | Agent E or main agent | Tracks B and C complete | Not Started | 2026-04-11 06:59:14 KST | Later popup track, may save `BasicMap` |
| 3 | F | Track | `interaction_scroll_message_popup_plan` | Agent F or main agent | Tracks B and C complete | Not Started | 2026-04-11 06:59:14 KST | Later popup track, Korean-only plan doc, may save `BasicMap` |
| 3 | G | Track | `interaction_dual_tile_transfer_popup_plan` | Agent G or main agent | Tracks B and C complete | Not Started | 2026-04-11 06:59:14 KST | Later popup track, may save `BasicMap` |
| 4 | H | Track | `basicmap_floor_stylized_grass_dirt_material_plan` | Agent H or main agent | `BasicMap` lock free and floor prerequisites satisfied | Not Started | 2026-04-11 06:59:14 KST | Official floor track, recommended late map-write phase |
| 5 | P4 | Phase | Integration and final verification | Main agent or Agent I | Results from A through H secured | Not Started | 2026-04-11 06:59:14 KST | Final batch-scope verification |

## Detailed Subtask Board
| Subtask ID | Parent Task | Detailed Task | Owner | Status | Last Updated | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| TPL-001 | Common | Read all 8 official batch plan documents and reorder detailed procedures | Main agent | Not Started | 2026-04-11 06:59:14 KST | Explicitly confirm floor-track inclusion |
| TPL-002 | Common | Finalize the `BasicMap` save-conflict rules and floor-lock rules | Main agent | Not Started | 2026-04-11 06:59:14 KST | Tracks C / E / F / G / H are serialized at map-save time |
| TPL-003 | Common | Confirm the canonical asset naming and alias rules | Main agent | Not Started | 2026-04-11 06:59:14 KST | Normalize against `Docs/README_EN.md` |

## Shared Edit Lock Board
| Resource | Locked By | Related Task | Status | Last Updated | Notes |
| --- | --- | --- | --- | --- | --- |
| `/Game/Maps/BasicMap` | - | Tracks C / E / F / G / H | Open | 2026-04-11 06:59:14 KST | Real map load / edit / save windows are serialized |
| `Interaction Asset Build BasicMap Save Window` | - | Tracks C / E / F / G | Open | 2026-04-11 06:59:14 KST | Held when the interaction asset builder saves `BasicMap` |
| `/Game/Materials/T_Stylized_Grass_Dirt_01` | - | Track H | Open | 2026-04-11 06:59:14 KST | Floor texture asset |
| `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01` | - | Track H | Open | 2026-04-11 06:59:14 KST | Floor material asset |
| `UCodexBasicMapFloorBuildCommandlet` | - | Track H | Open | 2026-04-11 06:59:14 KST | Floor build entry point |
| `UCodexInteractionAssetBuildCommandlet` | - | Tracks C / E / F / G | Open | 2026-04-11 06:59:14 KST | Interaction build entry point |
