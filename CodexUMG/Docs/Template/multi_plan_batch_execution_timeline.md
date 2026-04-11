# 다중 플랜 일괄 실행 타임라인 Template

## 문서 목적
- 다중 플랜 일괄 실행의 실제 수행 시기를 타임스탬프로 남기는 공식 시계열 로그 Template이다.
- 병렬 에이전트가 있으면 메인 에이전트가 각 보고를 수집해 단일 타임라인으로 중계 기록한다.
- 실제 작업을 시작할 때 이 파일을 `Docs/multi_plan_batch_execution_timeline.md`로 복사한 뒤 사용한다.

## 기록 규칙
- 모든 기록은 `YYYY-MM-DD HH:mm:ss KST` 형식을 사용한다.
- 최소 기록 대상은 착수, 완료, 차단, 차단 해제, 실행 순서 재배치, 핸드오프, 병합, 최종 검증이다.
- 공용 파일, 공용 Blueprint, 공용 에셋, 공용 `Commandlet`, 공용 에디터 모듈, 공용 맵 저장 창구 선점 및 해제 시점도 기록한다.
- 상태판의 `완료`와 동일하게, 타임라인의 완료 기록도 `batch scope 완료`를 뜻하며 `repo docs 전체 완료`를 자동 의미하지 않는다.
- 병렬 에이전트는 메인 에이전트에게 상태를 보고하고, 메인 에이전트가 본 문서에 반영한다.
- 복사 직후 첫 행부터 실제 수행 시각으로 새로 기록한다.

## 기본 Phase / Track 식별자
| ID | 구분 | 설명 |
| --- | --- | --- |
| P0 | Phase | 전체 분석 및 작업 분해 |
| A | Track | `topdown_fixed_camera_wasd_plan` |
| B | Track | `vox_mesh_asset_pipeline_plan` |
| C | Track | `interaction_umg_component_plan` |
| D | Track | `player_projectile_firing_plan` |
| E | Track | `interaction_message_popup_plan` |
| F | Track | `interaction_scroll_message_popup_plan` |
| G | Track | `interaction_dual_tile_transfer_popup_plan` |
| H | Track | `basicmap_floor_stylized_grass_dirt_material_plan` |
| P4 | Phase | 통합 및 최종 검증 |

## 공용 리소스 기록 키
| 리소스 키 | 설명 |
| --- | --- |
| `/Game/Maps/BasicMap` | Track C / E / F / G / H가 공유하는 맵 저장 창구 |
| `Interaction Asset Build BasicMap Save Window` | interaction asset builder가 `BasicMap`을 저장하는 구간 |
| `/Game/Materials/T_Stylized_Grass_Dirt_01` | floor 텍스처 애셋 |
| `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01` | floor 머터리얼 애셋 |
| `UCodexBasicMapFloorBuildCommandlet` | floor build 진입점 |
| `UCodexInteractionAssetBuildCommandlet` | interaction build 진입점 |

## 타임라인 로그
| 시각 | 기록자 | 단계/트랙 | 이벤트 | 세부 내용 | 후속 조치 |
| --- | --- | --- | --- | --- | --- |
| `<YYYY-MM-DD HH:mm:ss KST>` | 메인 에이전트 | 운영 준비 | 작업본 생성 | Template을 복사해 작업본을 만들고 초기 상태를 기록함 | 이후 실제 작업 이벤트를 순차 기록 |
