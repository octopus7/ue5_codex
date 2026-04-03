# 현재 프로젝트 상태 스냅샷

기준 시각: 2026-04-04

## 프로젝트 메타데이터

- 프로젝트 경로: `현재 작업 디렉터리 기준 프로젝트 루트`
- 엔진 연동: `5.7`
- 프로젝트 파일: `<PROJECT_NAME>.uproject`
- 기본 런타임 모듈: `<PROJECT_RUNTIME_MODULE>`
- 에디터 자동화 모듈: `<PROJECT_EDITOR_MODULE>`
- 에디터 전용 플러그인: `ModelingToolsEditorMode` 활성화

## 코드 상태

- `Source/<PROJECT_RUNTIME_MODULE>/`에 기본 런타임 모듈이 존재한다.
- `Source/<PROJECT_EDITOR_MODULE>/`에 에디터 자동화 모듈이 존재한다.
- 현재 확인된 주요 소스 파일:
  - `Source/<PROJECT_RUNTIME_MODULE>/<PROJECT_RUNTIME_MODULE>.Build.cs`
  - `Source/<PROJECT_RUNTIME_MODULE>/<PROJECT_RUNTIME_MODULE>.cpp`
  - `Source/<PROJECT_RUNTIME_MODULE>/<PROJECT_RUNTIME_MODULE>.h`
  - `Source/<PROJECT_NAME>.Target.cs`
  - `Source/<PROJECT_EDITOR_MODULE>.Target.cs`
  - `Source/<PROJECT_EDITOR_MODULE>/<PROJECT_EDITOR_MODULE>.Build.cs`
  - `Source/<PROJECT_EDITOR_MODULE>/Private/<PROJECT_EDITOR_MODULE>Module.cpp`
  - `Source/<PROJECT_EDITOR_MODULE>/Public/Commandlets/<PROJECT_HEADLESS_SETUP_COMMANDLET>Commandlet.h`
  - `Source/<PROJECT_EDITOR_MODULE>/Private/Commandlets/<PROJECT_HEADLESS_SETUP_COMMANDLET>Commandlet.cpp`
- 게임플레이 클래스는 아직 없다.
- `GameMode`, `PlayerController`, `Character`, `Enemy`, `HUD`, `Widget` 런타임 클래스는 아직 없다.
- 에디터에서 교체 가능한 Blueprint 파생 런타임 클래스도 아직 없다.

## 빌드 의존성 상태

- 런타임 모듈 의존성에는 `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`이 들어 있다.
- 에디터 자동화 모듈 의존성에는 `AssetRegistry`, `<PROJECT_RUNTIME_MODULE>`, `UnrealEd`가 들어 있다.
- `UMG`, `Slate`, `SlateCore`, `AIModule`, `NavigationSystem` 등은 아직 런타임 모듈 의존성에 추가되지 않았다.

## 설정 상태

- `Config/DefaultEngine.ini`
  - `GameDefaultMap=/Game/Maps/BasicMap`
  - `EditorStartupMap=/Game/Maps/BasicMap`
  - `GlobalDefaultGameMode` 항목은 아직 없다.
- `Config/DefaultInput.ini`
  - `DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput`
  - `DefaultInputComponentClass=/Script/EnhancedInput.EnhancedInputComponent`
- `Config/DefaultGame.ini`
  - `CommonUI` 기본 설정만 존재한다.
- `<PROJECT_NAME>.uproject`
  - 런타임 모듈 `<PROJECT_RUNTIME_MODULE>`
  - 에디터 모듈 `<PROJECT_EDITOR_MODULE>`

## 콘텐츠 상태

- 현재 확인된 주요 맵은 `/Game/Maps/BasicMap`이다.
- 헤드리스 제작 기반용 머터리얼 `/Game/<PROJECT_CONTENT_ROOT>/Materials/M_VoxBase`가 존재한다.
- 플레이어, 적, 무기, UI, 입력 애셋은 아직 없다.
- `GameMode`를 포함한 Blueprint 연결용 클래스 애셋도 아직 없다.
- `.vox`에서 import 된 `StaticMesh` 애셋도 아직 없다.
- 화면에 보이는 플레이어용 메시 애셋 연결도 아직 없다.
- 캐릭터 Blueprint 내부 메시 컴포넌트에 할당된 프로젝트 메시 애셋도 아직 없다.
- `IA_*`, `IMC_*`, `DA_*InputConfig` 입력 애셋도 아직 없다.

## 소스 아트 및 자동화 상태

