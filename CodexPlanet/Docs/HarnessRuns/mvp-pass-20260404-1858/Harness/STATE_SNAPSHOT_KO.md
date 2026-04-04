# CodexPlanet 상태 스냅샷

기준 시각: 2026-04-04

## 프로젝트 메타데이터

- 프로젝트 경로: `d:\github\ue5_codex\CodexPlanet`
- 엔진 연동: `5.7`
- 프로젝트 파일: `CodexPlanet.uproject`
- 기본 런타임 모듈: `CodexPlanet`
- 에디터 전용 플러그인: `ModelingToolsEditorMode` 활성화
- 런타임 플러그인: `ProceduralMeshComponent` 활성화

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

- `Source/CodexPlanet/` 아래에 코어 런타임 골격이 추가됐다.
- 현재 확인된 주요 소스 파일:
  - `Source/CodexPlanet/CodexPlanet.Build.cs`
  - `Source/CodexPlanet/CodexPlanet.cpp`
  - `Source/CodexPlanet/CodexPlanet.h`
  - `Source/CodexPlanet/Public/Core/CodexPlanetGameMode.h`
  - `Source/CodexPlanet/Private/Core/CodexPlanetGameMode.cpp`
  - `Source/CodexPlanet/Public/Player/CodexPlanetPlayerController.h`
  - `Source/CodexPlanet/Private/Player/CodexPlanetPlayerController.cpp`
  - `Source/CodexPlanet/Public/Player/CodexPlanetViewPawn.h`
  - `Source/CodexPlanet/Private/Player/CodexPlanetViewPawn.cpp`
  - `Source/CodexPlanet/Public/Planets/CodexPlanetActor.h`
  - `Source/CodexPlanet/Private/Planets/CodexPlanetActor.cpp`
  - `Source/CodexPlanet/Public/Planets/NoisyPlanetMeshComponent.h`
  - `Source/CodexPlanet/Private/Planets/NoisyPlanetMeshComponent.cpp`
  - `Source/CodexPlanet/Public/Interaction/CodexPlacementSurface.h`
  - `Source/CodexPlanet/Public/Props/CodexPlaceablePropActor.h`
  - `Source/CodexPlanet/Private/Props/CodexPlaceablePropActor.cpp`
  - `Source/CodexPlanet/Public/Props/CodexPlacementPreviewActor.h`
  - `Source/CodexPlanet/Private/Props/CodexPlacementPreviewActor.cpp`
- `Source/CodexPlanet/Private/Tests/CodexPlanetTrackballRotationTest.cpp`
- `CodexPlanetGameMode`가 기본 진입 시 행성 액터를 보장한다.
- `CodexPlanetPlayerController`는 관측 중인 행성 참조, `EnhancedInput` 매핑 생성, 드래그 상태 관리, 트랙볼 회전, 배치 모드, 커서 trace, 프리뷰 업데이트, `Q/E` 기반 프랍 카탈로그 순환을 맡는다.
- `CodexPlanetViewPawn`은 행성 중심 카메라 앵커 역할을 가진다.
- `CodexPlanetActor`는 교체 가능한 표면 책임의 시작점이며 `ICodexPlacementSurface`를 구현한다.
- `UNoisyPlanetMeshComponent`가 초기 noisy sphere 메시와 충돌 표면을 생성한다.
- `CodexPlaceablePropActor`가 적층 가능한 기본 프랍 표면 역할을 한다.
- `CodexPlacementPreviewActor`가 배치 프리뷰 메시를 담당한다.
- `BuildTrackballDeltaRotation`, `BuildSurfaceAlignedRotation` 순수 함수와 자동화 테스트가 추가돼 입력/정렬 수학을 헤드리스로 검증할 수 있다.
- `Tools/vox/generate_vox_props.py`가 `.vox`와 `.obj` source를 함께 생성한다.
- `Tools/ue/import_generated_vox_props.py`가 `/Game/Props/Vox` static mesh import를 수행한다.
- `CodexEditorAssetLibrary`가 editor build에서 `WBP_PlanetOrbitControls` 위젯 트리 생성/저장을 수행한다.
- `Tools/ue/create_orbit_controls_widget.py`는 해당 editor utility를 호출하는 래퍼다.
- `PlayerController`가 `WBP_PlanetOrbitControls_C`를 runtime viewport에 올리고 `FooterText`를 갱신한다.
- 최소 배치 규칙은 `남은 슬롯 수` 카운트로 시작하며, 슬롯이 0이면 추가 배치를 막는다.
- 에디터 자동화 모듈이나 커맨드렛은 아직 없다.

## 빌드 의존성 상태

