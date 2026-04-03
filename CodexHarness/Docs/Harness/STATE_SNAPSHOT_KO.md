# CodexHarness 상태 스냅샷

기준 시각: 2026-04-04

## 프로젝트 메타데이터

- 프로젝트 경로: `d:\github\ue5_codex\CodexHarness`
- 엔진 연동: `5.7`
- 프로젝트 파일: `CodexHarness.uproject`
- 기본 모듈: `CodexHarness`
- 에디터 전용 플러그인: `ModelingToolsEditorMode` 활성화

## 코드 상태

- `Source/CodexHarness/`에 기본 런타임 모듈이 존재한다.
- `Source/CodexHarnessEditor/` 에디터 전용 모듈이 추가되었다.
- 현재 확인된 소스 파일:
  - `Source/CodexHarness/CodexHarness.Build.cs`
  - `Source/CodexHarness/CodexHarness.h`
  - `Source/CodexHarness/CodexHarness.cpp`
  - `Source/CodexHarness.Target.cs`
  - `Source/CodexHarnessEditor.Target.cs`
  - `Source/CodexHarness/Public/Game/CHTopDownGameMode.h`
  - `Source/CodexHarness/Private/Game/CHTopDownGameMode.cpp`
  - `Source/CodexHarness/Public/Player/CHPlayerController.h`
  - `Source/CodexHarness/Private/Player/CHPlayerController.cpp`
  - `Source/CodexHarness/Public/Player/CHPlayerCharacter.h`
  - `Source/CodexHarness/Private/Player/CHPlayerCharacter.cpp`
  - `Source/CodexHarnessEditor/CodexHarnessEditor.Build.cs`
  - `Source/CodexHarnessEditor/Public/CodexHarnessEditorModule.h`
  - `Source/CodexHarnessEditor/Private/CodexHarnessEditorModule.cpp`
  - `Source/CodexHarnessEditor/Public/CHBuildHarnessAssetsCommandlet.h`
  - `Source/CodexHarnessEditor/Private/CHBuildHarnessAssetsCommandlet.cpp`
- `CHTopDownGameMode`, `CHPlayerController`, `CHPlayerCharacter`가 추가되었다.
- `CHPlayerController`는 런타임 생성 `EnhancedInput` 이동 매핑, 마우스 월드 조준, 자동 스모크 검증 경로를 가진다.
- `CHPlayerController`는 기본 발사 입력과 `CHFireSmoke` 자동 발사 스모크 경로를 가진다.
- `CHPlayerController`는 `CHDamageSmoke` 자동 데미지 스모크와 사망 관측 경로를 가진다.
- `CHPlayerController`는 `R` 재시작 입력과 `CHRestartSmoke` 자동 재시작 스모크 경로를 가진다.
- `CHPlayerCharacter`는 카메라 평면 이동, 조준 월드 포인트 기반 바라보기 회전, `FCHFireResult` 기반 발사 결과 보관, `TakeDamage` 연동을 가진다.
- `UCHHealthComponent`가 공용 체력, 사망 상태, 체력 변화 이벤트를 담당한다.
- `ACHEnemyCharacter`가 추가되어 직접 추적 이동, 근접 공격, 공용 체력 경로를 사용한다.
- `CHTopDownGameMode`는 현재 웨이브와 살아 있는 적 수를 추적하고 다음 웨이브를 시작한다.
- `ACHGameHUD`가 체력, 웨이브, 적 수, 게임 오버 문구를 캔버스 텍스트로 표시한다.
- UMG `Widget` 애셋은 아직 없다.
- `CHBuildHarnessAssetsCommandlet`가 헤드리스 애셋 부트스트랩 진입점으로 추가되었다.

## 빌드 의존성 상태

- 현재 런타임 모듈 의존성에는 `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`이 들어 있다.
- 에디터 모듈 의존성에는 `AssetRegistry`, `AssetTools`, `MaterialEditor`, `UnrealEd`가 추가되었다.
- `UMG`, `Slate`, `SlateCore`, `AIModule`, `NavigationSystem` 등 런타임 게임플레이 확장용 의존성은 아직 추가되지 않았다.

## 설정 상태

- `Config/DefaultEngine.ini`
  - `GameDefaultMap=/Game/Maps/BasicMap`
  - `EditorStartupMap=/Game/Maps/BasicMap`
  - `GlobalDefaultGameMode=/Script/CodexHarness.CHTopDownGameMode`
- `CodexHarness.uproject`
  - `CodexHarnessEditor` 모듈이 `Default` 로딩 단계로 등록되었다.
- `Config/DefaultInput.ini`
  - `DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput`
  - `DefaultInputComponentClass=/Script/EnhancedInput.EnhancedInputComponent`
- `Config/DefaultGame.ini`
  - `CommonUI` 기본 설정만 존재한다.
- `Docs/Harness/PHASE_TIME_LOG_KO.md`
  - 단계 완료 시각과 소요 시간을 append 방식으로 기록한다.

## 콘텐츠 상태

- 현재 확인된 주요 콘텐츠는 `/Game/Maps/BasicMap` 뿐이다.
- 헤드리스 부트스트랩으로 아래 리소스가 추가되었다.
  - `SourceArt/Vox/Smoke/CH_SmokeCube.vox`
  - `/Game/TopDownShooter/Vox/Materials/M_VoxBase`
  - `Saved/Harness/M0_P1_BootstrapReport.txt`
- 플레이어, 적, 무기, UI, 입력 애셋은 아직 없다.

## 현재 공백

- `PROJECT_HARNESS_KO.md`의 최초 완성 기준은 현재 코드와 스모크 테스트 기준으로 충족되었다.
- 입력 구조는 런타임 생성 `EnhancedInput` 객체 기준으로만 존재하고 에셋은 없음
- 첫 발사 루프, 체력/사망, 적 추적/공격, 웨이브, HUD, 게임 오버/재시작 흐름이 모두 존재한다.
- `.vox -> 메시 애셋` 실제 변환 파이프라인 없음

## 현재 리스크

- `EnhancedInput` 기반 실제 입력 에셋 생성 경로가 아직 없다.
- 에디터 GUI를 열지 않는 제약 때문에, 향후 입력 에셋과 UI 애셋 생성은 커맨드렛 기반 자동화가 계속 필요하다.
- VOX 메시 파이프라인은 현재 `M_VoxBase`와 스모크 `.vox` 생성까지만 준비되었고, 실제 메시 변환은 아직 없다.
- 초기 이동은 직접 월드 오프셋 방식이어서 충돌 스윕과 이동 감각 보정이 아직 없다.
- 실제 마우스 디프로젝션 기반 조준은 헤드리스 환경에서 체감 확인이 어렵고, 현재 자동 검증은 고정 목표 기반 스모크에 의존한다.
- 적 추적은 직접 월드 오프셋 방식이어서 복잡한 지형과 충돌 회피는 아직 없다.
- HUD는 기능적으로 동작하지만 아직 AHUD 텍스트 기반 임시 표현이다.
- 게임 오버와 재시작은 동작하지만 연출과 UI polish는 아직 없다.

## 바로 다음 추천 작업

- 장기 목표는 달성되었고, 다음 작업은 `M6` 선택 정리 단계다.
- 우선순위가 남아 있다면 이동 감각 보정, HUD polish, VOX 실제 변환 파이프라인 순으로 진행한다.

## 갱신 규칙

- 코드, 설정, 애셋 구조가 달라질 때마다 이 문서를 갱신한다.
- 단계 완료 후에는 새 클래스, 새 모듈, 새 애셋 루트, 새 리스크를 반영한다.
