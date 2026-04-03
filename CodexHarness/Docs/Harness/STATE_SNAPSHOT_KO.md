# TopDownTestOne 상태 스냅샷

기준 시각: 2026-04-04

## 프로젝트 메타데이터

- 프로젝트 경로: `현재 작업 디렉터리 기준 프로젝트 루트`
- 엔진 연동: `5.7`
- 프로젝트 파일: `TopDownTestOne.uproject`
- 기본 런타임 모듈: `TopDownTestOne`
- 에디터 자동화 모듈: `TopDownTestOneEditor`
- 에디터 전용 플러그인: `ModelingToolsEditorMode` 활성화

## 코드 상태

- `Source/TopDownTestOne/`에 기본 런타임 모듈이 존재한다.
- `Source/TopDownTestOneEditor/`에 에디터 자동화 모듈이 존재한다.
- 현재 확인된 주요 소스 파일:
  - `Source/TopDownTestOne/TopDownTestOne.Build.cs`
  - `Source/TopDownTestOne/TopDownTestOne.cpp`
  - `Source/TopDownTestOne/TopDownTestOne.h`
  - `Source/TopDownTestOne.Target.cs`
  - `Source/TopDownTestOneEditor.Target.cs`
  - `Source/TopDownTestOneEditor/TopDownTestOneEditor.Build.cs`
  - `Source/TopDownTestOneEditor/Private/TopDownTestOneEditorModule.cpp`
  - `Source/TopDownTestOneEditor/Public/Commandlets/TopDownTestOneHeadlessSetupCommandlet.h`
  - `Source/TopDownTestOneEditor/Private/Commandlets/TopDownTestOneHeadlessSetupCommandlet.cpp`
- 게임플레이 클래스는 아직 없다.
- `GameMode`, `PlayerController`, `Character`, `Enemy`, `HUD`, `Widget` 런타임 클래스는 아직 없다.
- 에디터에서 교체 가능한 Blueprint 파생 런타임 클래스도 아직 없다.

## 빌드 의존성 상태

- 런타임 모듈 의존성에는 `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`이 들어 있다.
- 에디터 자동화 모듈 의존성에는 `AssetRegistry`, `TopDownTestOne`, `UnrealEd`가 들어 있다.
- `UMG`, `Slate`, `SlateCore`, `AIModule`, `NavigationSystem` 등은 아직 런타임 모듈 의존성에 추가되지 않았다.

## 설정 상태

- `Config/DefaultEngine.ini`
  - `GameDefaultMap=/Game/Maps/BasicMap`
  - `EditorStartupMap=/Game/Maps/BasicMap`
- `Config/DefaultInput.ini`
  - `DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput`
  - `DefaultInputComponentClass=/Script/EnhancedInput.EnhancedInputComponent`
- `Config/DefaultGame.ini`
  - `CommonUI` 기본 설정만 존재한다.
- `TopDownTestOne.uproject`
  - 런타임 모듈 `TopDownTestOne`
  - 에디터 모듈 `TopDownTestOneEditor`

## 콘텐츠 상태

- 현재 확인된 주요 맵은 `/Game/Maps/BasicMap`이다.
- 헤드리스 제작 기반용 머터리얼 `/Game/TopDownShooter/Materials/M_VoxBase`가 존재한다.
- 플레이어, 적, 무기, UI, 입력 애셋은 아직 없다.
- `GameMode`를 포함한 Blueprint 연결용 클래스 애셋도 아직 없다.

## 소스 아트 및 자동화 상태

- `SourceArt/Vox/SM_Vox_TestCube_01.vox`가 존재한다.
- `Saved/HeadlessSetup/TopDownTestOneHeadlessSetupReport.txt`가 최근 실행 결과를 기록한다.
- `TopDownTestOneHeadlessSetup` 커맨드렛은 `SourceArt/Vox`, `Saved/HeadlessSetup`, `M_VoxBase`를 보장한다.
- `UnrealEditor-Cmd` 재실행 기준으로 커맨드렛은 무경고 성공이 확인됐다.

## 문서 운영 상태

- `Docs/Harness/WORK_TIME_LOG_KO.md`를 단계별 시간 기록용 append 로그로 사용한다.
- 병렬 수행 허용 규칙과 Blueprint 파생 클래스 기준 연결 원칙이 문서에 반영되어 있다.
- `generator/evaluator` 분리 컨텍스트 협업과 메인 에이전트 중계 원칙이 문서에 반영되어 있다.
- 실제 분리 운영 사용 내역은 `CURRENT_PHASE_KO.md`와 `WORK_TIME_LOG_KO.md`에 남기도록 한다.
- 현재 `CURRENT_PHASE_KO.md`는 다음 단계 `M1-P1`를 가리킨다.

## 현재 공백

- 플레이어 입력 구조 없음
- 플레이어 이동 없음
- 카메라 없음
- 조준 없음
- 전투 없음
- 적 없음
- HUD 없음
- 게임 흐름 없음
- Blueprint 파생 런타임 클래스 레이어 없음
- `.vox`를 실제 메시 애셋으로 변환하는 본격 import 단계 없음

## 현재 리스크

- `EnhancedInput` 기반 실제 입력 에셋 생성 경로가 아직 없다.
- 에디터 GUI를 열지 않는 제약 때문에, 향후 입력 에셋과 UI 애셋 생성은 커맨드렛 기반 자동화가 계속 필요하다.
- VOX 파이프라인은 현재 베이스 머터리얼과 샘플 입력까지만 구축돼 있고, 실제 메시 import 자동화는 다음 확장이 필요하다.
- 에디터에서 교체 가능한 Blueprint 파생 클래스 레이어가 아직 없어 다음 단계에서 기본 연결 정리가 필요하다.

## 바로 다음 추천 작업

- `CURRENT_PHASE_KO.md` 기준으로 `GameMode`, `PlayerController`, `Character` 골격을 추가한다.
- 그 다음 `EnhancedInput` 기반 이동과 카메라, 조준을 올린다.

## 갱신 규칙

- 코드, 설정, 애셋 구조가 달라질 때마다 이 문서를 갱신한다.
- 단계 완료 후에는 새 클래스, 새 모듈, 새 애셋 루트, 새 리스크를 반영한다.
