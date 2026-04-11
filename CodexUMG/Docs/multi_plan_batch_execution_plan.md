# 다중 플랜 일괄 실행 계획

## 문서 목적
- 아래 6개 플랜의 의존 관계, 착수 조건, 병렬 처리 경계를 하나의 상위 실행 계획으로 정리한다.
- 반드시 순차로 처리해야 하는 트랙과 병렬 처리 가능한 트랙을 분리한다.
- 공용 파일, 공용 Blueprint, 공용 에디터 모듈 충돌을 줄이기 위한 작업 원칙을 고정한다.
- 이 배치 플랜의 검증 목적은 구현 중간점검이 아니라, 이미 개별 검증이 끝난 작업들을 다시 수행했을 때 전체 재수행이 올바르게 재현되는지 확인하는 것이다.
- 이 문서에서 가장 중요한 우선순위는 작업이 중간에 끊기지 않고 가능한 한 끝까지 한 번에 진행되는 것이다.

## 대상 플랜
- `Docs/topdown_fixed_camera_wasd_plan.md`
- `Docs/interaction_umg_component_plan.md`
- `Docs/interaction_message_popup_plan.md`
- `Docs/interaction_scroll_message_popup_plan.md`
- `Docs/player_projectile_firing_plan.md`
- `Docs/vox_mesh_asset_pipeline_plan.md`

## 운영 보조 문서
- 작업본
  - `Docs/multi_plan_batch_execution_status.md`
  - `Docs/multi_plan_batch_execution_timeline.md`
- 보존용 Template
  - `Docs/Template/multi_plan_batch_execution_status.md`
  - `Docs/Template/multi_plan_batch_execution_timeline.md`
- 작업은 항상 `Docs/` 루트 작업본에서 진행하고, 처음부터 다시 시작해야 할 때만 `Docs/Template/`의 원본을 복사해 초기화한다.

## 최상위 실행 지침
- 먼저 전체 플랜을 읽고 공용 변경 지점과 책임 범위를 고정한 뒤 병렬 처리를 배치한다.
- 이 문서의 Phase 순서는 기준선이지만, 메인 작업자는 각 세부 플랜 문서의 상세 절차와 차단 조건을 다시 읽고 세부 실행 순서를 자율적으로 재배치할 수 있다.
- 단, 선행 게이트와 공유 리소스 보호 규칙은 깨지지 않아야 한다.
- 같은 파일, 같은 Blueprint, 같은 입력 애셋, 같은 commandlet 진입점은 동시에 수정하지 않는다.
- 메인 작업자는 `Docs/multi_plan_batch_execution_status.md`를 공식 상태판으로 유지하고, 시작/대기/차단/재개/완료 시 즉시 갱신한다.
- 실제 수행 시각, 인계 시점, 병렬 시작/중단/병합 시점은 `Docs/multi_plan_batch_execution_timeline.md`에 `YYYY-MM-DD HH:mm:ss KST` 형식으로 기록한다.
- 전체 작업이 완료되기 전에는 사용자에게 중간 구현 상태 검증을 요구하지 않는다.
- 중간 단계에서는 내부 점검만 수행하고, 사용자 검증 요청은 전체 트랙이 완료된 뒤 최종 재수행 검증이 필요할 때만 사용한다.
- 사용자 확인 대기나 중간 점검을 이유로 흐름을 멈추지 않고, 실제 차단 사유가 없는 한 전체 작업을 끝까지 이어서 수행하는 것을 최우선 원칙으로 둔다.
- 부분 완료 상태를 여러 번 끊어 보여주는 것보다, 최종 상태까지 한 번에 도달하는 것이 더 중요하다.

## 상세 실행 순서 재배치 원칙
- 하위 문서가 요구하는 선행 입력 파이프라인, 애셋, 테스트 게이트가 준비되었는지 먼저 본다.
- 공용 파일, 공용 Blueprint, 공용 `Commandlet`, 공용 에디터 모듈의 충돌 가능성을 먼저 평가한다.
- 병렬 에이전트를 투입할 수 있는지와 메인 작업자가 병합 가능한 시점을 함께 본다.
- 이후 검증을 쉽게 만드는 공용 기반 작업이 있다면 먼저 구현한다.
- `topdown_fixed_camera_wasd_plan`의 선행 게이트 성격과 `vox_mesh_asset_pipeline_plan`의 독립 병렬 트랙 성격은 유지한다.
- `interaction_message_popup_plan`과 `interaction_scroll_message_popup_plan`은 동일한 착수 조건을 공유하지만, 계획 단위 기준으로는 병렬 처리 가능한 독립 작업으로 본다.

