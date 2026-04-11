# 다중 플랜 일괄 실행 상태판

## 문서 목적
- 다중 플랜 일괄 실행 중 상태 유실을 막고, 현재 진행/미진행/차단 상태를 한눈에 확인하기 위한 공식 상태판이다.
- 병렬 에이전트가 있더라도 공식 상태 변경과 공용 잠금 관리는 메인 에이전트만 반영한다.

## 상태 값 정의
- `미착수`: 아직 시작하지 않음
- `진행중`: 현재 작업 수행 중
- `대기`: 선행 게이트, 공용 수정 지점 해제, 사용자 응답 등을 기다리는 상태
- `차단`: 현재 조건으로 진행 불가
- `검증중`: 구현이 끝났고 batch scope 기준 검증 또는 리뷰 중
- `완료`: 해당 Track 또는 Phase가 이 배치 범위에서 검증까지 종료됨
- `완료`는 `repo docs 전체 완료`를 의미하지 않는다.

## 갱신 규칙
- 작업 시작, 중단, 재개, 완료, 검증 시작, 차단 발생, 차단 해제 시 즉시 갱신한다.
- 실행 순서를 재배치하면 상태판의 순서, 상태, 비고를 함께 갱신한다.
- 공용 파일, 공용 Blueprint, 공용 에셋, 공용 `Commandlet`, 공용 맵 저장 창구 선점 현황도 메인 에이전트가 관리한다.
- 상태판에서 모든 공식 배치 대상이 `완료`여도, `Docs/README.md`의 배치 제외/보조/학습 문서를 확인하기 전까지 `repo docs 전체 완료`로 선언하지 않는다.

## 상위 실행 보드
| 우선순위 | ID | 구분 | 작업 | 담당 | 선행 조건 | 상태 | 마지막 갱신 | 비고 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | P0 | Phase | 전체 분석 및 작업 분해 | 메인 에이전트 | 없음 | 미착수 | 2026-04-11 06:59:14 KST | 하위 플랜 세부 절차 재배치 포함 |
| 1 | A | Track | `topdown_fixed_camera_wasd_plan` | Agent A 또는 메인 에이전트 | P0 완료 | 미착수 | 2026-04-11 06:59:14 KST | 선행 게이트: 캐릭터 이동 처리 완료 |
| 1 | B | Track | `vox_mesh_asset_pipeline_plan` | Agent B 또는 메인 에이전트 | P0 완료 | 미착수 | 2026-04-11 06:59:14 KST | 독립 병렬 트랙 |
| 2 | C | Track | `interaction_umg_component_plan` | Agent C 또는 메인 에이전트 | Track A 게이트 통과 | 미착수 | 2026-04-11 06:59:14 KST | `BasicMap` 저장 가능 |
| 2 | D | Track | `player_projectile_firing_plan` | Agent D 또는 메인 에이전트 | Track A 게이트 통과 | 미착수 | 2026-04-11 06:59:14 KST | Track C와 상대 순서는 자율 재배치 |
| 3 | E | Track | `interaction_message_popup_plan` | Agent E 또는 메인 에이전트 | Track B, C 완료 | 미착수 | 2026-04-11 06:59:14 KST | 후행 팝업 트랙, `BasicMap` 저장 가능 |
| 3 | F | Track | `interaction_scroll_message_popup_plan` | Agent F 또는 메인 에이전트 | Track B, C 완료 | 미착수 | 2026-04-11 06:59:14 KST | 후행 팝업 트랙, 한글 전용 문서, `BasicMap` 저장 가능 |
| 3 | G | Track | `interaction_dual_tile_transfer_popup_plan` | Agent G 또는 메인 에이전트 | Track B, C 완료 | 미착수 | 2026-04-11 06:59:14 KST | 후행 팝업 트랙, `BasicMap` 저장 가능 |
| 4 | H | Track | `basicmap_floor_stylized_grass_dirt_material_plan` | Agent H 또는 메인 에이전트 | `BasicMap` 잠금 해제, floor 선행 조건 충족 | 미착수 | 2026-04-11 06:59:14 KST | 공식 floor 트랙, 권장 위치: late map-write phase |
| 5 | P4 | Phase | 통합 및 최종 검증 | 메인 에이전트 또는 Agent I | A~H 결과 확보 | 미착수 | 2026-04-11 06:59:14 KST | batch scope 최종 검증 |

## 세부 서브태스크 보드
| 서브태스크 ID | 상위 작업 | 세부 작업 | 담당 | 상태 | 마지막 갱신 | 비고 |
| --- | --- | --- | --- | --- | --- | --- |
| TPL-001 | 공통 | 공식 배치 대상 8개 플랜을 읽고 세부 절차를 재배치한다 | 메인 에이전트 | 미착수 | 2026-04-11 06:59:14 KST | floor 트랙 포함 여부를 명시 확인 |
| TPL-002 | 공통 | `BasicMap` 저장 충돌 규칙과 floor 잠금 규칙을 확정한다 | 메인 에이전트 | 미착수 | 2026-04-11 06:59:14 KST | Track C/E/F/G/H 직렬화 규칙 |
| TPL-003 | 공통 | canonical asset naming / alias 규칙을 확인한다 | 메인 에이전트 | 미착수 | 2026-04-11 06:59:14 KST | `Docs/README.md` 기준 정규화 검증 |

## 공용 수정 지점 잠금 현황
| 리소스 | 선점 담당 | 관련 작업 | 상태 | 마지막 갱신 | 비고 |
| --- | --- | --- | --- | --- | --- |
| `/Game/Maps/BasicMap` | - | Track C / E / F / G / H | 열림 | 2026-04-11 06:59:14 KST | 실제 맵 로드/수정/저장 구간은 직렬화 |
| `Interaction Asset Build BasicMap Save Window` | - | Track C / E / F / G | 열림 | 2026-04-11 06:59:14 KST | interaction asset builder가 `BasicMap`을 저장할 때 점유 |
| `/Game/Materials/T_Stylized_Grass_Dirt_01` | - | Track H | 열림 | 2026-04-11 06:59:14 KST | floor 텍스처 애셋 |
| `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01` | - | Track H | 열림 | 2026-04-11 06:59:14 KST | floor 머터리얼 애셋 |
| `UCodexBasicMapFloorBuildCommandlet` | - | Track H | 열림 | 2026-04-11 06:59:14 KST | floor build 진입점 |
| `UCodexInteractionAssetBuildCommandlet` | - | Track C / E / F / G | 열림 | 2026-04-11 06:59:14 KST | interaction build 진입점 |
