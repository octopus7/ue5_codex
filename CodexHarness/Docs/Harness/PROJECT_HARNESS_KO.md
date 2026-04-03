# CodexHarness 프로젝트 하네스

최종 수정일: 2026-04-04

## 문서 목적

이 문서는 `CodexHarness`를 장기 작업으로 운영하기 위한 최상위 프로젝트 하네스다.
목표, 범위, 전역 제약, 구현 원칙, 마일스톤, 문서 운영 규칙을 고정한다.

이 프로젝트의 장기 작업은 아래 5개 문서를 함께 사용한다.
- `Docs/Harness/PROJECT_HARNESS_KO.md`
- `Docs/Harness/CURRENT_PHASE_KO.md`
- `Docs/Harness/DECISION_LOG_KO.md`
- `Docs/Harness/STATE_SNAPSHOT_KO.md`
- `Docs/Harness/WORK_TIME_LOG_KO.md`

## 프로젝트 요약

- 프로젝트명: `CodexHarness`
- 엔진 버전: `UE 5.7`
- 대상 플랫폼: `PC`
- 네트워크 모델: `Single Player`
- 장르 목표: 탑다운 슈터 프로토타입
- 기본 맵: `/Game/Maps/BasicMap`
- 현재 상태: 최초 완성 기준 충족. `Harness_T2` 작업 트리에서 `CodexHarnessEditor` 빌드, `CodexHarnessHeadlessSetup` 재실행, 생성 애셋과 자동화 보고서 검증까지 완료했다.

## 장기 목표

이 프로젝트의 목표는 바닥부터 시작하는 싱글플레이 PC용 탑다운 슈터 프로토타입을 구축하는 것이다.

최초 완성 기준은 아래와 같다.
- 플레이어가 `BasicMap`에서 자동 스폰된다.
- 플레이어는 화면에서 실제 메시로 보이며 투명 placeholder 상태가 아니다.
- 플레이어는 `WASD`로 이동한다.
- 플레이어는 마우스 커서를 향해 회전한다.
- 좌클릭으로 기본 무기를 발사하고, 적 명중 판정은 `PlayerWeaponTrace`(`ECC_GameTraceChannel1`) 라인트레이스로 처리한다.
- 적 1종이 스폰되어 플레이어를 추적하고 공격한다.
- 플레이어와 적은 체력, 피격, 사망 처리를 가진다.
- 플레이어는 피격 시 반동, 피격 `Niagara System`, 카메라 흔들림 피드백을 가진다.
- HUD에 체력, 웨이브, 남은 적 수가 표시된다.
- 플레이어가 죽으면 게임 오버와 재시작이 가능하다.

## 전역 구현 원칙

- 핵심 게임플레이 구조는 C++ 중심으로 구현한다.
- Blueprint와 UMG는 튜닝, 시각 구성, 에셋 연결, 레이아웃에 사용한다.
- 작업 시간을 줄일 수 있으면 탐색, 구현, 검증을 병렬 수행한다.
- 병렬 수행 여부 판단에서는 토큰 비용보다 총 작업 시간 단축을 우선한다.
- 필요하면 `generator` 에이전트와 `evaluator` 에이전트를 분리된 컨텍스트로 운영한다.
- `generator`는 초안 구현과 코드 생산, `evaluator`는 리뷰, 리스크 점검, 검증 관점 평가를 맡는다.
- 두 에이전트는 직접 통신하지 않고 메인 에이전트가 결과와 피드백을 중계한다.
- 구현은 항상 플레이 가능한 작은 단위로 끊는다.
- 한 단계가 끝날 때마다 현재 상태와 결정 사항을 문서에 반영한다.
- 사용자 승인 없이 범위를 자동으로 확장하지 않는다.

## 전역 작업 제약