- `SourceArt/Vox/SM_Vox_TestCube_01.vox`가 존재한다.
- `Saved/HeadlessSetup/<PROJECT_HEADLESS_SETUP_COMMANDLET>Report.txt`가 최근 실행 결과를 기록한다.
- `<PROJECT_HEADLESS_SETUP_COMMANDLET>` 커맨드렛은 `SourceArt/Vox`, `Saved/HeadlessSetup`, `M_VoxBase`를 보장한다.
- `UnrealEditor-Cmd` 재실행 기준으로 커맨드렛은 무경고 성공이 확인됐다.

## 문서 운영 상태

- `Docs/Harness/WORK_TIME_LOG_KO.md`를 단계별 시간 기록용 append 로그로 사용한다.
- 병렬 수행 허용 규칙과 Blueprint 파생 클래스 기준 연결 원칙이 문서에 반영되어 있다.
- `generator/evaluator` 분리 컨텍스트 협업과 메인 에이전트 중계 원칙이 문서에 반영되어 있다.
- 실제 분리 운영 사용 내역은 `CURRENT_PHASE_KO.md`와 `WORK_TIME_LOG_KO.md`에 남기도록 한다.
- 직접 C++ 연결 금지, 커맨드렛 기반 BP 생성, VOX `StaticMesh` 가시 연결 요구가 문서에 반영되어 있다.
- `IA_*`, `IMC_*`, `DA_*InputConfig` 실애셋 생성과 DA 집계 입력 구조가 문서에 반영되어 있다.
- 현재 `CURRENT_PHASE_KO.md`는 다음 단계 `M1-P1`를 가리킨다.

## 현재 공백

- 플레이어 입력 구조 없음
- `IA_*`, `IMC_*`, `DA_*InputConfig` 입력 애셋 없음
- 입력을 집계하는 Data Asset 레이어 없음
- 플레이어 이동 없음
- 카메라 없음
- 조준 없음
- 전투 없음
- 적 없음
- HUD 없음
- 게임 흐름 없음
- Blueprint 파생 런타임 클래스 레이어 없음
- `.vox`를 실제 메시 애셋으로 변환하는 본격 import 단계 없음
- 프로젝트 또는 맵에 연결된 구체적인 Blueprint GameMode/Pawn/PlayerController 없음
- 화면에 보이는 플레이어 메시 없음
- 메시 컴포넌트에 프로젝트 메시 애셋이 할당된 캐릭터 Blueprint 없음
- 현재 상태로는 이동을 구현해도 보이지 않는 플레이어가 될 위험이 큼

## 현재 리스크

- `EnhancedInput` 기반 실제 입력 에셋 생성 경로가 아직 없다.
- 플레이어가 직접 참조할 `DA_*InputConfig` 구조와 그에 연결된 `IA_*`/`IMC_*` 실애셋이 아직 없다.
- 에디터 GUI를 열지 않는 제약 때문에, 향후 입력 에셋과 UI 애셋 생성은 커맨드렛 기반 자동화가 계속 필요하다.
- VOX 파이프라인은 현재 베이스 머터리얼과 샘플 입력까지만 구축돼 있고, 실제 메시 import 자동화는 다음 확장이 필요하다.
- 에디터에서 교체 가능한 Blueprint 파생 클래스 레이어가 아직 없어 다음 단계에서 기본 연결 정리가 필요하다.
- `GlobalDefaultGameMode`와 기본 Pawn/Controller 연결이 아직 비어 있어 직접 플레이 연결 구조가 없다.
- 실제로 보이는 플레이어를 만들기 위한 VOX `StaticMesh` import와 메시 연결이 아직 없다.

## 바로 다음 추천 작업

- `CURRENT_PHASE_KO.md` 기준으로 `GameMode`, `PlayerController`, `Character` 골격과 구체적인 Blueprint 연결 애셋을 추가한다.
- VOX 기반 `StaticMesh` import와 플레이어 가시 메시 연결을 커맨드렛 경로로 올린다.
- `IA_*`, `IMC_*`, `DA_*InputConfig` 실애셋과 DA 기반 입력 연결 레이어를 추가한다.
- 그 다음 `EnhancedInput` 기반 이동과 카메라, 조준을 올린다.

## 갱신 규칙

- 코드, 설정, 애셋 구조가 달라질 때마다 이 문서를 갱신한다.
- 단계 완료 후에는 새 클래스, 새 모듈, 새 애셋 루트, 새 리스크를 반영한다.
