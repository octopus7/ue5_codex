# 다중 플랜 일괄 실행 계획

## 문서 목적
- 아래 5개 플랜의 의존성, 착수 조건, 병렬 처리 경계를 하나의 상위 실행 계획으로 정리한다.
- 순차로 처리해야 할 트랙과 병렬로 처리할 수 있는 트랙을 명확히 나눈다.
- 공용 파일, 공용 Blueprint, 공용 에디터 모듈 충돌을 줄이기 위한 작업 원칙을 고정한다.

## 대상 플랜
- `Docs/topdown_fixed_camera_wasd_plan.md`
- `Docs/interaction_umg_component_plan.md`
- `Docs/interaction_message_popup_plan.md`
- `Docs/player_projectile_firing_plan.md`
- `Docs/vox_mesh_asset_pipeline_plan.md`

## 운영 보조 문서
- 작업본
  - `Docs/multi_plan_batch_execution_status.md`
  - `Docs/multi_plan_batch_execution_timeline.md`
- 보존용 Template
  - `Docs/Template/multi_plan_batch_execution_status.md`
  - `Docs/Template/multi_plan_batch_execution_timeline.md`
- 작업은 항상 `Docs/` 루트 작업본에서 진행하고, 처음부터 다시 시작해야 할 때만 `Docs/Template/`의 원본을 복사해 작업본을 초기화한다.

## 최상위 실행 지침
- "전체 분석해보고 자율적으로 에이전트를 이용해 병렬처리하여 리드타임을 줄이도록한다"를 본 계획의 최상위 지침으로 둔다.
- 메인 작업자는 먼저 5개 플랜 전체를 분석한 뒤 공용 변경 지점과 책임 범위를 확정한다.
- 상위 문서에 적힌 Phase 순서는 기준선으로 유지하되, 메인 작업자는 각 하위 플랜 문서의 세부 절차, 착수 조건, 차단 조건, 공용 수정 지점을 다시 읽고 세부 실행 순서를 자율적으로 재배치한다.
- 세부 실행 순서를 재배치하더라도 선행 게이트, 공용 수정 지점 보호, 병렬 충돌 금지 규칙은 깨지지 않아야 한다.
- 병렬 처리는 적극적으로 사용하되, 같은 파일, 같은 Blueprint, 같은 입력 에셋, 같은 커맨드렛 진입점을 동시에 수정하지 않는다.
- 상태 유실 방지를 위해 메인 작업자는 `Docs/multi_plan_batch_execution_status.md`를 단일 상태판으로 유지하고, 작업 시작/대기/차단/재개/완료 시 즉시 갱신한다.
- 단계별 실제 수행 시각, 핸드오프, 병렬 착수/중단/병합 시점은 `Docs/multi_plan_batch_execution_timeline.md`에 `YYYY-MM-DD HH:mm:ss KST` 형식으로 기록한다.
- 작업을 처음부터 다시 시작해야 할 때만 `Docs/Template/`의 보존용 원본을 `Docs/` 루트 작업본으로 복사해 초기 상태로 되돌린다.
- 병렬 작업 에이전트가 있으면 메인 작업자가 중계자이자 교통정리 담당이 되어 착수 승인, 공용 파일 선점, 산출물 전달, 병합 순서를 조정한다.
- 즉시 막히는 핵심 경로 판단, 병합, 충돌 해결, 최종 검증은 메인 작업자가 담당한다.

## 세부 실행순서 재배치 원칙
- 본 문서의 권장 수행 순서는 상위 가이드이며, 실제 세부 순서는 하위 플랜 문서까지 읽은 뒤 메인 작업자가 확정한다.
- 재배치 판단 기준은 아래와 같다.
  - 하위 문서가 요구하는 선행 타입, 입력 파이프라인, 에셋, 테스트 게이트 준비 여부
  - 공용 파일, 공용 Blueprint, 공용 `Commandlet`, 공용 에디터 모듈 충돌 가능성
  - 병렬 투입 가능한 에이전트 수와 메인 작업자의 병합 가능 시점
  - 먼저 구현해야 후속 검증이 쉬워지는 공용 기반 작업 존재 여부