- Unreal Editor GUI는 열지 않는다.
- 에디터 API가 필요한 작업은 `UnrealEditor-Cmd`와 커맨드렛으로 처리한다.
- 레거시 입력 대신 `EnhancedInput`을 사용한다.
- `IA_*`, `IMC_*`, `DA_*InputConfig`는 실제 프로젝트 애셋으로 존재해야 한다.
- 플레이어 런타임 클래스는 `IA_*`나 `IMC_*`를 직접 참조하지 않고, `DA_DefaultInputConfig` 같은 입력 집계 Data Asset만 참조한다.
- `DA_*GlobalFxConfig` 또는 동등한 전역 피드백/FX Data Asset은 실제 프로젝트 애셋으로 존재해야 하며, `GameInstance`가 이를 참조해 전역 접근 지점을 제공해야 한다.
- 피격 피드백에 필요한 `Niagara System`과 카메라 흔들림 레퍼런스는 전역 피드백/FX Data Asset에 집계한다.
- 실제 연결 대상은 반드시 구체적인 Blueprint 파생 클래스 애셋으로 유지한다.
- `GameMode`, `DefaultPawnClass`, `PlayerControllerClass`, `HUDClass`, 카메라 흔들림 클래스 참조 같은 기본 연결 지점에 직접 C++ 클래스를 꽂지 않는다.
- 기본 연결에 필요한 Blueprint 애셋 생성과 갱신은 `UnrealEditor-Cmd` 기반 커맨드렛으로 완료한다.
- 플레이어 공격의 적 명중 판정은 `PlayerWeaponTrace`(`ECC_GameTraceChannel1`) 라인트레이스 채널을 사용하고, 적은 해당 채널에 응답해야 한다.
- 화면에 보여야 하는 런타임 액터는 렌더링 가능한 실제 애셋이 연결되어야 하며, 빈 컴포넌트나 투명 placeholder 상태를 완료로 간주하지 않는다.
- 기존 프로젝트 루트, 모듈명 `CodexHarness`, 에디터 모듈 `CodexHarnessEditor`, 기본 맵 `/Game/Maps/BasicMap`은 유지한다.
- 기본 콘텐츠 루트는 `/Game/CodexHarness/` 아래로 정리한다.
- 멀티플레이, 인벤토리, 저장, 설정 메뉴, 영구 성장 시스템은 현재 범위 밖이다.
- 에셋 부족을 이유로 진행을 멈추지 않고 임시 구조로 먼저 완성한다.

## 헤드리스 애셋 생성 정책

### 공통 규칙

- 에디터 API가 필요한 애셋 생성은 `CodexHarnessEditor` 모듈과 커맨드렛으로 처리한다.
- 현재 기본 진입점은 `CodexHarnessHeadlessSetup` 커맨드렛이다.
- 실행 형식은 기본적으로 아래 패턴을 따른다.

```powershell
<UE_INSTALL_ROOT>\Engine\Binaries\Win64\UnrealEditor-Cmd.exe `
  <PROJECT_ROOT>\CodexHarness.uproject `
  -run=CodexHarnessHeadlessSetup -unattended -nop4 -nosplash -nullrhi -NoHotReloadFromIDE
```

- 자동화 결과 요약은 `Saved/HeadlessSetup/CodexHarnessHeadlessSetupReport.txt`에 기록한다.
- 런타임 연결에 필요한 `BP_*` GameMode, PlayerController, Character, EnemyCharacter, HUD 애셋도 커맨드렛으로 생성 또는 갱신 가능해야 한다.
- 런타임 입력 연결에 필요한 `IA_*`, `IMC_*`, `DA_*InputConfig` 애셋도 커맨드렛으로 생성 또는 갱신 가능해야 한다.
- 런타임 피드백 연결에 필요한 `DA_DefaultEffectsConfig`, `NS_PlayerHitReaction`, `BP_CodexHarnessPlayerHitCameraShake` 애셋도 커맨드렛으로 생성 또는 갱신 가능해야 한다.
- 기본 클래스 연결과 기본값 주입도 헤드리스 경로에서 완료되어야 한다.
- 가시 오브젝트가 필요하면 `.vox -> StaticMesh` import와 해당 메시의 Blueprint 연결까지 헤드리스 경로에서 완료되어야 한다.
- 플레이어 외형은 `BP_CodexHarnessCharacter` 내부의 렌더링 컴포넌트가 프로젝트 내 생성된 메시 애셋을 실제로 참조하는 상태여야 한다.
- 입력 설정은 헤드리스 경로에서 `DA_DefaultInputConfig`에 `IA_*`와 `IMC_*` 참조를 실제로 채우고, 플레이어 런타임 클래스가 그 DA를 참조하도록 완료되어야 한다.
- 전역 피드백/FX 접근도 헤드리스 경로에서 `BP_CodexHarnessGameInstance`가 `DA_DefaultEffectsConfig`를 참조하도록 완료되어야 한다.
- `PlayerWeaponTrace` 채널 정의와 적 응답 규칙은 코드, 설정, 문서에 함께 남긴다.

