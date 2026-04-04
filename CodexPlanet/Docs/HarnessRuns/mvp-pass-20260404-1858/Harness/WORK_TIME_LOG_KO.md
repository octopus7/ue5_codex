# CodexPlanet 작업 시간 로그

최종 수정일: 2026-04-04

## 사용 규칙

- 이 문서는 append 전용 로그로 사용한다.
- 각 항목은 하나의 단계 또는 하나의 독립 작업 단위를 뜻한다.
- 최소 기록 항목은 작업 ID, 작업명, 시작 시각, 종료 시각, 소요 시간, 병렬 수행 여부, 메모다.
- `generator/evaluator` 분리 운영을 사용한 경우 메모에 역할 분리와 메인 에이전트 중계 방식을 함께 적는다.
- 과거 기록은 수정하지 않고, 보정이 필요하면 새 항목으로 남긴다.

## 기록 형식

```md
### YYYY-MM-DD | <작업 ID> | <작업명>

- 시작: `YYYY-MM-DD HH:MM:SS +09:00`
- 종료: `YYYY-MM-DD HH:MM:SS +09:00`
- 소요 시간: `HH:MM:SS`
- 병렬 수행: `예` 또는 `아니오`
- 메모: ... (`generator/evaluator` 분리 운영을 썼다면 역할 분리와 중계 방식 포함)
```

## 기록

### 2026-04-04 | M0-P1 | 문서 하네스 작성과 초기 상태 기준선 정리

- 시작: `2026-04-04 17:20:53 +09:00`
- 종료: `2026-04-04 17:23:36 +09:00`
- 소요 시간: `00:02:43`
- 병렬 수행: `예`
- 메모: `CodexHarness`의 문서 역할만 참고하고, `CodexPlanet` 실제 상태에 맞는 초기 하네스 문서를 새로 작성했다. 헤드리스 자동화나 전용 애셋 파이프라인 같은 상위 프로젝트 전용 규칙은 가져오지 않았다.

### 2026-04-04 | M1-P1-DOC-REALIGN | 행성 경영 컨셉 기준으로 하네스 문서 재정렬

- 시작: `2026-04-04 17:24:40 +09:00`
- 종료: `2026-04-04 17:24:40 +09:00`
- 소요 시간: `00:00:00`
- 병렬 수행: `아니오`
- 메모: 사용자가 확정한 방향에 맞춰 탑다운 전제를 제거하고, `행성 회전`, `표면 프랍 배치`, `cute VOX 프랍` 중심으로 `PROJECT_HARNESS`, `CURRENT_PHASE`, `DECISION_LOG`, `STATE_SNAPSHOT`을 재작성했다. 문서 목적을 `진행 과정을 참고해 장기 수행 작업을 지시하는 기준 문서 유지`로 명시했다.

### 2026-04-04 | M1-P1-DOC-PLANET-RULES | 행성 본체 생성 규칙과 교체 가능 구조 반영

- 시작: `2026-04-04 17:27:30 +09:00`
- 종료: `2026-04-04 17:28:36 +09:00`
- 소요 시간: `00:01:06`
- 병렬 수행: `예`
- 메모: 행성 본체는 `VOX`가 아니라 `코드 생성된 약간의 노이즈 스피어`를 초기 기준으로 사용하고, 나중에 외부 메시로 교체 가능한 구조로 유지한다는 규칙을 하네스 문서 전반에 반영했다. `VOX`는 행성 표면 프랍에만 적용하는 방향으로 정리했다.

### 2026-04-04 | DOC-MILESTONE-RESET | 빈 프로젝트 기준 시작 상태로 마일스톤 표현 보정

- 시작: `2026-04-04 17:30:53 +09:00`
- 종료: `2026-04-04 17:31:33 +09:00`
- 소요 시간: `00:00:40`
- 병렬 수행: `아니오`
- 메모: `M0`, `M1`이 과거 구현 이력을 뜻하는 것처럼 보이지 않도록 문서를 수정했다. 현재 프로젝트는 거의 빈 상태에서 처음부터 시작하는 프로젝트이며, 이번 하네스 문서의 선행 단계는 문서 정리 기준선이라는 점을 명시했다.

### 2026-04-04 | DOC-STACKING-RULES | 적층 배치와 표면 노멀 정렬 규칙 반영

- 시작: `2026-04-04 17:32:33 +09:00`
- 종료: `2026-04-04 17:33:24 +09:00`
- 소요 시간: `00:00:51`
- 병렬 수행: `아니오`
- 메모: 프랍은 행성 표면뿐 아니라 이미 배치된 오브젝트 표면에도 적층 가능하며, 새 오브젝트는 접촉 표면 삼각형의 노멀 방향을 따라 정렬된다는 규칙을 하네스 문서 전반에 반영했다.

### 2026-04-04 | HARNESS-SEED-TOOLS | seed/live/archive 구조와 Python 스크립트 추가

