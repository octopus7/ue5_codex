# Multi-Plan Batch Execution Status Board Template

## Document Purpose
- This is the official status board template for preventing state loss during multi-plan batch execution and for checking current progress, pending work, and blockers at a glance.
- Even when parallel agents are involved, only the main agent updates official status changes.
- When real work begins, copy this file to `Docs/multi_plan_batch_execution_status_EN.md` and use it from there.

## Status Definitions
- `Not Started`: work has not begun yet
- `In Progress`: the task is currently being worked on
- `Waiting`: waiting for a prerequisite gate, shared edit slot, user response, or similar dependency
- `Blocked`: cannot proceed under the current conditions
- `Verifying`: implementation is finished and verification or review is in progress
- `Done`: verification is complete

## Update Rules
- Update immediately when work starts, pauses, resumes, completes, begins verification, becomes blocked, or becomes unblocked.
- If the execution order is rearranged, update the order, status, and notes on the board together.
- The main agent also manages lock ownership of shared files, shared Blueprints, shared assets, and shared `Commandlet` entry points.
- Right after copying, replace the placeholder values in the `Last Updated` column with the real start time.

## Top-Level Execution Board
| Priority | ID | Type | Task | Owner | Prerequisite | Status | Last Updated | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | P0 | Phase | Overall analysis and task breakdown | Main agent | None | Not Started | `<First update time after copy>` | Includes reordering detailed procedures from sub-plans |
| 1 | A | Track | `topdown_fixed_camera_wasd_plan` | Agent A or main agent | P0 complete | Not Started | `<First update time after copy>` | Leading gate: character movement handling complete |
| 1 | B | Track | `vox_mesh_asset_pipeline_plan` | Agent B or main agent | P0 complete | Not Started | `<First update time after copy>` | Independent parallel track |
| 2 | C | Track | `interaction_umg_component_plan` | Agent C or main agent | Track A gate passed | Not Started | `<First update time after copy>` | Relative order versus Track D can be rearranged autonomously |
| 2 | D | Track | `player_projectile_firing_plan` | Agent D or main agent | Track A gate passed | Not Started | `<First update time after copy>` | Relative order versus Track C can be rearranged autonomously |
| 3 | P3 | Phase | Integration and final verification | Main agent or Agent E | Results from A, B, C, and D secured | Not Started | `<First update time after copy>` | Merge and final review |

## Detailed Subtask Board
| Subtask ID | Parent Task | Detailed Task | Owner | Status | Last Updated | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| TPL-001 | Common | Read the sub-plan documents and reorder detailed procedures | Main agent | Not Started | `<First update time after copy>` | Break down into detailed items when real work begins |

## Shared Edit Lock Board
| Resource | Locked By | Related Task | Status | Last Updated | Notes |
| --- | --- | --- | --- | --- | --- |
| None | - | - | Open | `<First update time after copy>` | Initial state |