- `topdown_fixed_camera_wasd_plan`의 선행 게이트와 `vox_mesh_asset_pipeline_plan`의 독립 병렬 트랙 성격은 유지한다.
- `interaction_umg_component_plan`와 `player_projectile_firing_plan`의 상대 순서, 공용 수정 지점 선점 순서, 검증 시점은 메인 작업자가 하위 문서 내용을 반영해 자율 결정한다.
- `interaction_message_popup_plan`는 `interaction_umg_component_plan` 완료와 `vox_mesh_asset_pipeline_plan` 완료 이후에만 착수할 수 있는 후행 플랜으로 고정한다.
- 재배치 결과가 바뀌면 메인 작업자는 사유와 변경 결과를 상태판과 타임라인에 즉시 반영한다.

## 의존성 정리

### 1. `topdown_fixed_camera_wasd_plan` 선행
- `interaction_umg_component_plan`, `player_projectile_firing_plan`보다 먼저 수행한다.
- 이유는 상호작용/투사체 구조가 아래 기반을 필요로 하기 때문이다.
  - `GameInstance` 경유 데이터 접근 구조
  - `DA_` 기반 참조 구조
  - 기존 `IMC_` / `IA_` 입력 파이프라인
  - 플레이어 `BP_Character`, `BP_PlayerController`, `BP_GameMode` 기본 흐름
- 따라서 `topdown_fixed_camera_wasd_plan`은 후속 플랜의 기반 플랜이다.
- 특히 후속 플랜 착수용 하위 게이트는 "캐릭터 이동 처리 완료"로 둔다.
  - 고정 카메라 기준 이동이 실제로 동작할 것
  - 기존 `Enhanced Input` 경로가 확인될 것
  - 플레이어 관련 BP가 테스트 가능한 상태일 것

### 2. `interaction_umg_component_plan` 후행
- `topdown_fixed_camera_wasd_plan`의 캐릭터 이동 처리 완료 후 착수할 수 있다.
- 최소 착수 조건은 아래와 같다.
  - 플레이어 입력 파이프라인이 실제 에셋 기준으로 동작할 것
  - `GameInstance -> DA_` 경유 접근 구조가 검증될 것
  - 플레이어 관련 BP가 테스트 가능한 상태일 것
- 기존 `Enhanced Input` 구현이 실제로 존재하는지 먼저 확인해야 한다.
- 기존 `Enhanced Input` 구현이 없다면 이 플랜은 즉시 중단하고 사용자에게 선행 구현을 요구한다.
- 입력 확장은 반드시 기존 `IMC_` / `IA_` / `DA_` 구조를 그대로 따라간다.

### 3. `player_projectile_firing_plan` 후행
- `topdown_fixed_camera_wasd_plan`의 캐릭터 이동 처리 완료 후 착수할 수 있다.
- 최소 착수 조건은 아래와 같다.
  - 플레이어 입력 파이프라인이 실제 에셋 기준으로 동작할 것
  - `GameInstance -> DA_` 경유 접근 구조가 검증될 것
  - 플레이어 관련 BP가 테스트 가능한 상태일 것
- 위 조건이 충족되기 전에는 투사체 발사 플랜을 본격 진행하지 않는다.
- `interaction_umg_component_plan`과의 우선순위는 고정하지 않는다.
  - 현재 충돌 파일 수
  - 메인 작업자의 공유 파일 선점 상태
  - 병렬 투입 가능한 에이전트 수
  - 전체 크리티컬 패스
  를 보고 메인 작업자가 자율 판단한다.
- 공유 파일 충돌을 줄일 수 있으면 `interaction_umg_component_plan`과 병렬 수행해도 무방하다.

