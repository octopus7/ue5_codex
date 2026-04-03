# CodexHarness 작업 시간 로그

최종 수정일: 2026-04-04

## 사용 규칙

- 이 문서는 append 전용 로그로 운영한다.
- 각 항목은 하나의 수행 단계 또는 독립 작업 단위를 나타낸다.
- 최소 기록 필드는 작업 ID 또는 단계 ID, 작업명, 시작 시각, 종료 시각, 소요 시간, 병렬 수행 여부, 메모다.
- `generator/evaluator` 분리 운영을 사용한 경우 메모에 역할 분리와 메인 에이전트 중계 방식을 함께 적는다.

## 기록 형식

```md
### YYYY-MM-DD | <단계 ID 또는 작업 ID> | <작업명>

- 시작: `YYYY-MM-DD HH:MM:SS +09:00`
- 종료: `YYYY-MM-DD HH:MM:SS +09:00`
- 소요 시간: `HH:MM:SS`
- 병렬 수행: `예 | 아니오`
- 메모: ... (`generator/evaluator` 분리 운영을 썼다면 역할 분리와 중계 방식 포함)
```

## 기록

### 2026-04-04 | DOC-REBASE | 하네스 문서 실제 저장소 기준 재정렬 시작

- 시작: `2026-04-04 02:50:00 +09:00`
- 종료: `2026-04-04 03:02:58 +09:00`
- 소요 시간: `00:12:58`
- 병렬 수행: `예`
- 메모: `TopDownTestOne` 템플릿 잔재를 실제 `CodexHarness` 저장소 기준으로 교정했다. `evaluator` 컨텍스트가 `CodexInvenMedium`, `CodexInven`, `CodexWave`, `CodexUISimple`의 재사용 패턴을 조사했고 메인 컨텍스트가 결과를 중계받아 문서와 구현 방향을 확정했다.

### 2026-04-04 | M0-P1 | 헤드리스 제작 기반과 VOX import 경로 구축

- 시작: `2026-04-04 02:55:21 +09:00`
- 종료: `2026-04-04 03:02:58 +09:00`
- 소요 시간: `00:07:37`
- 병렬 수행: `예`
- 메모: `CodexHarnessEditor` 모듈, `CodexHarnessHeadlessSetup` 커맨드렛, 로컬 `VoxImporter` 플러그인 편입, `M_VoxBase`, 샘플 `.vox`, 샘플 `StaticMesh`, 자동화 보고서를 추가했다. 빌드는 `-NoHotReloadFromIDE`로 Live Coding mutex를 우회했고, 커맨드렛 재실행이 무경고로 통과했다.

### 2026-04-04 | M1-P1 | 게임플레이 기반 클래스와 실체 BP 연결 생성

- 시작: `2026-04-04 03:08:52 +09:00`
- 종료: `2026-04-04 03:18:33 +09:00`
- 소요 시간: `00:09:41`
- 병렬 수행: `예`
- 메모: `evaluator` 컨텍스트가 BP 생성 API, 입력 애셋 생성 API, `GameMapsSettings` 연결 주의점을 재확인했고 메인 컨텍스트가 결과를 중계받아 구현했다. `GameInstance`, `InputConfigDataAsset`, `GameMode`, `PlayerController`, `Character`, `HUD` C++ 골격을 추가하고 커맨드렛을 확장해 `BP_*`, `IA_*`, `IMC_Default`, `DA_DefaultInputConfig`를 생성 또는 갱신했다. `GameInstanceClass`, `GlobalDefaultGameMode`, `BasicMap` `WorldSettings` 연결까지 적용했고, 빌드와 `CodexHarnessHeadlessSetup` 재실행이 `0 error(s), 0 warning(s)`로 통과했다.

### 2026-04-04 | M1-P2 | `EnhancedInput` 기반 이동 연결과 상면 카메라 이동 검증

- 시작: `2026-04-04 03:18:33 +09:00`
- 종료: `2026-04-04 03:22:39 +09:00`
- 소요 시간: `00:04:06`
- 병렬 수행: `아니오`
- 메모: `PlayerController`가 `DA_DefaultInputConfig`에서 `IA_Move`를 읽어 실제 바인딩하고, `Character`는 `MoveInTopDownPlane`으로 카메라 기준 평면 이동만 계산하도록 역할을 분리했다. 이동 구현 후 `CodexHarnessEditor` 빌드와 `CodexHarnessHeadlessSetup` 재실행이 계속 `0 error(s), 0 warning(s)`로 통과했다.

### 2026-04-04 | M2-P1 | 마우스 조준과 바라보기 회전

- 시작: `2026-04-04 03:23:48 +09:00`
- 종료: `2026-04-04 03:25:16 +09:00`
- 소요 시간: `00:01:28`
- 병렬 수행: `아니오`
- 메모: `PlayerController`가 매 틱 마우스 커서를 월드 평면으로 투영하고, `Character`는 `AimAtWorldLocation`으로 상면 Yaw 회전을 수행하도록 분리했다. 조준 구현 후 `CodexHarnessEditor` 빌드와 `CodexHarnessHeadlessSetup` 재실행이 계속 `0 error(s), 0 warning(s)`로 통과했다.

### 2026-04-04 | M3-P1 | 기본 발사 입력과 초기 전투 루프

- 시작: `2026-04-04 03:25:16 +09:00`
- 종료: `2026-04-04 03:31:05 +09:00`
- 소요 시간: `00:05:49`
- 병렬 수행: `아니오`
- 메모: `PlayerController`가 `IA_Fire`를 실제 바인딩하고, `Character`는 현재 조준 지점 기준 히트스캔 라인트레이스로 최소 발사 루프를 수행하도록 분리했다. 명중 액터에는 `ApplyDamage`를 호출해 다음 단계 체력/피격 구조와 바로 연결 가능한 경로를 확보했고, `CodexHarnessEditor` 빌드와 `CodexHarnessHeadlessSetup` 재실행이 계속 `0 error(s), 0 warning(s)`로 통과했다.

