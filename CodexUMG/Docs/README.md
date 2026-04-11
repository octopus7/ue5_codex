# Docs README

`Docs` 폴더 문서를 배치 범위와 용도 기준으로 찾기 위한 인덱스다.

## 배치 범위 요약
- `Docs/multi_plan_batch_execution_plan.md`가 다루는 공식 배치 포함 구현 플랜은 아래 8개다.
  - `topdown_fixed_camera_wasd_plan`
  - `interaction_umg_component_plan`
  - `interaction_message_popup_plan`
  - `interaction_scroll_message_popup_plan`
  - `interaction_dual_tile_transfer_popup_plan`
  - `player_projectile_firing_plan`
  - `vox_mesh_asset_pipeline_plan`
  - `basicmap_floor_stylized_grass_dirt_material_plan`
- `basicmap_floor_stylized_grass_dirt_material_plan`은 공식 배치 포함 플랜이다.
- `UIPlayground`는 별도 트랙이며 현재 배치 제외다.
- 배치 문서의 `완료`는 `batch scope 완료`를 뜻한다. `Docs` 전체 문서 완료와 같은 뜻이 아니다.

## 구현 플랜 인덱스
| Plan ID | KR 문서 | EN 문서 | 분류 | 비고 |
| --- | --- | --- | --- | --- |
| `topdown_fixed_camera_wasd_plan` | [topdown_fixed_camera_wasd_plan.md](./topdown_fixed_camera_wasd_plan.md) | [topdown_fixed_camera_wasd_plan_EN.md](./topdown_fixed_camera_wasd_plan_EN.md) | batch 포함 | 선행 게이트 |
| `interaction_umg_component_plan` | [interaction_umg_component_plan.md](./interaction_umg_component_plan.md) | [interaction_umg_component_plan_EN.md](./interaction_umg_component_plan_EN.md) | batch 포함 | `BasicMap` 저장 가능 |
| `interaction_message_popup_plan` | [interaction_message_popup_plan.md](./interaction_message_popup_plan.md) | [interaction_message_popup_plan_EN.md](./interaction_message_popup_plan_EN.md) | batch 포함 | 후행 팝업 트랙 |
| `interaction_scroll_message_popup_plan` | [interaction_scroll_message_popup_plan.md](./interaction_scroll_message_popup_plan.md) | - | batch 포함 | 한글 전용 문서, 후행 팝업 트랙 |
| `interaction_dual_tile_transfer_popup_plan` | [interaction_dual_tile_transfer_popup_plan.md](./interaction_dual_tile_transfer_popup_plan.md) | - | batch 포함 | 한글 전용 문서, 후행 팝업 트랙 |
| `player_projectile_firing_plan` | [player_projectile_firing_plan.md](./player_projectile_firing_plan.md) | [player_projectile_firing_plan_EN.md](./player_projectile_firing_plan_EN.md) | batch 포함 | 런타임 후속 트랙 |
| `vox_mesh_asset_pipeline_plan` | [vox_mesh_asset_pipeline_plan.md](./vox_mesh_asset_pipeline_plan.md) | [vox_mesh_asset_pipeline_plan_EN.md](./vox_mesh_asset_pipeline_plan_EN.md) | batch 포함 | 독립 병렬 트랙 |
| `basicmap_floor_stylized_grass_dirt_material_plan` | [basicmap_floor_stylized_grass_dirt_material_plan.md](./basicmap_floor_stylized_grass_dirt_material_plan.md) | [basicmap_floor_stylized_grass_dirt_material_plan_EN.md](./basicmap_floor_stylized_grass_dirt_material_plan_EN.md) | batch 포함 | 공식 floor 트랙, `BasicMap` 직접 저장 |
| `interaction_ui_playground_popup_plan` | [interaction_ui_playground_popup_plan.md](./interaction_ui_playground_popup_plan.md) | - | batch 제외 | 별도 track, 현재 배치 제외 |

