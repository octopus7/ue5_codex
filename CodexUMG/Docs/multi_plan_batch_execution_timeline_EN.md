# Multi-Plan Batch Execution Timeline

## Document Purpose
- This is the official chronological log for recording the actual execution times of multi-plan batch work with timestamps.
- If parallel agents are involved, the main agent collects their reports and records them into one unified timeline.

## Logging Rules
- All records use the `YYYY-MM-DD HH:mm:ss KST` format.
- The minimum events to record are start, completion, blocked, unblocked, execution-order rearrangement, handoff, merge, and final verification.
- Times for acquiring and releasing shared files, shared Blueprints, shared assets, shared `Commandlet`s, shared editor modules, and shared map-save windows are also recorded.
- As with the status board, a completion event in this timeline means `batch scope complete`, not automatic repository-wide document completion.
- Parallel agents report status to the main agent, and the main agent reflects that status in this document.

## Default Phase / Track Keys
| ID | Type | Description |
| --- | --- | --- |
| P0 | Phase | Overall analysis and task breakdown |
| A | Track | `topdown_fixed_camera_wasd_plan` |
| B | Track | `vox_mesh_asset_pipeline_plan` |
| C | Track | `interaction_umg_component_plan` |
| D | Track | `player_projectile_firing_plan` |
| E | Track | `interaction_message_popup_plan` |
| F | Track | `interaction_scroll_message_popup_plan` |
| G | Track | `interaction_dual_tile_transfer_popup_plan` |
| H | Track | `basicmap_floor_stylized_grass_dirt_material_plan` |
| P4 | Phase | Integration and final verification |

## Shared Resource Log Keys
| Resource Key | Description |
| --- | --- |
| `/Game/Maps/BasicMap` | Shared map-save window across Tracks C / E / F / G / H |
| `Interaction Asset Build BasicMap Save Window` | Window where the interaction asset builder saves `BasicMap` |
| `/Game/Materials/T_Stylized_Grass_Dirt_01` | Floor texture asset |
| `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01` | Floor material asset |
| `UCodexBasicMapFloorBuildCommandlet` | Floor build entry point |
| `UCodexInteractionAssetBuildCommandlet` | Interaction build entry point |

## Timeline Log
| Time | Recorder | Phase/Track | Event | Details | Follow-up |
| --- | --- | --- | --- | --- | --- |
| 2026-04-11 06:59:14 KST | Main agent | Operations setup | Support document initialization | Created the status board and timeline files and prepared the base Template | Record actual work events in sequence afterward |