## 의존성 정리

### 1. `topdown_fixed_camera_wasd_plan` 선행
- `interaction_umg_component_plan`, `player_projectile_firing_plan`보다 먼저 수행한다.
- 이유는 다음 기반 구조가 먼저 필요하기 때문이다.
  - `GameInstance` 경유 데이터 접근 구조
  - `DA_` 기반 참조 구조
  - 기존 `IMC_` / `IA_` 입력 파이프라인
  - 플레이어 `BP_Character`, `BP_PlayerController`, `BP_GameMode` 기본 흐름
- 후속 플랜 착수 게이트는 "캐릭터 이동 처리 완료"로 둔다.

### 2. `interaction_umg_component_plan` 후행
- `topdown_fixed_camera_wasd_plan`의 이동 처리 완료 후 착수 가능하다.
- 최소 조건은 아래와 같다.
  - 플레이어 입력 파이프라인이 실제 애셋 기준으로 동작할 것
  - `GameInstance -> DA_` 접근 구조가 검증될 것
  - 플레이어 관련 BP가 테스트 가능한 상태일 것
- 기존 `Enhanced Input` 구현이 없다면 즉시 중단하고 선행 구현을 사용자에게 요구한다.

### 3. `player_projectile_firing_plan` 후행
- 역시 `topdown_fixed_camera_wasd_plan`의 이동 처리 완료 후 착수 가능하다.
- 최소 조건은 아래와 같다.
  - 플레이어 입력 파이프라인이 실제 애셋 기준으로 동작할 것
  - `GameInstance -> DA_` 접근 구조가 검증될 것
  - 플레이어 관련 BP가 테스트 가능한 상태일 것
- `interaction_umg_component_plan`과의 상대 우선순위는 고정하지 않는다.

### 4. `vox_mesh_asset_pipeline_plan` 독립 병렬 트랙
- `vox_mesh_asset_pipeline_plan`은 위 런타임 플랜과 별개 병렬 트랙으로 수행한다.
- VOX 플랜은 런타임 플레이어 구조를 기다릴 필요가 없다.
- 다만 `CodexUMGBootstrapEditor`와 에디터 자동화 패턴을 공유할 수 있으므로 메인 작업자가 공유 파일 소유권을 먼저 정한다.

### 5. `interaction_message_popup_plan` 후행 통합 트랙
- 아래 두 플랜이 모두 완료된 뒤에만 착수한다.
  - `interaction_umg_component_plan`
  - `vox_mesh_asset_pipeline_plan`
- 이유는 아래 기반을 동시에 사용하기 때문이다.
  - 상호작용 컴포넌트와 전역 상호작용 서브시스템
  - `IMC_` / `IA_` / `DA_` 기반 입력 확장 경로
  - VOX 파이프라인으로 준비된 나무 팻말 메시

### 6. `interaction_scroll_message_popup_plan` 병렬 독립 후행 트랙
- 아래 두 플랜이 모두 완료된 뒤에만 착수한다.
  - `interaction_umg_component_plan`
  - `vox_mesh_asset_pipeline_plan`
- 착수 시점은 `interaction_message_popup_plan`과 동일하다.
- 이 플랜은 `interaction_message_popup_plan`과 계획 단위 기준으로는 독립적이며, 가능한 시점부터 병렬 처리 가능한 별도 트랙으로 본다.
- 두 팝업 플랜 모두 상호작용 서브시스템과 WBP 생성 흐름을 재사용하므로 실제 파일 소유권 충돌은 메인 작업자가 조정한다.

## 권장 수행 순서

### Phase 0. 전체 분석 및 작업 분해
1. 6개 플랜 문서를 모두 읽고 공용 의존성, 잠재 충돌 파일, 하위 문서 내부 절차, 차단 조건을 정리한다.
2. `Docs/multi_plan_batch_execution_status.md`, `Docs/multi_plan_batch_execution_timeline.md` 작업본 존재 여부를 확인하고 필요한 경우에만 Template 원본으로 초기화한다.
3. 메인 작업자가 아래 트랙으로 작업을 분리한다.
   - Track A: `topdown_fixed_camera_wasd_plan`
   - Track B: `vox_mesh_asset_pipeline_plan`
   - Track C: `interaction_umg_component_plan`
   - Track D: `player_projectile_firing_plan`
   - Track E: `interaction_message_popup_plan`
   - Track F: `interaction_scroll_message_popup_plan`
