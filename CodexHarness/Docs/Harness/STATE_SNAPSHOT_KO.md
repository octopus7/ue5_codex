# CodexHarness 상태 스냅샷

기준 시각: 2026-04-04

## 프로젝트 메타데이터

- 프로젝트 경로: `d:\github\ue5_codex\CodexHarness`
- 엔진 연동: `5.7`
- 프로젝트 파일: `CodexHarness.uproject`
- 기본 모듈: `CodexHarness`
- 에디터 전용 플러그인: `ModelingToolsEditorMode` 활성화

## 코드 상태

- `Source/CodexHarness/`에 기본 런타임 모듈만 존재한다.
- 현재 확인된 소스 파일:
  - `Source/CodexHarness/CodexHarness.Build.cs`
  - `Source/CodexHarness/CodexHarness.h`
  - `Source/CodexHarness/CodexHarness.cpp`
  - `Source/CodexHarness.Target.cs`
  - `Source/CodexHarnessEditor.Target.cs`
- 게임플레이 클래스는 아직 없다.
- `GameMode`, `PlayerController`, `Character`, `Enemy`, `HUD`, `Widget`, `Commandlet`, 에디터 전용 모듈은 아직 없다.

## 빌드 의존성 상태

- 현재 런타임 모듈 의존성에는 `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`이 들어 있다.
- `UMG`, `Slate`, `SlateCore`, `AIModule`, `NavigationSystem` 등은 아직 모듈 의존성에 추가되지 않았다.
- 에디터 자동화용 별도 모듈도 아직 존재하지 않는다.

## 설정 상태

- `Config/DefaultEngine.ini`
  - `GameDefaultMap=/Game/Maps/BasicMap`
  - `EditorStartupMap=/Game/Maps/BasicMap`
- `Config/DefaultInput.ini`
  - `DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput`
  - `DefaultInputComponentClass=/Script/EnhancedInput.EnhancedInputComponent`
- `Config/DefaultGame.ini`
  - `CommonUI` 기본 설정만 존재한다.

## 콘텐츠 상태

- 현재 확인된 주요 콘텐츠는 `/Game/Maps/BasicMap` 뿐이다.
- 플레이어, 적, 무기, UI, 입력, 머터리얼, VOX 관련 애셋은 아직 없다.
- `SourceArt/Vox/` 경로도 아직 없다.

## 현재 공백

- 플레이어 입력 구조 없음
- 플레이어 이동 없음
- 카메라 없음
- 조준 없음
- 전투 없음
- 적 없음
- HUD 없음
- 게임 흐름 없음
- 헤드리스 애셋 생성 파이프라인 없음

## 현재 리스크

- `EnhancedInput` 기반 실제 입력 에셋 생성 경로가 아직 없다.
- 에디터 GUI를 열지 않는 제약 때문에, 향후 입력 에셋과 UI 애셋 생성은 커맨드렛 기반 자동화가 사실상 필요하다.
- VOX 메시 파이프라인을 아직 구현하지 않았기 때문에 임시 메시 제작 규칙은 문서에만 존재한다.

## 바로 다음 추천 작업

- `CURRENT_PHASE_KO.md` 기준으로 헤드리스 애셋 제작 기반을 먼저 만든다.
- 그 다음 게임플레이 기반 클래스와 입력, 이동, 카메라를 추가한다.

## 갱신 규칙

- 코드, 설정, 애셋 구조가 달라질 때마다 이 문서를 갱신한다.
- 단계 완료 후에는 새 클래스, 새 모듈, 새 애셋 루트, 새 리스크를 반영한다.