- 런타임 모듈 의존성에는 `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `ProceduralMeshComponent`가 들어 있다.
- editor build에서는 `AssetTools`, `Slate`, `SlateCore`, `UMG`, `UMGEditor`, `UnrealEd`를 추가로 사용한다.
- `AIModule`, `NavigationSystem` 등은 아직 추가되지 않았다.

## 설정 상태

- `Config/DefaultEngine.ini`
  - `GameDefaultMap=/Game/Maps/BasicMap`
  - `EditorStartupMap=/Game/Maps/BasicMap`
  - `GlobalDefaultGameMode=/Script/CodexPlanet.CodexPlanetGameMode`
- `Config/DefaultInput.ini`
  - `DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput`
  - `DefaultInputComponentClass=/Script/EnhancedInput.EnhancedInputComponent`
- `Config/DefaultGame.ini`
  - `CommonUI` 기본 설정과 `ProjectID`만 존재한다.

## 콘텐츠 상태

- 현재 확인된 콘텐츠는 `/Game/Maps/BasicMap`과 초기 폴더 골격이다.
- 생성된 폴더:
  - `/Game/Core`
  - `/Game/Blueprints`
  - `/Game/Planets`
  - `/Game/Props`
  - `/Game/UI`
- `SourceArt/Vox/Props` 폴더가 생성됐다.
- `SourceArt/Vox/Props`
  - `bridge.vox`, `statue.vox`, `palm_tree.vox`, `rock.vox`, `bush.vox`
  - 각 프랍의 Unreal import용 `.obj`
  - `manifest.json`
- 행성 메시 또는 행성 액터용 uasset은 아직 없다.
- 코드 생성 행성 시스템은 C++ `UNoisyPlanetMeshComponent`로 추가됐다.
- `/Game/Props/Vox`
  - `SM_VOX_Bridge`
  - `SM_VOX_Statue`
  - `SM_VOX_PalmTree`
  - `SM_VOX_Rock`
  - `SM_VOX_Bush`
- 현재 배치 시스템은 이 5종 static mesh를 카탈로그로 로드하도록 연결됐다.
- 프로젝트 전용 Blueprint 애셋은 아직 없다.
- `/Game/UI/WBP_PlanetOrbitControls`
  - `RootBorder`
  - `TitleText`
  - `BodyText`
  - `ButtonRow`
  - `AddRingButton`
  - `AddSatelliteButton`
  - `FooterText`

## 문서 상태

- `Docs/HarnessSeed`가 존재하며 현재 하네스 초기 기준 상태를 보존한다.
- `Docs/Harness/README.md`
- `Docs/Harness/FULL_EXECUTION_INSTRUCTION_KO.md`
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
- 문서에는 병렬 수행 허용 규칙과 `generator/evaluator` 분리 운영 원칙이 반영되어 있다.
- 새 대화에서 사용할 수 있는 전체 수행 지시문은 live 문서 `Docs/Harness/FULL_EXECUTION_INSTRUCTION_KO.md`에 저장한다.
- 수행 중 `Docs/HarnessSeed`는 보존용 원본으로 취급하며 직접 수정하지 않는다.

## 현재 공백

- 복잡한 배치 UI 없음
- 복잡한 경영/배치 규칙 레이어 없음

## 현재 리스크

- 트랙볼 회전 수학은 테스트했지만 실제 손맛은 비헤드리스 입력 확인이 한 번 더 필요하다.
- 배치 모드와 회전 모드의 실제 커서 감각은 GUI 에디터/실행 환경에서 한 번 더 확인이 필요하다.
- VOX 프랍은 생성/로드됐지만 실제 배치 손맛 기준 스케일과 피벗은 수동 플레이에서 추가 튜닝이 필요할 수 있다.
- 현재 슬롯 규칙은 최소 카운트 방식이라 장기적인 경영/배치 규칙으로는 아직 얕다.

## 바로 다음 추천 작업

- 현재 결과를 `M7` 관점에서 정리하고, 수동 플레이 감각 조정 및 확장 phase를 정의한다.
- 하네스를 다시 시작하고 싶을 때는 `Tools/harness/reset_harness.py --force`를 사용한다.
- 리드타임 단축 이득이 있으면 탐색, 구현, 검증을 병렬로 나누는 방식을 허용한다.
- UI 단계에서는 `고리 NS`/`위성` 추가용 `WBP`를 에디터 코드로 저장하는 경로를 마련한다.

## 최근 검증 결과

- `CodexPlanetEditor Win64 Development` 빌드 성공
- `UnrealEditor-Cmd.exe <uproject> /Game/Maps/BasicMap -game -nullrhi` 헤드리스 맵 로드 성공
- 헤드리스 실행 로그에서 `Game class is 'CodexPlanetGameMode'` 확인
- 초기 noisy sphere 생성에서 발생하던 극점 퇴화 삼각형 경고를 수정한 뒤 `Input trimesh contains ... bad triangles` 경고가 사라짐
- 자동화 테스트 `CodexPlanet.M1P1.TrackballRotationMath` 성공
- 자동화 테스트 `CodexPlanet.M1P2.SurfaceAlignmentMath` 성공
- Unreal full editor `ExecutePythonScript` 경로로 5종 static mesh import 성공
- 자동화 테스트 `CodexPlanet.M1P3.PropCatalogAssets` 성공
- Unreal full editor `ExecutePythonScript` 경로로 `/Game/UI/WBP_PlanetOrbitControls` 생성 성공
- 자동화 테스트 `CodexPlanet.M1P4.OrbitControlsWidgetAsset` 성공
- 헤드리스 맵 로드에서 `Orbit controls widget created and added to viewport.` 로그 확인
- 헤드리스 맵 로드에서 `Placement HUD state | Selected=Bridge | Slots=20` 로그 확인

## 갱신 규칙

- 코드, 설정, 콘텐츠, 문서 구조가 달라지면 이 문서를 갱신한다.
- 현재 단계가 바뀌면 공백과 리스크도 함께 조정한다.