4. 파일/BP/애셋 책임 범위를 먼저 고정한다.
5. Track C와 Track D의 세부 순서, 공용 파일 점유 순서, 병렬 시작 시점을 자율 재배치한다.
6. Track E와 Track F는 Track B와 Track C 완료 후 시작하는 후행 팝업 트랙으로 예약한다.

### Phase 1A. 탑다운 기반 플랜 수행
1. `topdown_fixed_camera_wasd_plan`을 먼저 수행한다.
2. 목표는 아래 기반을 테스트 가능한 상태로 만드는 것이다.
   - `IA_`, `IMC_`, `DA_` 입력 애셋
   - `BP_GI_*`, `BP_GM_*`, `BP_PC_*`, `BP_Character_*`
   - 고정 탑다운 카메라
   - `WASD` 이동
3. 이 단계에서 "캐릭터 이동 처리 완료" 게이트를 통과하면 후속 상호작용/발사 플랜 착수가 가능하다.
4. 메인 작업자는 게이트 통과 시각을 상태판과 타임라인에 즉시 기록한다.

### Phase 1B. VOX 플랜 병렬 수행
1. `vox_mesh_asset_pipeline_plan`은 Phase 0 직후 별도 병렬 트랙으로 바로 착수 가능하다.
2. 목표는 아래를 독립 진행하는 것이다.
   - `.vox` 파서
   - VOX 매니페스트
   - `StaticMesh` 생성 파이프라인
   - 공용 VOX 머티리얼
   - VOX 전용 commandlet
3. 이 트랙은 `interaction_umg_component_plan`, `player_projectile_firing_plan`의 착수를 막지 않는다.
4. 공유 에디터 모듈이나 공유 commandlet 진입점 수정이 필요하면 메인 작업자가 먼저 점유 순서를 고정한다.

### Phase 2. 상호작용/발사 후속 플랜 분기
1. `topdown_fixed_camera_wasd_plan`의 이동 처리 완료 후 `interaction_umg_component_plan`, `player_projectile_firing_plan`의 착수 가능 여부를 확인한다.
2. `interaction_umg_component_plan`은 기존 입력 구조, `Enhanced Input`, `GameInstance -> DA_`, 플레이어/컨트롤러 BP를 재사용한다.
3. 기존 `Enhanced Input` 구현이 없으면 상호작용 플랜은 즉시 중단한다.
4. `player_projectile_firing_plan`도 기존 입력 구조, `GameInstance -> DA_`, 플레이어/컨트롤러 BP를 재사용한다.
5. 두 플랜의 상대 우선순위는 고정하지 않는다.
6. 아래 조건이 모두 맞으면 Track C와 Track D는 병렬 수행 가능하다.
   - 공용 파일 소유권이 정리되었을 것
   - 입력 애셋, 플레이어 컨트롤러, `GameInstance` 같은 공유 수정 지점의 순서가 정리되었을 것
   - 같은 Blueprint와 같은 commandlet 진입점을 동시에 수정하지 않을 것

### Phase 3. 후행 팝업 트랙 수행
1. `interaction_umg_component_plan`과 `vox_mesh_asset_pipeline_plan`이 모두 완료되면 `interaction_message_popup_plan`과 `interaction_scroll_message_popup_plan`이 동시에 착수 가능 상태가 된다.
2. Track E는 기본 메시지 팝업 트랙이다.
3. Track F는 스크롤 메시지 팝업 트랙이다.
4. Track E와 Track F는 계획 단위 기준으로는 독립 작업이며, 가능한 시점부터 병렬 처리 가능한 별도 트랙으로 본다.
5. 다만 두 트랙 모두 상호작용 서브시스템, 입력 확장, WBP 생성 자동화 흐름을 공유할 수 있으므로 실제 파일 점유는 메인 작업자가 조정한다.
6. `player_projectile_firing_plan`은 Track E와 Track F의 선행 조건이 아니므로 메인 작업자는 Track D, Track E, Track F를 병렬 또는 순차로 배치할 수 있다.