## 보조 문서 인덱스
| 문서 | 분류 | 비고 |
| --- | --- | --- |
| [multi_plan_batch_execution_plan.md](./multi_plan_batch_execution_plan.md) | 보조 문서 | 공식 배치 범위, 트랙, 충돌 규칙 |
| [multi_plan_batch_execution_plan_EN.md](./multi_plan_batch_execution_plan_EN.md) | 보조 문서 | 영문 배치 계획 문서 |
| [multi_plan_batch_execution_status.md](./multi_plan_batch_execution_status.md) | 보조 문서 | 공식 상태판 |
| [multi_plan_batch_execution_status_EN.md](./multi_plan_batch_execution_status_EN.md) | 보조 문서 | 영문 상태판 |
| [multi_plan_batch_execution_timeline.md](./multi_plan_batch_execution_timeline.md) | 보조 문서 | 공식 타임라인 |
| [multi_plan_batch_execution_timeline_EN.md](./multi_plan_batch_execution_timeline_EN.md) | 보조 문서 | 영문 타임라인 |
| [wbp_asset_work_guidelines.md](./wbp_asset_work_guidelines.md) | 보조 문서 | WBP 생성/참조 규칙 |
| [wbp_asset_work_guidelines_EN.md](./wbp_asset_work_guidelines_EN.md) | 보조 문서 | 영문 WBP 규칙 |
| [interaction_ui_playground_popup_implementation_checklist.md](./interaction_ui_playground_popup_implementation_checklist.md) | 보조 문서 | UIPlayground 구현 체크리스트, 현재 배치 제외 |

## 학습 문서 인덱스
| 문서 | 분류 | 비고 |
| --- | --- | --- |
| [Learning/dual_tile_transfer_umg_learning_guide.md](./Learning/dual_tile_transfer_umg_learning_guide.md) | 학습 문서 | 듀얼 타일 팝업 학습용 문서 |
| [Learning/ui_playground_umg_learning_guide.md](./Learning/ui_playground_umg_learning_guide.md) | 학습 문서 | UIPlayground 학습용 문서, 현재 배치 제외 |

## canonical asset naming / alias 규칙
아래 표는 생성 애셋 비교와 검증 시 기준이 되는 canonical 이름과 허용 alias를 정리한 것이다.

| 영역 | canonical asset path | 허용 alias / legacy ref | 검증 규칙 |
| --- | --- | --- | --- |
| Projectile config | `/Game/Data/Projectile/DA_PlayerProjectileConfig` | `DA_Projectile_Default` | alias를 canonical path로 정규화한 뒤 비교한다. |
| Projectile BP | `/Game/Blueprints/Projectile/BP_Projectile_PlayerBasic` | `BP_Projectile_Default` | raw filename만 보지 말고 asset path 기준으로 비교한다. |
| Scroll popup smile icon | `/Game/UI/Interaction/T_InteractionSmileYellow` | `T_InteractionSmileIcon` | 노란 스마일 아이콘은 `T_InteractionSmileYellow`를 canonical로 본다. |
| VOX milk bottle | `/Game/Vox/Meshes/Food/SM_Vox_MilkBottle` | `SM_Vox_BottleMilk` | VOX food는 manifest의 `targetPackagePath + targetAssetName`을 기준으로 정규화한다. |
| VOX grape cluster | `/Game/Vox/Meshes/Food/SM_Vox_GrapeCluster` | `SM_Vox_Grapes` | alias만 다르면 누락으로 보지 않는다. |
| VOX bone-in roast | `/Game/Vox/Meshes/Food/SM_Vox_BoneInRoast` | `SM_Vox_BoneInRoastedMeat` | canonical food path는 `/Game/Vox/Meshes/Food`다. |
| VOX water lily | `/Game/Vox/Meshes/Foliage/SM_Vox_WaterLily` | `/Game/Vox/Meshes/Ground/SM_Vox_WaterLily`, `/Game/Vox/Meshes/Props/SM_Vox_WaterLily`, `Ground`/`Props` category ref | `SM_Vox_WaterLily`의 canonical category/path는 `Foliage`다. |

## 비교 / 검증 규칙
- 생성 애셋 비교는 `package path + asset name`을 기준으로 한다.
- 문서나 체크리스트에 legacy alias가 남아 있으면 canonical path로 변환한 뒤 비교한다.
- raw filename, category 문자열, 축약 이름만으로 `누락` 또는 `오구현` 판정을 내리지 않는다.
- batch 문서가 커버하는 범위는 공식 batch 포함 플랜 8개다.
- batch 문서가 커버하지 않는 범위는 `UIPlayground`, 보조 문서, 학습 문서, 그리고 batch 밖 별도 트랙이다.
