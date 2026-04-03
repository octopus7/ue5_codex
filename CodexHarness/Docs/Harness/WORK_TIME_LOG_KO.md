# CodexHarness 작업 시간 로그

최종 수정일: 2026-04-04

## 사용 규칙

- 이 문서는 append 전용 로그로 운영한다.
- 각 항목은 하나의 수행 단계 또는 독립 작업 단위를 나타낸다.
- 최소 기록 필드는 작업 ID 또는 단계 ID, 작업명, 시작 시각, 종료 시각, 소요 시간, 병렬 수행 여부, 메모다.
- `generator/evaluator` 분리 운영을 사용한 경우 메모에 역할 분리와 메인 에이전트 중계 방식을 함께 적는다.
- 과거 기록은 수정하지 않고, 보정이 필요하면 새 항목을 추가해 정정한다.

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

### 2026-04-04 | M0-P1-DOCS | 하네스 문서 운영 규칙 보강

- 시작: `2026-04-04 01:53:15 +09:00`
- 종료: `2026-04-04 01:54:58 +09:00`
- 소요 시간: `00:01:43`
- 병렬 수행: `예`
- 메모: 병렬 수행 허용 규칙, 단계별 시간 append 로그, Blueprint 파생 클래스 기준 연결 원칙을 문서에 반영함.

### 2026-04-04 | M0-P1 | 헤드리스 애셋 제작 기반 정립

- 시작: `2026-04-04 02:02:26 +09:00`
- 종료: `2026-04-04 02:09:12 +09:00`
- 소요 시간: `00:06:46`
- 병렬 수행: `예`
- 메모: `CodexHarnessEditor` 모듈, `CodexHarnessHeadlessSetup` 커맨드렛, `M_VoxBase`, 샘플 `.vox`, 자동화 보고서 경로를 추가하고 에디터 빌드 및 커맨드렛 재실행 무경고 검증까지 완료함.

### 2026-04-04 | DOCS-HARNESS-FEEDBACK-COMBAT | 전투 피드백과 판정 규칙 문서 반영

- 시작: `2026-04-04 05:01:46 +09:00`
- 종료: `2026-04-04 05:03:49 +09:00`
- 소요 시간: `00:02:03`
- 병렬 수행: `아니오`
- 메모: 플레이어 피격 반동, 피격 `Niagara System`, 카메라 흔들림, `DA_*GlobalFxConfig`와 `GameInstance` 전역 접근 규칙, 적 명중용 라인트레이스 채널 요구사항을 하네스 문서에 반영함.

### 2026-04-04 | T2-REBASE-M6 | `Harness_T1` 검증 스냅샷 통합, 누락 기능 보강, 최종 검증

- 시작: `2026-04-04 05:10:29 +09:00`
- 종료: `2026-04-04 05:22:19 +09:00`
- 소요 시간: `00:11:50`
- 병렬 수행: `예`
- 메모: 읽기 전용 `evaluator` 컨텍스트가 `Harness_T1` 구현 범위, 검증 공백, 문서 충돌 범위를 병렬로 점검했고 메인 컨텍스트가 결과를 중계받아 `Harness_T2`에 `CodexHarness` 경로만 선택 통합했다. 이후 현재 하네스 기준과 어긋나 있던 `PlayerWeaponTrace`(`ECC_GameTraceChannel1`) 공격 판정 채널과 `BP_CodexHarnessPlayerHitCameraShake`/`DA_DefaultEffectsConfig` 기반 피격 카메라 흔들림 연결을 로컬에서 추가했다. `Build.bat CodexHarnessEditor ... -NoHotReloadFromIDE`와 `UnrealEditor-Cmd.exe ... -run=CodexHarnessHeadlessSetup ... -NoHotReloadFromIDE`를 현재 작업 트리에서 재실행해 모두 성공했고, 자동화 보고서에 `EnemyCharacter`, `RestartAction`, `PlayerHitCameraShake`, `EffectsConfig` 반영을 확인했다. 마지막으로 하네스 문서 5종을 현재 브랜치 기준 완료 상태로 정렬했다.

### 2026-04-04 | M6-P2-VOX-CHICKENS | 플레이어 흰 닭 / 적 붉은 닭 VOX 생성 및 연결

- 시작: `2026-04-04 06:04:08 +09:00`
- 종료: `2026-04-04 06:26:43 +09:00`
- 소요 시간: `00:22:35`
- 병렬 수행: `아니오`
- 메모: `CodexHarnessHeadlessSetup` 커맨드렛을 확장해 `SourceArt/Vox/SM_Vox_PlayerChicken_White.vox`와 `SourceArt/Vox/SM_Vox_EnemyChicken_Red.vox`를 헤드리스로 생성하고, `/Game/CodexHarness/Vox/SM_Vox_PlayerChicken_White`와 `/Game/CodexHarness/Vox/SM_Vox_EnemyChicken_Red`로 import하도록 변경했다. `BP_CodexHarnessCharacter`는 흰 닭 메시, `BP_CodexHarnessEnemyCharacter`는 붉은 닭 메시를 `DefaultVisualMesh -> VisualMeshComponent` 경로로 사용하도록 갱신했다. 빌드 재검증 중 프로젝트 `UnrealEditor.exe` 점유로 링크가 한 차례 실패했지만 에디터 종료 후 `Build.bat CodexHarnessEditor ... -NoHotReloadFromIDE`와 `UnrealEditor-Cmd.exe ... -run=CodexHarnessHeadlessSetup ... -NoHotReloadFromIDE`를 재실행해 모두 성공했고, 자동화 보고서에 두 VOX 소스와 두 `StaticMesh` 경로 반영을 확인했다. 마지막으로 하네스 문서 5종을 새 외형 상태 기준으로 갱신했다.