- 시작: `2026-04-04 17:37:30 +09:00`
- 종료: `2026-04-04 17:39:43 +09:00`
- 소요 시간: `00:02:13`
- 병렬 수행: `예`
- 메모: `Docs/HarnessSeed`, `Docs/HarnessRuns`, `Tools/harness` 구조를 추가하고 Python 기반 `reset_harness.py`, `archive_harness.py`, `common.py`를 작성했다. 현재 `Docs/Harness`를 `Docs/HarnessSeed`로 복사했고, `initial-baseline` 아카이브를 생성한 뒤 실제 `reset_harness.py --force` 실행으로 seed 복원이 동작함을 확인했다. 이후 상태 문서를 갱신하고 그 상태를 다시 seed로 동기화했다.

### 2026-04-04 | DOC-PHASE-ID-RESET | 현재 단계 ID를 `M1-P0`로 보정

- 시작: `2026-04-04 17:43:03 +09:00`
- 종료: `2026-04-04 17:43:54 +09:00`
- 소요 시간: `00:00:51`
- 병렬 수행: `예`
- 메모: 현재 프로젝트가 거의 빈 상태에서 첫 구현을 시작하는 의미가 분명하도록 현재 live 단계 ID를 `M2-P1`에서 `M1-P0`으로 바꾸고, 다음 단계 후보도 `M1-P1`, `M1-P2`, `M1-P3`로 정리했다.

### 2026-04-04 | DOC-WBP-ASSET-RULES | 고리 NS/위성 추가 UI의 실존 WBP 규칙 반영

- 시작: `2026-04-04 17:45:27 +09:00`
- 종료: `2026-04-04 17:46:04 +09:00`
- 소요 시간: `00:00:37`
- 병렬 수행: `아니오`
- 메모: 행성 `고리 Niagara System` 또는 `위성`을 추가하는 UI는 에디터에서 확인 가능한 실존 `WBP` 애셋이어야 하며, 위젯 트리는 에디터 코드로 작성해 저장하고 런타임 `NativeConstruct` 조립 구현은 피한다는 규칙을 문서 전반에 반영했다.

### 2026-04-04 | DOC-AGENT-PARALLEL-RULES | generator/evaluator 분리 운영과 병렬 처리 규칙 반영

- 시작: `2026-04-04 17:49:47 +09:00`
- 종료: `2026-04-04 17:50:38 +09:00`
- 소요 시간: `00:00:51`
- 병렬 수행: `아니오`
- 메모: 상위 `CodexHarness`의 수행 방식 규칙을 가져와 `CodexPlanet` 하네스에 반영했다. 작업 시간을 줄일 수 있으면 병렬 수행을 허용하고, 토큰 비용보다 전체 리드타임 단축을 우선한다. 필요하면 `generator`와 `evaluator`를 분리 컨텍스트로 운용하되, 직접 통신 대신 메인 에이전트가 중계한다.

### 2026-04-04 | DOC-FULL-EXECUTION-INSTRUCTION | 전체 수행 지시문 문서 추가

- 시작: `2026-04-04 17:58:50 +09:00`
- 종료: `2026-04-04 18:00:30 +09:00`
- 소요 시간: `00:01:40`
- 병렬 수행: `아니오`
- 메모: 새 대화에서 그대로 사용할 수 있는 전체 수행 지시문을 `Docs/Harness/FULL_EXECUTION_INSTRUCTION_KO.md`로 추가했다. 수행 중 변경은 `Docs/Harness`의 live 문서에서만 반영하고 `Docs/HarnessSeed`는 보존용 원본으로 직접 수정하지 않는 규칙을 `README`, `DECISION_LOG`, `STATE_SNAPSHOT`에도 반영했다.

### 2026-04-04 | M1-P0 | 행성 뷰와 코어 런타임 골격 추가

- 시작: `2026-04-04 18:02:11 +09:00`
- 종료: `2026-04-04 18:19:45 +09:00`
- 소요 시간: `00:17:34`
- 병렬 수행: `예`
- 메모: `CodexPlanetGameMode`, `CodexPlanetPlayerController`, `CodexPlanetViewPawn`, `CodexPlanetActor`, `UNoisyPlanetMeshComponent`, `ICodexPlacementSurface`를 추가했다. `ProceduralMeshComponent` 플러그인을 활성화하고 `GlobalDefaultGameMode`를 연결했으며, `/Game/Core`, `/Game/Blueprints`, `/Game/Planets`, `/Game/Props`, `/Game/UI`, `SourceArt/Vox/Props` 폴더 골격을 만들었다. `CodexPlanetEditor` 빌드와 `UnrealEditor-Cmd.exe` 기반 `BasicMap` 헤드리스 로드를 검증했고, 초기 noisy sphere의 극점 퇴화 삼각형 문제를 수정한 뒤 phase를 닫았다.

