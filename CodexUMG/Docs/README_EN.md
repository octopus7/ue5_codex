# Docs README

This is an index for locating the documents in `Docs` by batch scope and document role.

## Batch Scope Summary
- `Docs/multi_plan_batch_execution_plan_EN.md` and its Korean counterpart cover the 8 official batch-included implementation plan IDs below.
  - `topdown_fixed_camera_wasd_plan`
  - `interaction_umg_component_plan`
  - `interaction_message_popup_plan`
  - `interaction_scroll_message_popup_plan`
  - `interaction_dual_tile_transfer_popup_plan`
  - `player_projectile_firing_plan`
  - `vox_mesh_asset_pipeline_plan`
  - `basicmap_floor_stylized_grass_dirt_material_plan`
- `basicmap_floor_stylized_grass_dirt_material_plan` is an official batch-included plan.
- `UIPlayground` is a separate track and is currently out of batch scope.
- `Done` inside the batch documents means `batch scope complete`. It does not mean the entire repository document set is complete.

## Implementation Plan Index
| Plan ID | Korean Doc | English Doc | Category | Notes |
| --- | --- | --- | --- | --- |
| `topdown_fixed_camera_wasd_plan` | [topdown_fixed_camera_wasd_plan.md](./topdown_fixed_camera_wasd_plan.md) | [topdown_fixed_camera_wasd_plan_EN.md](./topdown_fixed_camera_wasd_plan_EN.md) | batch included | leading gate |
| `interaction_umg_component_plan` | [interaction_umg_component_plan.md](./interaction_umg_component_plan.md) | [interaction_umg_component_plan_EN.md](./interaction_umg_component_plan_EN.md) | batch included | may save `BasicMap` |
| `interaction_message_popup_plan` | [interaction_message_popup_plan.md](./interaction_message_popup_plan.md) | [interaction_message_popup_plan_EN.md](./interaction_message_popup_plan_EN.md) | batch included | later popup track |
| `interaction_scroll_message_popup_plan` | [interaction_scroll_message_popup_plan.md](./interaction_scroll_message_popup_plan.md) | - | batch included | Korean-only plan doc, later popup track |
| `interaction_dual_tile_transfer_popup_plan` | [interaction_dual_tile_transfer_popup_plan.md](./interaction_dual_tile_transfer_popup_plan.md) | - | batch included | Korean-only plan doc, later popup track |
| `player_projectile_firing_plan` | [player_projectile_firing_plan.md](./player_projectile_firing_plan.md) | [player_projectile_firing_plan_EN.md](./player_projectile_firing_plan_EN.md) | batch included | follow-up runtime track |
| `vox_mesh_asset_pipeline_plan` | [vox_mesh_asset_pipeline_plan.md](./vox_mesh_asset_pipeline_plan.md) | [vox_mesh_asset_pipeline_plan_EN.md](./vox_mesh_asset_pipeline_plan_EN.md) | batch included | independent parallel track |
| `basicmap_floor_stylized_grass_dirt_material_plan` | [basicmap_floor_stylized_grass_dirt_material_plan.md](./basicmap_floor_stylized_grass_dirt_material_plan.md) | [basicmap_floor_stylized_grass_dirt_material_plan_EN.md](./basicmap_floor_stylized_grass_dirt_material_plan_EN.md) | batch included | official floor track, directly saves `BasicMap` |
| `interaction_ui_playground_popup_plan` | [interaction_ui_playground_popup_plan.md](./interaction_ui_playground_popup_plan.md) | - | batch excluded | separate track, currently out of batch scope |

## Support Document Index
| Document | Category | Notes |
| --- | --- | --- |
| [multi_plan_batch_execution_plan.md](./multi_plan_batch_execution_plan.md) | support document | Korean batch plan with the same official scope |
| [multi_plan_batch_execution_plan_EN.md](./multi_plan_batch_execution_plan_EN.md) | support document | English batch plan |
| [multi_plan_batch_execution_status.md](./multi_plan_batch_execution_status.md) | support document | official status board |
| [multi_plan_batch_execution_status_EN.md](./multi_plan_batch_execution_status_EN.md) | support document | English status board |
| [multi_plan_batch_execution_timeline.md](./multi_plan_batch_execution_timeline.md) | support document | official timeline |
| [multi_plan_batch_execution_timeline_EN.md](./multi_plan_batch_execution_timeline_EN.md) | support document | English timeline |
| [wbp_asset_work_guidelines.md](./wbp_asset_work_guidelines.md) | support document | Korean WBP generation/reference rules |
| [wbp_asset_work_guidelines_EN.md](./wbp_asset_work_guidelines_EN.md) | support document | English WBP generation/reference rules |
| [interaction_ui_playground_popup_implementation_checklist.md](./interaction_ui_playground_popup_implementation_checklist.md) | support document | UIPlayground checklist, currently out of batch scope |

## Learning Document Index
| Document | Category | Notes |
| --- | --- | --- |
| [Learning/dual_tile_transfer_umg_learning_guide.md](./Learning/dual_tile_transfer_umg_learning_guide.md) | learning document | dual-tile popup learning guide |
| [Learning/ui_playground_umg_learning_guide.md](./Learning/ui_playground_umg_learning_guide.md) | learning document | UIPlayground learning guide, currently out of batch scope |

## Canonical Asset Naming And Alias Rules
Use the table below when comparing generated assets or checking for missing outputs.

| Area | Canonical asset path | Accepted alias / legacy reference | Verification rule |
| --- | --- | --- | --- |
| Projectile config | `/Game/Data/Projectile/DA_PlayerProjectileConfig` | `DA_Projectile_Default` | Normalize aliases to the canonical path before comparing. |
| Projectile BP | `/Game/Blueprints/Projectile/BP_Projectile_PlayerBasic` | `BP_Projectile_Default` | Compare by full asset path, not raw filename alone. |
| Scroll popup smile icon | `/Game/UI/Interaction/T_InteractionSmileYellow` | `T_InteractionSmileIcon` | Treat `T_InteractionSmileYellow` as the canonical smile-icon asset. |
| VOX milk bottle | `/Game/Vox/Meshes/Food/SM_Vox_MilkBottle` | `SM_Vox_BottleMilk` | Normalize against the manifest's `targetPackagePath + targetAssetName`. |
| VOX grape cluster | `/Game/Vox/Meshes/Food/SM_Vox_GrapeCluster` | `SM_Vox_Grapes` | An accepted alias is not treated as a missing asset. |
| VOX bone-in roast | `/Game/Vox/Meshes/Food/SM_Vox_BoneInRoast` | `SM_Vox_BoneInRoastedMeat` | The canonical food path is `/Game/Vox/Meshes/Food`. |
| VOX water lily | `/Game/Vox/Meshes/Foliage/SM_Vox_WaterLily` | `/Game/Vox/Meshes/Ground/SM_Vox_WaterLily`, `/Game/Vox/Meshes/Props/SM_Vox_WaterLily`, `Ground` / `Props` category refs | The canonical category and path for `SM_Vox_WaterLily` are `Foliage`. |

## Comparison And Verification Rules
- Generated-asset comparison uses `package path + asset name`.
- If a document or checklist still uses a legacy alias, convert it to the canonical path before comparing.
- Do not declare `missing` or `incorrect` based only on raw filenames, category strings, or shorthand names.
- The batch documents cover only the 8 official batch-included plan IDs.
- The batch documents do not cover `UIPlayground`, support documents, learning documents, or other separate out-of-batch tracks.