### 2026-04-04 | M3-P2 | 체력, 피격, 사망 처리

- 시작: `2026-04-04 03:31:05 +09:00`
- 종료: `2026-04-04 03:36:19 +09:00`
- 소요 시간: `00:05:14`
- 병렬 수행: `예`
- 메모: `UCodexHarnessHealthComponent`를 추가해 생명주기를 공용 컴포넌트로 분리하고, 플레이어 사망 시 `GameMode`의 게임오버 플래그와 입력 차단으로 연결했다. 발사 판정은 체력 컴포넌트가 있는 액터에만 `ApplyDamage`를 보내도록 정리했다. 병렬로 `evaluator` 컨텍스트가 `M4~M5` 최소 완주 구조와 리스크를 읽기 전용으로 검토했고, 메인 컨텍스트가 피드백을 중계받아 구조를 확정했다. `CodexHarnessEditor` 빌드와 `CodexHarnessHeadlessSetup` 재실행은 계속 `0 error(s), 0 warning(s)`로 통과했다.

### 2026-04-04 | M4-P1 | 적 1종 추적 이동과 공격

- 시작: `2026-04-04 03:36:19 +09:00`
- 종료: `2026-04-04 03:42:17 +09:00`
- 소요 시간: `00:05:58`
- 병렬 수행: `아니오`
- 메모: `ACodexHarnessEnemyCharacter`를 추가하고 `GameMode`가 런타임에 적 BP 클래스를 1회 스폰하도록 확장했다. 적은 별도 내비게이션 의존성 없이 플레이어를 직접 추적하고, 공격 쿨다운마다 플레이어 체력 컴포넌트로 데미지를 전달한다. `CodexHarnessHeadlessSetup` 커맨드렛은 `/Game/CodexHarness/Blueprints/Enemies/BP_CodexHarnessEnemyCharacter`를 생성 또는 갱신하고 `EnemyCharacterClass`를 `BP_CodexHarnessGameMode`에 연결한다. 빌드와 커맨드렛 재실행은 계속 `0 error(s), 0 warning(s)`로 통과했고, `BP_CodexHarnessEnemyCharacter.uasset` 생성도 확인했다.

### 2026-04-04 | M4-P2 | 웨이브 스폰과 남은 적 수 추적

- 시작: `2026-04-04 03:42:17 +09:00`
- 종료: `2026-04-04 03:45:04 +09:00`
- 소요 시간: `00:02:47`
- 병렬 수행: `아니오`
- 메모: `ACodexHarnessGameMode`에 `CurrentWave`, `RemainingEnemyCount`, `PendingEnemySpawnCount`, `AliveEnemyCount`를 정리하고, 웨이브 시작 시 다중 적 스폰과 적 사망 시 남은 적 수 감소, 웨이브 종료 시 다음 웨이브 타이머 시작을 연결했다. 게임오버에서는 웨이브/적 스폰 타이머를 중단한다. 빌드와 `CodexHarnessHeadlessSetup` 재실행은 계속 `0 error(s), 0 warning(s)`로 통과했다.

### 2026-04-04 | M5-P1 | HUD 표시

- 시작: `2026-04-04 03:45:04 +09:00`
- 종료: `2026-04-04 03:47:12 +09:00`
- 소요 시간: `00:02:08`
- 병렬 수행: `아니오`
- 메모: `ACodexHarnessHUD::DrawHUD()`에 `Canvas HUD` 기반 체력 바와 텍스트를 추가해 플레이어 체력, 현재 웨이브, 남은 적 수를 직접 표시하도록 정리했다. 빌드와 `CodexHarnessHeadlessSetup` 재실행은 계속 `0 error(s), 0 warning(s)`로 통과했다.

### 2026-04-04 | M5-P2 | 게임 오버 UI와 재시작 흐름

- 시작: `2026-04-04 03:47:12 +09:00`
- 종료: `2026-04-04 03:50:50 +09:00`
- 소요 시간: `00:03:38`
- 병렬 수행: `아니오`
- 메모: `Canvas HUD`에 게임오버 오버레이와 재시작 안내를 추가하고, `IA_Restart`를 헤드리스 입력 자산 경로와 `DA_DefaultInputConfig`에 연결했다. `PlayerController`는 재시작 액션을 받아 `GameMode::RequestRestart()`를 호출하고, 게임모드는 현재 맵을 다시 열어 상태를 초기화한다. 빌드와 `CodexHarnessHeadlessSetup` 재실행은 계속 `0 error(s), 0 warning(s)`로 통과했고, `IA_Restart.uasset` 생성과 보고서 반영도 확인했다.

### 2026-04-04 | M6-P1 | 최종 정리와 검증

- 시작: `2026-04-04 03:50:50 +09:00`
- 종료: `2026-04-04 03:52:12 +09:00`
- 소요 시간: `00:01:22`
- 병렬 수행: `아니오`
- 메모: 최초 완성 기준 충족 여부를 최종 점검하고 하네스 문서를 완료 상태 기준으로 정리했다. 최종 검증은 `CodexHarnessEditor` 빌드 성공, `CodexHarnessHeadlessSetup` 재실행 `0 error(s), 0 warning(s)`, `BP_CodexHarnessEnemyCharacter.uasset`와 `IA_Restart.uasset` 생성 확인, 보고서 반영 확인을 기준으로 했다. 수동 플레이 테스트는 수행하지 않았다.
