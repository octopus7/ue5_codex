# 다중 플랜 일괄 실행 계획

## 문서 목적
- 아래 3개 플랜의 의존성, 착수 조건, 병렬 처리 경계를 하나의 상위 실행 계획으로 정리한다.
- 순차로 처리해야 할 트랙과 병렬로 처리할 수 있는 트랙을 명확히 나눈다.
- 공용 파일, 공용 Blueprint, 공용 에디터 모듈 충돌을 줄이기 위한 작업 원칙을 고정한다.

## 대상 플랜
- `Docs/topdown_fixed_camera_wasd_plan.md`
- `Docs/player_projectile_firing_plan.md`
- `Docs/vox_mesh_asset_pipeline_plan.md`

## 최상위 실행 지침
- "전체 분석해보고 자율적으로 에이전트를 이용해 병렬처리하여 리드타임을 줄이도록한다"를 본 계획의 최상위 지침으로 둔다.
- 메인 작업자는 먼저 3개 플랜 전체를 분석한 뒤 공용 변경 지점과 책임 범위를 확정한다.
- 병렬 처리는 적극적으로 사용하되, 같은 파일, 같은 Blueprint, 같은 입력 에셋, 같은 커맨드렛 진입점을 동시에 수정하지 않는다.
- 즉시 막히는 핵심 경로 판단, 병합, 충돌 해결, 최종 검증은 메인 작업자가 담당한다.

## 의존성 정리

### 1. `topdown_fixed_camera_wasd_plan` 선행
- `player_projectile_firing_plan`보다 먼저 수행한다.
- 이유는 투사체 발사 구조가 아래 기반을 필요로 하기 때문이다.
  - `GameInstance` 경유 데이터 접근 구조
  - `DA_` 기반 참조 구조
  - 기존 `IMC_` / `IA_` 입력 파이프라인
  - 플레이어 `BP_Character`, `BP_PlayerController`, `BP_GameMode` 기본 흐름
- 따라서 `topdown_fixed_camera_wasd_plan`은 후속 플랜의 기반 플랜이다.

### 2. `player_projectile_firing_plan` 후행
- `topdown_fixed_camera_wasd_plan` 완료 후 착수한다.
- 최소 착수 조건은 아래와 같다.
  - 플레이어 입력 파이프라인이 실제 에셋 기준으로 동작할 것
  - `GameInstance -> DA_` 경유 접근 구조가 검증될 것
  - 플레이어 관련 BP가 테스트 가능한 상태일 것
- 위 조건이 충족되기 전에는 투사체 발사 플랜을 본격 진행하지 않는다.

### 3. `vox_mesh_asset_pipeline_plan` 병렬 별도 트랙
- `vox_mesh_asset_pipeline_plan`는 위 두 플랜과 별도 병렬 트랙으로 수행한다.
- VOX 플랜은 런타임 플레이 구조 의존성이 없으므로 `topdown_fixed_camera_wasd_plan` 완료를 기다릴 필요가 없다.
- 다만 `CodexUMGBootstrapEditor` 모듈과 에디터 자동화 패턴을 공유할 수 있으므로 아래 충돌 방지 규칙을 둔다.
  - VOX 트랙은 `Vox/`, VOX 전용 `Commandlet`, VOX 머터리얼/메시 빌드 파일을 우선 소유한다.
  - 탑다운 트랙은 입력 에셋, 플레이어 BP, 게임 인스턴스/게임 모드, 탑다운용 부트스트랩 자산을 우선 소유한다.
  - 공용 모듈 등록 파일이나 공용 타입 파일은 메인 작업자가 먼저 잠그거나 병합 지점을 지정한다.

## 권장 수행 순서

### Phase 0. 전체 분석 및 작업 분해
1. 세 플랜 문서를 모두 읽고 공용 의존성과 충돌 가능 파일을 식별한다.
2. 메인 작업자가 작업 트랙을 아래처럼 분리한다.
   - Track A: `topdown_fixed_camera_wasd_plan`
   - Track B: `vox_mesh_asset_pipeline_plan`
3. 에이전트별 파일/BP/에셋 책임 범위를 먼저 고정한다.

### Phase 1A. 탑다운 기반 플랜 수행
1. `topdown_fixed_camera_wasd_plan`를 먼저 수행한다.
2. 목표는 아래 기반을 테스트 가능한 상태로 만드는 것이다.
   - `IA_`, `IMC_`, `DA_` 입력 자산
   - `BP_GI_*`, `BP_GM_*`, `BP_PC_*`, `BP_Character_*`
   - 고정 탑다운 카메라
   - `WASD` 이동
3. 이 단계가 후속 투사체 플랜의 선행 게이트다.

### Phase 1B. VOX 플랜 병렬 수행
1. `vox_mesh_asset_pipeline_plan`는 Phase 0 직후 별도 병렬 트랙으로 바로 착수할 수 있다.
2. 목표는 아래를 독립적으로 진행하는 것이다.
   - `.vox` 파서
   - VOX 매니페스트
   - `StaticMesh` 생성 파이프라인
   - 공용 VOX 머터리얼
   - VOX 전용 커맨드렛
3. 이 트랙은 `player_projectile_firing_plan` 착수를 막지 않는다.

### Phase 2. 투사체 플랜 수행
1. `topdown_fixed_camera_wasd_plan` 검증 완료 후 `player_projectile_firing_plan`를 진행한다.
2. 투사체 플랜은 아래 기반을 재사용한다.
   - 기존 플레이어 입력 구조
   - `GameInstance -> DA_` 참조 구조
   - 플레이어 BP와 컨트롤러 BP
3. 입력 처리 추가가 필요하더라도 기존 `IMC_`를 우선 확장한다.

### Phase 3. 통합 및 최종 검증
1. Track A의 `topdown + projectile` 결과를 먼저 통합 검증한다.
2. Track B의 VOX 결과를 별도로 검증한 뒤 메인 브랜치 수준에서 병합한다.
3. 공용 모듈, 공용 설정, 공용 타입 충돌은 마지막에 메인 작업자가 정리한다.

## 권장 에이전트 운영안
- 메인 작업자
  - 전체 분석
  - 의존성 판단
  - 공용 파일 소유권 지정
  - 병합 및 최종 검증
- Agent A
  - `topdown_fixed_camera_wasd_plan` 전담
- Agent B
  - `vox_mesh_asset_pipeline_plan` 전담
- Agent C
  - `player_projectile_firing_plan` 전담
  - 단, Agent A의 선행 게이트 통과 후 착수
- Agent D
  - 최종 검증 또는 리뷰 전담

## 실행 원칙 요약
- 순차 트랙: `topdown_fixed_camera_wasd_plan` -> `player_projectile_firing_plan`
- 병렬 트랙: `vox_mesh_asset_pipeline_plan`
- 공용 변경점은 메인 작업자가 선점하고, 세부 구현은 에이전트로 병렬 분해한다.
- 리드타임 단축이 목표지만, 같은 파일과 같은 BP를 동시에 수정하는 병렬화는 금지한다.
