# CodexPlanet 상태 스냅샷

기준 시각: 2026-04-04

## 프로젝트 메타데이터

- 프로젝트 경로: `d:\github\ue5_codex\CodexPlanet`
- 엔진 연동: `5.7`
- 프로젝트 파일: `CodexPlanet.uproject`
- 기본 런타임 모듈: `CodexPlanet`
- 에디터 전용 플러그인: `ModelingToolsEditorMode` 활성화

## 현재 확정된 방향

- 현재 프로젝트는 거의 비어 있는 상태에서 처음부터 시작하는 중이다.
- 실제 첫 구현 단계 ID는 `M1-P0`를 사용한다.
- 이 프로젝트는 탑다운 슈터와 무관하다.
- 현재 컨셉은 `단일 행성 경영/배치 프로토타입`이다.
- 핵심 입력은 `드래그 기반 트랙볼 회전`이다.
- 핵심 상호작용은 `행성 표면 프랍 배치`다.
- 배치 규칙은 `적층 가능`이 기본이다.
- 배치 정렬 기준은 `접촉 표면 삼각형 노멀`이다.
- 행성 본체는 초기 단계에서 `코드 생성된 약간의 노이즈 스피어`를 사용한다.
- 행성 본체는 나중에 외부 메시로 교체 가능한 구조를 목표로 한다.
- 아트 방향은 `귀여운 VOX 프랍`이다.
- 초기 우선 프랍은 `아치형 나무다리`, `석상`, `야자수`, `바위`, `수풀`이다.
- 향후 `고리 NS` 또는 `위성` 추가 UI는 실존 `WBP` 애셋으로 존재해야 한다.

## 코드 상태

- `Source/CodexPlanet/`에 기본 런타임 모듈만 존재한다.
- 현재 확인된 주요 소스 파일:
  - `Source/CodexPlanet/CodexPlanet.Build.cs`
  - `Source/CodexPlanet/CodexPlanet.cpp`
  - `Source/CodexPlanet/CodexPlanet.h`
  - `Source/CodexPlanet.Target.cs`
  - `Source/CodexPlanetEditor.Target.cs`
- `GameMode`, `PlayerController`, `Pawn`, `PlanetActor`, `GameInstance`, `HUD`, `Widget` 관련 런타임 클래스는 아직 없다.
- 에디터 자동화 모듈이나 커맨드렛은 아직 없다.

## 빌드 의존성 상태

- 런타임 모듈 의존성에는 `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`이 들어 있다.
- `Slate`, `SlateCore`, `UMG`, `AIModule`, `NavigationSystem` 등은 아직 추가되지 않았다.

## 설정 상태

- `Config/DefaultEngine.ini`
  - `GameDefaultMap=/Game/Maps/BasicMap`
  - `EditorStartupMap=/Game/Maps/BasicMap`
- `Config/DefaultInput.ini`
  - `DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput`
  - `DefaultInputComponentClass=/Script/EnhancedInput.EnhancedInputComponent`
- `Config/DefaultGame.ini`
  - `CommonUI` 기본 설정과 `ProjectID`만 존재한다.

## 콘텐츠 상태

- 현재 확인된 콘텐츠는 `/Game/Maps/BasicMap`이 사실상 전부다.
- 행성 메시 또는 행성 액터용 애셋은 아직 없다.
- 코드 생성 행성 시스템도 아직 없다.
- 프랍 배치용 애셋은 아직 없다.
- VOX 소스 프랍도 아직 없다.
- 프로젝트 전용 Blueprint 애셋도 아직 없다.
- `고리 NS`/`위성` 추가용 `WBP` 애셋도 아직 없다.
- 프로젝트 전용 콘텐츠 폴더 구조도 아직 없다.

## 문서 상태

- `Docs/HarnessSeed`가 존재하며 현재 하네스 초기 기준 상태를 보존한다.
- `Docs/Harness/README.md`
- `Docs/Harness/PROJECT_HARNESS_KO.md`
- `Docs/Harness/CURRENT_PHASE_KO.md`
- `Docs/Harness/DECISION_LOG_KO.md`
- `Docs/Harness/STATE_SNAPSHOT_KO.md`
- `Docs/Harness/WORK_TIME_LOG_KO.md`
- `Docs/HarnessRuns`는 run별 결과를 보관하는 아카이브 루트다.
- `Docs/HarnessRuns/initial-baseline` 아카이브가 존재한다.
- `Tools/harness/common.py`
- `Tools/harness/reset_harness.py`
- `Tools/harness/archive_harness.py`
- `Tools/harness/README.md`
- 프로젝트 초기 설정 요청서 `UE5_PROJECT_BOOTSTRAP_REQUEST_KO.md`가 루트에 존재한다.

## 현재 공백

- 행성 뷰 제어 구조 없음
- `PlanetActor` 없음
- 프로시저럴 noisy sphere 생성 없음
- 드래그 회전 입력 없음
- 행성/오브젝트 공통 표면 hit 판정 없음
- 배치 프리뷰 없음
- 프랍 배치 시스템 없음
- 적층 배치 시스템 없음
- 삼각형 노멀 기반 배치 정렬 없음
- VOX 프랍 소스 없음
- VOX 프랍 import 결과 없음
- 배치 UI 없음
- `고리 NS`/`위성` 추가용 실존 `WBP` 없음
- 에디터 코드로 저장되는 위젯 트리 생성 경로 없음
- 경영/배치 규칙 레이어 없음

## 현재 리스크

- 행성 회전과 카메라 회전 중 무엇을 기본 책임으로 둘지 초기에 분명히 정하지 않으면 입력 구조가 흔들릴 수 있다.
- 코드 생성 행성 표현과 향후 외부 메시 교체 구조를 분리하지 않으면 나중에 교체 비용이 커질 수 있다.
- 표면 배치 구현 전에 행성 좌표계, 표면 노멀 정렬, 피벗 기준이 정리되지 않으면 프랍 배치 품질이 불안정해질 수 있다.
- 행성과 프랍을 서로 다른 배치 규칙으로 구현하면 적층 시 예외 처리가 급격히 늘어날 수 있다.
- UI를 런타임 `NativeConstruct` 조립으로 밀어두면 에디터 확인 가능 자산이라는 요구를 만족하지 못할 수 있다.
- VOX 프랍의 기준 스케일과 피벗 규칙을 초기에 잡지 않으면 자산이 누적될수록 정리가 어려워진다.
- 콘텐츠 폴더 구조가 아직 없어서 초기 프랍과 Blueprint가 흩어질 가능성이 있다.

## 바로 다음 추천 작업

- `CURRENT_PHASE_KO.md` 기준으로 `GameMode`, `PlayerController`, 뷰 제어용 `Pawn`, `PlanetActor`를 추가한다.
- 기본 맵에서 행성을 중심으로 보는 화면 구도를 먼저 만든다.
- 그 다음 `EnhancedInput` 기반 드래그 회전을 올린다.
- 이후 행성과 오브젝트를 함께 다루는 표면 판정과 적층 배치를 추가한다.
- VOX 프랍 제작과 import는 배치 골격이 준비된 뒤 바로 이어서 붙인다.
- 하네스를 다시 시작하고 싶을 때는 `Tools/harness/reset_harness.py --force`를 사용한다.
- UI 단계에서는 `고리 NS`/`위성` 추가용 `WBP`를 에디터 코드로 저장하는 경로를 마련한다.

## 갱신 규칙

- 코드, 설정, 콘텐츠, 문서 구조가 달라지면 이 문서를 갱신한다.
- 현재 단계가 바뀌면 공백과 리스크도 함께 조정한다.