### 4. `vox_mesh_asset_pipeline_plan` 병렬 별도 트랙
- `vox_mesh_asset_pipeline_plan`는 위 두 플랜과 별도 병렬 트랙으로 수행한다.
- VOX 플랜은 런타임 플레이 구조 의존성이 없으므로 `topdown_fixed_camera_wasd_plan` 완료를 기다릴 필요가 없다.
- 다만 `CodexUMGBootstrapEditor` 모듈과 에디터 자동화 패턴을 공유할 수 있으므로 아래 충돌 방지 규칙을 둔다.
  - VOX 트랙은 `Vox/`, VOX 전용 `Commandlet`, VOX 머터리얼/메시 빌드 파일을 우선 소유한다.
  - 탑다운/상호작용/투사체 트랙은 입력 에셋, 플레이어 BP, 게임 인스턴스/게임 모드, 탑다운용 부트스트랩 자산을 우선 소유한다.
  - 공용 모듈 등록 파일이나 공용 타입 파일은 메인 작업자가 먼저 잠그거나 병합 지점을 지정한다.

### 5. `interaction_message_popup_plan` 후행
- `interaction_message_popup_plan`는 아래 두 플랜이 모두 완료된 뒤에만 착수한다.
  - `interaction_umg_component_plan`
  - `vox_mesh_asset_pipeline_plan`
- 이유는 팝업 플랜이 아래 기반을 동시에 필요로 하기 때문이다.
  - 기존 상호작용 컴포넌트와 전역 상호작용 서브시스템
  - 기존 상호작용 입력 경로와 `IMC_` / `IA_` / `DA_` 확장 구조
  - VOX 파이프라인으로 생성된 나무 팻말 메시
- 따라서 팝업 플랜은 상호작용 트랙과 VOX 트랙의 완료 산출물을 재사용하는 통합 후행 플랜이다.
- 이 플랜은 `player_projectile_firing_plan` 완료를 기다릴 필요는 없지만, 공용 입력 에셋과 공용 상호작용 서브시스템 수정 구간은 메인 작업자가 선점 순서를 정한다.

## 권장 수행 순서

### Phase 0. 전체 분석 및 작업 분해
1. 다섯 플랜 문서를 모두 읽고 공용 의존성, 충돌 가능 파일, 하위 문서 내부의 세부 실행 절차와 차단 조건을 식별한다.
2. 메인 작업자가 `Docs/multi_plan_batch_execution_status.md`, `Docs/multi_plan_batch_execution_timeline.md` 작업본 존재 여부를 확인하고, 초기화가 필요할 때만 `Docs/Template/`의 보존용 원본으로 덮어쓴 뒤 첫 기록을 남긴다.
3. 메인 작업자가 작업 트랙을 아래처럼 분리한다.
   - Track A: `topdown_fixed_camera_wasd_plan`
   - Track B: `vox_mesh_asset_pipeline_plan`
   - Track C: `interaction_umg_component_plan`
   - Track D: `player_projectile_firing_plan`
   - Track E: `interaction_message_popup_plan`
4. 에이전트별 파일/BP/에셋 책임 범위를 먼저 고정한다.
5. 하위 문서 분석 결과를 기준으로 Track C와 Track D의 세부 순서, 공용 파일 선점 순서, 병렬 착수 시점을 자율 재배치한다.
6. Track E는 Track B와 Track C 완료 후 착수하는 후행 통합 트랙으로 예약한다.

### Phase 1A. 탑다운 기반 플랜 수행
1. `topdown_fixed_camera_wasd_plan`를 먼저 수행한다.
2. 목표는 아래 기반을 테스트 가능한 상태로 만드는 것이다.
   - `IA_`, `IMC_`, `DA_` 입력 자산
   - `BP_GI_*`, `BP_GM_*`, `BP_PC_*`, `BP_Character_*`
   - 고정 탑다운 카메라
   - `WASD` 이동
3. 이 단계에서 "캐릭터 이동 처리 완료" 게이트를 통과하면 후속 상호작용/투사체 플랜 착수가 가능하다.
4. 게이트 통과 여부와 실제 통과 시각은 메인 작업자가 상태판과 타임라인에 즉시 반영한다.

### Phase 1B. VOX 플랜 병렬 수행
1. `vox_mesh_asset_pipeline_plan`는 Phase 0 직후 별도 병렬 트랙으로 바로 착수할 수 있다.
2. 목표는 아래를 독립적으로 진행하는 것이다.
   - `.vox` 파서
   - VOX 매니페스트
   - `StaticMesh` 생성 파이프라인
   - 공용 VOX 머터리얼
   - VOX 전용 커맨드렛