### Phase 4. 통합 및 최종 검증
1. Track A 기반 위에서 Track C, Track D, Track E, Track F 결과를 통합 검증한다.
2. Track B의 VOX 결과가 두 팝업 트랙에서 올바르게 참조되는지 별도로 검증한다.
3. 공용 모듈, 공용 설정, 공용 타입, 공용 입력 애셋 충돌을 메인 작업자가 최종 정리한다.
4. 최종 병합 전에 상태판의 `미착수`, `대기`, `차단` 항목이 없는지 확인하고, 최종 검증 시각과 병합 시점을 타임라인에 기록한다.
5. 이 단계의 검증은 미완성 중간 산출물 점검이 아니라, 이미 검증된 작업 묶음을 배치로 다시 수행했을 때 전체 재수행이 올바르게 되는지 확인하는 목적이다.
6. 따라서 사용자에게 중간 결과 확인을 요청하지 않고, 모든 트랙 완료 후 최종 재수행 검증 단위에서만 사용자 확인이 가능하다.
7. 최종 검증에 도달하기 전까지는 작업 연속성을 해치지 않는 것이 우선이며, 검증은 끝까지 완주한 뒤 한 번에 묶어서 수행한다.

## 상태판 및 타임라인 운영 규칙
- 공식 작업본은 `Docs/multi_plan_batch_execution_status.md`, `Docs/multi_plan_batch_execution_timeline.md`다.
- 초기화용 보존 원본은 `Docs/Template/multi_plan_batch_execution_status.md`, `Docs/Template/multi_plan_batch_execution_timeline.md`다.
- 작업본이 손상되었거나 정말 처음부터 다시 시작해야 할 때만 Template을 작업본으로 복사한다.
- 상태판은 최소한 `미착수`, `진행중`, `대기`, `차단`, `검증중`, `완료` 상태를 사용한다.
- 시작, 중단, 재개, 완료, 검증 시작, 차단 발생, 차단 해제, 실행 순서 재배치 시 메인 작업자가 상태판을 갱신한다.
- 타임라인에는 각 Phase/Track의 시작, 완료, 차단, 인계, 병합, 검증 시점을 남긴다.
- 공용 파일, 공용 Blueprint, 공용 `Commandlet`, 공용 에디터 모듈 점유와 해제 시점도 기록한다.
- 여기서 `검증중`은 구현 중간점검이 아니라 전체 또는 최종 재수행 검증 단계를 뜻한다.
- 전체 작업 완료 전에는 사용자 확인 대기 상태를 검증 절차로 사용하지 않는다.

## 권장 에이전트 운영안
- 메인 작업자
  - 전체 분석
  - 세부 실행 순서 재배치 확정
  - 공식 상태판/타임라인 갱신
  - 의존성 판단
  - 공용 파일 소유권 지정
  - 병렬 에이전트 중계 및 교통정리
  - 병합 및 최종 검증
- Agent A
  - `topdown_fixed_camera_wasd_plan` 전담
- Agent B
  - `vox_mesh_asset_pipeline_plan` 전담
- Agent C
  - `interaction_umg_component_plan` 전담
- Agent D
  - `player_projectile_firing_plan` 전담
- Agent E
  - `interaction_message_popup_plan` 전담
- Agent F
  - `interaction_scroll_message_popup_plan` 전담
- Agent G
  - 최종 검증 또는 리뷰 전담

## 실행 원칙 요약
- 선행 게이트는 `topdown_fixed_camera_wasd_plan`의 캐릭터 이동 처리 완료다.
- 후속 런타임 트랙은 `interaction_umg_component_plan`, `player_projectile_firing_plan`이다.
- 독립 병렬 트랙은 `vox_mesh_asset_pipeline_plan`이다.
- 후행 팝업 트랙은 `interaction_message_popup_plan`, `interaction_scroll_message_popup_plan`이다.
- 두 팝업 트랙의 착수 조건은 동일하다.
  - `interaction_umg_component_plan` 완료
  - `vox_mesh_asset_pipeline_plan` 완료
- `interaction_message_popup_plan`과 `interaction_scroll_message_popup_plan`은 가능한 시점부터 병렬 처리 가능한 독립 작업으로 본다.
- 공식 상태판은 `Docs/multi_plan_batch_execution_status.md`다.
- 공식 타임라인은 `Docs/multi_plan_batch_execution_timeline.md`다.
- 전체 작업 완료 전에는 사용자에게 중간 검증을 요구하지 않는다.
- 이 배치 플랜의 검증 목적은 중간 점검이 아니라 이미 검증된 작업들의 전체 재수행 일관성 확인이다.
- 가장 중요한 우선순위는 작업을 가능한 한 끊지 않고 최종 상태까지 한 번에 진행하는 것이다.