### 메시 생성 규칙

- 프로젝트에 새 메시가 필요하면 우선 `.vox` 소스 파일을 생성한다.
- `.vox` 해상도는 기본적으로 `64 x 64 x 64` 이내를 기준으로 한다.
- `.vox` 파일은 프로젝트 루트의 `SourceArt/Vox/` 아래에 저장하는 것을 기본 규칙으로 한다.
- `.vox`에서 생성된 Unreal 애셋은 `/Game/CodexHarness/Vox/` 아래에 저장하는 것을 기본 규칙으로 한다.
- `.vox`에서 생성된 메시의 기본 색 표현은 버텍스 컬러를 사용한다.
- 메시별 개별 머터리얼을 늘리지 않고, 공용 베이스 머터리얼 하나를 재사용한다.
- 공용 베이스 머터리얼 이름은 기본적으로 `M_VoxBase`를 사용한다.
- `M_VoxBase`는 최소한 `VertexColor RGB -> Base Color` 연결을 포함해야 한다.

## 역할 분리 원칙

### 에디터 친화적 클래스 연결 원칙

- 핵심 로직과 타입 정의는 C++ 베이스 클래스에 둔다.
- 맵, 프로젝트 설정, 데이터 레퍼런스, 위젯 연결, 스폰 연결은 반드시 구체적인 Blueprint 파생 클래스 애셋을 기준으로 연결한다.
- 기본 게임 모드는 `BP_CodexHarnessGameMode`, 플레이어는 `BP_CodexHarnessCharacter`, 컨트롤러는 `BP_CodexHarnessPlayerController`, HUD는 `BP_CodexHarnessHUD`를 기준으로 연결한다.
- `GameMode`, `DefaultPawnClass`, `PlayerControllerClass`, `HUDClass` 등에 C++ 클래스를 직접 꽂은 상태는 완료 상태로 인정하지 않는다.
- 나중에 에디터에서 교체할 가능성이 있는 클래스는 C++ 타입을 직접 하드코딩하지 않고 Blueprint 레이어를 통해 교체 가능하게 둔다.
- 화면에 보여야 하는 핵심 런타임 오브젝트는 실제 렌더링 컴포넌트와 메시 애셋이 연결된 상태여야 하며, 플레이어가 보이지 않는 상태는 완료 기준에 포함되지 않는다.
- 입력 연결도 같은 원칙을 따르며, 플레이어 런타임 클래스는 `IA_*`/`IMC_*`를 직접 들지 않고 `DA_DefaultInputConfig` 하나를 통해 입력 구성을 받는다.
- 전역 피드백 연결도 같은 원칙을 따르며, 플레이어 런타임 클래스는 `Niagara System`과 카메라 흔들림 애셋을 직접 하드코딩하지 않고 `BP_CodexHarnessGameInstance -> DA_DefaultEffectsConfig` 경로로 접근한다.

### C++ 담당 범위

- 게임 모드, 게임 상태 흐름
- 플레이어 컨트롤러, 플레이어 캐릭터
- 이동, 조준, 발사, 라인트레이스 공격 판정, 데미지, 체력, 피격, 사망
- 적 추적 및 공격 로직
- `GameInstance` 기반 전역 피드백/FX 접근
- 웨이브 스폰과 진행 규칙
- HUD에 제공할 데이터 구조와 이벤트
- 커맨드렛 및 에디터 자동화 코드

### Blueprint 및 에셋 담당 범위

- GameMode, Pawn, PlayerController, EnemyCharacter, HUD용 Blueprint 파생 애셋 생성 및 기본값 연결
- `IA_*`, `IMC_*`, `DA_DefaultInputConfig` 입력 애셋 생성 및 참조 구성
- `DA_DefaultEffectsConfig`, `NS_PlayerHitReaction`, `BP_CodexHarnessPlayerHitCameraShake` 생성 및 참조 구성
- 메쉬, 머터리얼, 이펙트 연결
- 입력 액션과 매핑 컨텍스트 에셋
- `PlayerWeaponTrace` 채널 이름과 응답 설정
- 밸런스 수치 조정
- 스폰 포인트, 레벨 배치

### 에이전트 협업 운영 원칙