3. 이 트랙은 `interaction_umg_component_plan`, `player_projectile_firing_plan` 착수를 막지 않는다.
4. 다만 공용 에디터 모듈이나 공용 `Commandlet` 진입점 수정이 필요해지면 메인 작업자가 선점 여부를 판단하고 타임라인에 기록한다.

### Phase 2. 상호작용/투사체 후속 플랜 분기
1. `topdown_fixed_camera_wasd_plan`의 캐릭터 이동 처리 완료 후 `interaction_umg_component_plan`, `player_projectile_firing_plan` 착수 가능 여부를 함께 판단한다.
2. `interaction_umg_component_plan`는 아래 기반을 재사용한다.
   - 기존 플레이어 입력 구조
   - 기존 `Enhanced Input` 구현
   - `GameInstance -> DA_` 참조 구조
   - 플레이어 BP와 컨트롤러 BP
3. 기존 `Enhanced Input` 구현이 없으면 상호작용 플랜은 즉시 멈추고 사용자에게 선행 구현을 요구한다.
4. `player_projectile_firing_plan`는 아래 기반을 재사용한다.
   - 기존 플레이어 입력 구조
   - `GameInstance -> DA_` 참조 구조
   - 플레이어 BP와 컨트롤러 BP
5. 두 플랜의 우선순위는 고정하지 않는다.
6. 아래 조건을 만족하면 `interaction_umg_component_plan`와 `player_projectile_firing_plan`를 병렬 에이전트 트랙으로 수행해도 된다.
   - 공유 파일 선점이 끝났을 것
   - 입력 자산/플레이어 컨트롤러/`GameInstance` 같은 공용 수정 지점이 먼저 정리되었을 것
   - 같은 BP와 같은 커맨드렛 진입점을 동시 수정하지 않을 것
7. 입력 처리 추가가 필요하더라도 두 플랜 모두 기존 `IMC_`를 우선 확장한다.
8. Track C와 Track D 중 누가 먼저 공용 입력/컨트롤러/`GameInstance` 수정 구간을 사용할지는 메인 작업자가 선점 순서로 정하고 상태판에 기록한다.
9. 병렬 에이전트가 있으면 각 에이전트는 메인 작업자에게 상태 변경과 핸드오프를 보고하고, 메인 작업자가 공식 상태판과 타임라인을 갱신한다.

### Phase 3. 팝업 후행 플랜 수행
1. `interaction_umg_component_plan` 완료와 `vox_mesh_asset_pipeline_plan` 완료가 모두 확인되면 `interaction_message_popup_plan` 착수 가능 상태가 된다.
2. Track E는 아래 기반을 재사용한다.
   - Track C의 상호작용 컴포넌트 / 상호작용 서브시스템 / 입력 확장 결과
   - Track B의 VOX 나무 팻말 메시 결과
3. Track E는 Track C의 상호작용 시스템을 확장하는 성격이므로, 공용 상호작용 서브시스템, 공용 입력 자산, 공용 상호작용 커맨드렛 수정 구간은 메인 작업자가 먼저 선점 순서를 정한다.
4. Track E는 Track B와 Track C가 끝나기 전에는 착수하지 않는다.
5. `player_projectile_firing_plan`는 Track E의 선행 조건이 아니므로, 메인 작업자는 Track D와 Track E를 병렬 또는 순차로 배치할 수 있다.

### Phase 4. 통합 및 최종 검증
1. Track A의 기반 결과 위에 Track C, Track D, Track E 결과를 통합 검증한다.
2. Track B의 VOX 결과를 별도로 검증한 뒤, 팝업 트랙이 그 산출물을 정상 참조하는지 함께 점검한다.
3. 공용 모듈, 공용 설정, 공용 타입, 공용 입력 자산 충돌은 마지막에 메인 작업자가 정리한다.
4. 최종 병합 전 메인 작업자는 상태판의 미착수/대기/차단 항목을 점검하고, 최종 검증 시각과 병합 시점을 타임라인에 남긴다.