### 2026-04-04 | M1-P1 | `EnhancedInput` 기반 드래그 입력과 트랙볼 회전

- 시작: `2026-04-04 18:19:45 +09:00`
- 종료: `2026-04-04 18:26:27 +09:00`
- 소요 시간: `00:06:42`
- 병렬 수행: `예`
- 메모: `CodexPlanetPlayerController` 안에서 코드 생성 `UInputMappingContext`, `UInputAction`을 사용해 좌클릭 드래그 입력을 `EnhancedInput` 경로로 연결했다. 카메라 기준 축을 사용하는 `BuildTrackballDeltaRotation` 수학 함수를 추가하고, 행성은 계산된 delta quaternion만 적용하도록 책임을 나눴다. `CodexPlanet.M1P1.TrackballRotationMath` 자동화 테스트와 `BasicMap` 헤드리스 로드로 입력 수학과 런타임 경로를 함께 확인했다.

### 2026-04-04 | M1-P2 | 행성/오브젝트 공통 표면 hit 판정과 적층 배치 프리뷰

- 시작: `2026-04-04 18:26:27 +09:00`
- 종료: `2026-04-04 18:32:54 +09:00`
- 소요 시간: `00:06:27`
- 병렬 수행: `예`
- 메모: `CodexPlaceablePropActor`, `CodexPlacementPreviewActor`를 추가하고 `PlayerController`에 `Tab` 배치 모드, 좌클릭 확정, 우클릭 취소, 커서 기반 표면 trace, 표면 노멀 정렬, 적층 가능한 기본 프랍 스폰 경로를 넣었다. 행성과 프랍이 같은 `ICodexPlacementSurface` 계약을 사용하도록 맞췄고, `CodexPlanet.M1P2.SurfaceAlignmentMath` 자동화 테스트와 `BasicMap` 헤드리스 로드로 정렬 수학과 런타임 경로를 확인했다. GUI 환경에서 실제 커서 감각과 수동 적층 플레이 확인은 후속 검증이 필요하다.

### 2026-04-04 | M1-P3 | 초기 5종 VOX 프랍 제작, import, 카탈로그 연결

- 시작: `2026-04-04 18:32:54 +09:00`
- 종료: `2026-04-04 18:48:20 +09:00`
- 소요 시간: `00:15:26`
- 병렬 수행: `예`
- 메모: `Tools/vox/generate_vox_props.py`로 `bridge`, `statue`, `palm_tree`, `rock`, `bush`의 `.vox`와 `.obj` source를 생성하고 manifest를 남겼다. `Tools/ue/import_generated_vox_props.py`를 full editor `ExecutePythonScript` 경로로 실행해 `/Game/Props/Vox` 아래 5종 static mesh 애셋을 import했다. `PlayerController`는 이 5종을 `Q/E`로 순환 선택하는 카탈로그를 로드하도록 갱신했고, `CodexPlanet.M1P3.PropCatalogAssets` 자동화 테스트로 asset load를 확인했다.

### 2026-04-04 | M1-P4 | 행성 `고리 NS`/`위성` 추가용 실존 `WBP` 애셋 구조 설계 및 구현

- 시작: `2026-04-04 18:48:20 +09:00`
- 종료: `2026-04-04 18:54:47 +09:00`
- 소요 시간: `00:06:27`
- 병렬 수행: `예`
- 메모: Python만으로 보호된 `WidgetTree`를 안정적으로 다루기 어려워 editor build 전용 `CodexEditorAssetLibrary`를 추가했다. 이 utility와 `Tools/ue/create_orbit_controls_widget.py`를 사용해 `/Game/UI/WBP_PlanetOrbitControls`를 생성했고, `RootBorder`, `TitleText`, `BodyText`, `ButtonRow`, `AddRingButton`, `AddSatelliteButton`, `FooterText`가 들어 있는 widget tree를 asset에 저장했다. `CodexPlanet.M1P4.OrbitControlsWidgetAsset` 자동화 테스트로 asset load와 핵심 위젯 존재를 검증했다.

### 2026-04-04 | M6-P0 | runtime 배치 HUD 연결과 최소 배치 슬롯 규칙

- 시작: `2026-04-04 18:54:47 +09:00`
- 종료: `2026-04-04 18:58:08 +09:00`
- 소요 시간: `00:03:21`
- 병렬 수행: `아니오`
- 메모: `PlayerController`가 `/Game/UI/WBP_PlanetOrbitControls_C`를 runtime viewport에 올리고 `FooterText`로 현재 선택 프랍과 남은 슬롯 수를 갱신하도록 연결했다. 최소 규칙은 `MaxPlacementSlots`/`RemainingPlacementSlots` 카운트로 시작하며, 슬롯이 0이면 프리뷰와 추가 배치를 막는다. headless map load 로그에서 orbit widget 생성과 `Selected=Bridge | Slots=20` HUD 상태를 확인했다.