- 병렬화 이득이 있으면 `generator`와 `evaluator`를 독립 컨텍스트로 분리해 운용할 수 있다.
- `generator`는 구현안 생성, 코드 작성, 초안 패치를 우선 담당한다.
- `evaluator`는 코드 리뷰, 회귀 리스크 탐지, 검증 포인트 정리, 테스트 관점 평가를 우선 담당한다.
- 두 에이전트는 서로 직접 대화하지 않고 메인 에이전트가 메시지와 산출물을 전달한다.
- 쓰기 충돌을 줄이기 위해 기본적으로 `generator`가 쓰기 담당, `evaluator`가 읽기 및 검증 담당을 우선한다.

## 마일스톤

### M0. 헤드리스 제작 파이프라인

- 에디터 전용 모듈 구조 추가
- 커맨드렛 실행 경로 추가
- `M_VoxBase` 생성 또는 보장
- `.vox -> 프로젝트 애셋` 변환 루트 구축
- 최소 1개 테스트 입력과 테스트 메시 애셋 생성 흐름 정리
- 현재 상태: 완료

### M1. 게임플레이 기반

- `GameMode`, `PlayerController`, `Character` 골격 추가
- 커맨드렛 기반 `BP_*` GameMode, PlayerController, Character, HUD 생성 및 기본 연결
- 기본 스폰 흐름
- 기본 카메라 구조
- 플레이어 가시 표현용 VOX `StaticMesh` import 및 메시 연결
- `IA_*`, `IMC_*`, `DA_DefaultInputConfig` 실애셋 생성
- 플레이어가 `DA_DefaultInputConfig`만 참조하는 `EnhancedInput` 연결 구조
- 현재 상태: 완료

### M2. 이동과 조준

- `WASD` 이동
- 마우스 월드 위치 조준
- 바라보기 회전
- 현재 상태: 완료

### M3. 전투 루프

- 기본 발사 입력
- `PlayerWeaponTrace` 기반 히트스캔 판정
- 체력 컴포넌트
- 피격과 사망 처리
- 현재 상태: 완료

### M4. 적과 웨이브

- 적 1종
- 추적 이동
- 공격 처리
- 플레이어 피격 반동
- 플레이어 피격 `Niagara System`
- 플레이어 피격 카메라 흔들림
- 웨이브 스폰
- 적 수 추적
- 현재 상태: 완료

### M5. HUD와 게임 오버

- 체력 표시
- 웨이브 표시
- 남은 적 수 표시
- 게임 오버 UI
- 재시작 흐름
- 현재 상태: 완료

### M6. 폴리시와 정리

- 구조 정리
- 검증 결과 정리
- 문서 정합성 점검
- 현재 상태: 완료

## 범위

### 포함 범위

- 탑다운 슈터 프로토타입의 코어 플레이 루프
- C++ 기반 핵심 구조
- 헤드리스 애셋 제작 파이프라인
- 플레이 가능한 임시 에셋 구성

### 제외 범위

- 멀티플레이
- 인벤토리
- 세이브 및 로드
- 설정 메뉴
- 다중 무기 아키타입
- 드랍 및 루팅
- 영구 성장
- 보스전

## 단계 운영 규칙

- 실제 작업은 반드시 `CURRENT_PHASE_KO.md`에 정의된 현재 경계 안에서만 수행한다.
- 새 규칙, 예외, 설계 변경은 반드시 `DECISION_LOG_KO.md`에 기록한다.
- 현재 프로젝트의 코드, 설정, 애셋 상태가 달라지면 `STATE_SNAPSHOT_KO.md`를 갱신한다.
- 각 수행 단계의 시작 시각, 종료 시각, 소요 시간, 병렬 수행 여부는 `WORK_TIME_LOG_KO.md`에 append 한다.
- `generator/evaluator` 분리 운영을 썼다면 역할 분리 요약은 `CURRENT_PHASE_KO.md`에, 실제 수행 및 중계 방식은 `WORK_TIME_LOG_KO.md` 메모에 남겨야 한다.
- 단계 완료 후에는 `CURRENT_PHASE_KO.md`의 검증 결과와 다음 단계 제안을 갱신한다.

## 성공 조건

- 문서만 읽고도 다음 실행이 같은 방향으로 이어질 수 있어야 한다.
- 이전 대화를 모르는 상태에서도 현재 단계, 전역 제약, 구현 우선순위를 파악할 수 있어야 한다.
- 새 제약이 추가되더라도 문서 구조를 깨지 않고 `DECISION_LOG_KO.md`에 흡수할 수 있어야 한다.