## 상태판 및 타임라인 운영 규칙
- 저장소에는 공식 작업본 `Docs/multi_plan_batch_execution_status.md`, `Docs/multi_plan_batch_execution_timeline.md`를 유지한다.
- 별도로 초기화용 보존본 `Docs/Template/multi_plan_batch_execution_status.md`, `Docs/Template/multi_plan_batch_execution_timeline.md`를 함께 유지한다.
- 작업본이 오염되었거나 처음부터 다시 시작해야 할 때만 Template 원본을 루트 작업본으로 복사한다.
- `Docs/multi_plan_batch_execution_status.md`는 본 계획의 공식 진행 현황판이다.
- 상태판에는 최소한 `미착수`, `진행중`, `대기`, `차단`, `검증중`, `완료` 상태를 사용한다.
- 작업 시작, 중단, 재개, 완료, 검증 시작, 차단 발생, 차단 해제, 실행순서 재배치 시 메인 작업자가 상태판을 갱신한다.
- `Docs/multi_plan_batch_execution_timeline.md`는 시계열 로그이며, 각 Phase/Track의 시작 시각, 완료 시각, 차단 시각, 핸드오프, 병합, 검증 이벤트를 남긴다.
- 공용 파일, 공용 Blueprint, 공용 `Commandlet`, 공용 에디터 모듈 선점과 해제 시점도 타임라인에 남긴다.
- 병렬 에이전트는 자신의 작업 상태를 메인 작업자에게 보고만 하고, 공식 상태판/타임라인 갱신은 메인 작업자가 수행한다.

## 권장 에이전트 운영안
- 메인 작업자
  - 전체 분석
  - 하위 플랜 세부 절차 기반 실행순서 재배치 확정
  - 상태판/타임라인 공식 갱신
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
  - 단, Agent A의 캐릭터 이동 처리 게이트 통과 후 착수
- Agent D
  - `player_projectile_firing_plan` 전담
  - 단, Agent A의 캐릭터 이동 처리 게이트 통과 후 착수
- Agent E
  - `interaction_message_popup_plan` 전담
  - 단, Agent C 완료와 Agent B 완료 후 착수
- Agent F
  - 최종 검증 또는 리뷰 전담
- 모든 병렬 에이전트
  - 상태 변경, 선점 요청, 완료 보고를 메인 작업자에게 전달
  - 공식 상태판과 타임라인은 직접 수정하지 않고 메인 작업자를 통해 반영

## 실행 원칙 요약
- 선행 게이트: `topdown_fixed_camera_wasd_plan`의 캐릭터 이동 처리 완료
- 후속 트랙: `interaction_umg_component_plan`, `player_projectile_firing_plan`
- 두 후속 트랙의 우선순위는 고정하지 않고 메인 작업자가 자율 판단한다.
- 세부 실행순서는 하위 플랜 문서를 다시 읽은 뒤 메인 작업자가 자율 재배치한다.
- 병렬 트랙: `vox_mesh_asset_pipeline_plan`
- 조건부 병렬 트랙: `interaction_umg_component_plan` + `player_projectile_firing_plan`
- 후행 통합 트랙: `interaction_message_popup_plan`
- 팝업 트랙 착수 조건: `interaction_umg_component_plan` 완료 + `vox_mesh_asset_pipeline_plan` 완료
- 상태판 작업본: `Docs/multi_plan_batch_execution_status.md`
- 타임라인 작업본: `Docs/multi_plan_batch_execution_timeline.md`
- Template 원본: `Docs/Template/multi_plan_batch_execution_status.md`, `Docs/Template/multi_plan_batch_execution_timeline.md`
- 병렬 에이전트가 있으면 메인 작업자가 중계자이자 교통정리 담당이다.
- 공용 변경점은 메인 작업자가 선점하고, 세부 구현은 에이전트로 병렬 분해해도 무방하다.
- 리드타임 단축이 목표지만, 같은 파일과 같은 BP를 동시에 수정하는 병렬화는 금지한다.
