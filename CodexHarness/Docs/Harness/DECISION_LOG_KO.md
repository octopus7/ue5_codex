# CodexHarness 결정 로그

최종 수정일: 2026-04-04

## 사용 규칙

- 전역 제약, 구현 정책, 예외 처리 방식이 바뀌면 이 문서에 먼저 기록한다.
- 임시 규칙이라도 다음 단계에 영향을 주면 기록한다.
- 각 항목은 날짜와 영향을 받는 영역을 함께 적는다.

## 결정 사항

### 2026-04-04 | 브랜치 정합성 복구 | `Harness_T1` 검증 스냅샷을 `Harness_T2`에 선택 통합

- `Harness_T2` 작업 트리는 문서보다 실제 구현이 크게 뒤처져 있었고, `Harness_T1`에는 같은 프로젝트의 검증된 구현 스냅샷이 존재했다.
- 현재 브랜치에는 `CodexHarness` 경로의 코드, 설정, 콘텐츠, 플러그인 소스만 선택 반영하고, 문서는 현재 브랜치 검증 결과 기준으로 다시 정렬한다.
- 이후 상태 판정은 과거 브랜치 가정이 아니라 현재 작업 트리에서 다시 수행한 빌드와 헤드리스 검증 결과를 기준으로 한다.

### 2026-04-04 | 문서 정합성 | 문서 기준 식별자를 실제 프로젝트 `CodexHarness`로 고정

- 초기 하네스 문서의 범용 플레이스홀더 운영 방식은 템플릿 단계에는 유효했지만, 현재는 실제 프로젝트 경로와 애셋 경로를 적어야 검증성이 유지된다.
- 이후 문서는 프로젝트명 `CodexHarness`, 런타임 모듈 `CodexHarness`, 에디터 모듈 `CodexHarnessEditor`, 커맨드렛 `CodexHarnessHeadlessSetup`를 기준으로 유지한다.

### 2026-04-04 | 작업 방식 | 장기 작업은 문서 세트로 운영

- 상위 목표, 현재 단계, 결정 이력, 상태 스냅샷, 작업 시간 로그를 분리한 운영 문서 세트를 기본 방식으로 사용한다.

### 2026-04-04 | 실행 환경 | Unreal Editor GUI는 열지 않음

- 작업은 에디터가 꺼진 상태를 기본 전제로 한다.
- 에디터 API가 필요한 작업은 `UnrealEditor-Cmd`와 커맨드렛으로 처리한다.
- 수동 에디터 조작을 전제로 한 워크플로는 기본 경로로 사용하지 않는다.

### 2026-04-04 | 입력 시스템 | 레거시 입력 대신 `EnhancedInput` 사용

- 입력 시스템은 `EnhancedInput`으로 통일한다.
- 새 입력 작업은 레거시 축/액션 매핑 확장 대신 `Input Action`과 `Input Mapping Context` 기준으로 설계한다.

### 2026-04-04 | 입력 자산 구조 | IA와 IMC는 실애셋으로 만들고 플레이어는 DA만 참조

- `IA_*`와 `IMC_*`는 실제 프로젝트 애셋으로 생성 또는 갱신한다.
- 입력 설정 집계용 `DA_DefaultInputConfig`를 두고, 필요한 `IA_*`와 `IMC_*` 참조를 이 DA에 모은다.
- 플레이어 런타임 클래스는 `IA_*`나 `IMC_*`를 직접 참조하지 않고 `DA_DefaultInputConfig`만 참조한다.

### 2026-04-04 | 전역 피드백 자산 구조 | `DA_DefaultEffectsConfig`를 두고 `GameInstance`에서 전역 접근

- 플레이어 피격 연출에 필요한 `Niagara System`과 카메라 흔들림 클래스는 `DA_DefaultEffectsConfig`에 집계한다.
- `BP_CodexHarnessGameInstance`는 이 DA를 기본값으로 참조하고 런타임 접근 지점을 제공한다.
- 개별 플레이어 클래스가 피격 연출 애셋을 직접 하드코딩하는 방식은 기본 경로로 사용하지 않는다.

### 2026-04-04 | 플레이어 피격 피드백 | 반동, `Niagara`, 카메라 흔들림을 함께 사용

- 플레이어가 데미지를 받으면 반동이 적용돼야 한다.
- 같은 피격 이벤트에서 `NS_PlayerHitReaction`을 재생해야 한다.
- 같은 피격 이벤트에서 `BP_CodexHarnessPlayerHitCameraShake`도 재생해야 한다.

### 2026-04-04 | 구현 분리 | 핵심 구조는 C++, 조정과 표현은 Blueprint/UMG

- 핵심 게임플레이 구조는 C++에 둔다.
- Blueprint와 UMG는 튜닝, 레이아웃, 시각 구성, 에셋 연결에 사용한다.

### 2026-04-04 | 메시 제작 | 새 메시가 필요하면 VOX 소스부터 생성

- 새 메시가 필요할 경우 임의의 수작업 메시 대신 `.vox` 소스를 먼저 생성한다.
- 기본 해상도는 `64 x 64 x 64` 이내로 둔다.
- `.vox` 소스는 `SourceArt/Vox/` 아래에 보관하는 것을 기본 규칙으로 한다.
- 생성된 Unreal 애셋은 `/Game/CodexHarness/Vox/` 아래에 저장하는 것을 기본 규칙으로 한다.

### 2026-04-04 | 메시 머터리얼 | VOX 메시용 베이스 머터리얼은 1개만 재사용

- VOX 메시용 베이스 머터리얼은 프로젝트 전역에서 하나만 유지한다.
- 기본 이름은 `M_VoxBase`로 한다.
- 베이스 컬러는 버텍스 컬러를 사용해서 칠할 수 있어야 한다.

### 2026-04-04 | 구현 우선순위 | 제작 파이프라인을 먼저 고정하고 게임플레이를 올림

- 헤드리스 커맨드렛과 애셋 생성 루트를 먼저 고정하는 것이 이후 단계 전체를 안정화하는 데 유리하다고 판단했다.
- 순수 C++ 게임플레이 구현은 그 위에 단계적으로 올린다.

### 2026-04-04 | 수행 방식 | 시간이 줄어들면 병렬 수행을 허용

- 탐색, 구현, 검증을 순차로만 고정하지 않는다.
- 같은 단계 경계를 유지할 수 있고 총 작업 시간이 줄어들면 병렬 수행을 허용한다.

### 2026-04-04 | 에이전트 협업 | `generator`와 `evaluator`를 분리 컨텍스트로 운용 가능

- 구현 생산이 중심인 `generator` 에이전트와 평가 중심인 `evaluator` 에이전트를 분리된 컨텍스트로 둘 수 있다.
- `generator`는 초안 구현, 코드 작성, 수정안 생산을 맡고 `evaluator`는 리뷰, 리스크 점검, 검증 포인트 평가를 맡는다.
- 두 에이전트는 직접 통신하지 않고 메인 에이전트가 결과와 피드백을 중계한다.

### 2026-04-04 | 작업 기록 | 단계별 시간 기록은 append 전용 로그로 남김

- 각 수행 단계 또는 독립 작업 단위의 시작 시각, 종료 시각, 소요 시간, 병렬 수행 여부를 기록한다.
- 시간 기록은 `Docs/Harness/WORK_TIME_LOG_KO.md`에 append 형식으로 누적한다.

### 2026-04-04 | 클래스 연결 | 직접 C++ 연결을 금지하고 구체적인 Blueprint 애셋을 기본 연결 대상으로 사용

- 핵심 로직은 C++ 베이스 클래스에 둔다.
- 실제 프로젝트 설정, 맵, 레퍼런스 연결은 구체적인 Blueprint 파생 클래스 애셋을 기준으로 한다.
- `GameMode`, `DefaultPawnClass`, `PlayerControllerClass`, `HUDClass` 등 기본 연결 지점에는 직접 C++ 클래스를 연결하지 않는다.
- 기본 연결에 필요한 Blueprint 애셋은 커맨드렛으로 생성 또는 갱신 가능한 상태를 유지한다.

### 2026-04-04 | 가시 런타임 표현 | 화면에 보여야 하는 오브젝트는 실제 메시 애셋으로 렌더링

- 플레이어, 적, 무기 등 화면에 보여야 하는 핵심 런타임 오브젝트는 실제 렌더링 컴포넌트와 메시 애셋이 연결된 상태여야 한다.
- 빈 액터, 투명 placeholder, 메시가 없는 이동체를 완료 상태로 간주하지 않는다.
- VOX 기반 외형을 사용할 경우 `.vox -> StaticMesh` import 결과가 실제 Blueprint 또는 컴포넌트에 연결되어야 한다.

### 2026-04-04 | 헤드리스 자동화 | 초기 자동화 진입점은 `CodexHarnessHeadlessSetup` 커맨드렛

- 첫 자동화 진입점은 `CodexHarnessHeadlessSetup` 커맨드렛으로 고정한다.
- 이 커맨드렛은 `SourceArt/Vox/`, `Saved/HeadlessSetup/`, `/Game/CodexHarness/Materials/M_VoxBase`를 보장한다.
- 초기 검증 기준은 에디터 타겟 빌드 성공과 커맨드렛 재실행 성공으로 둔다.

### 2026-04-04 | VOX import 경로 | 가능하면 저장소 내 `VoxImporter` 플러그인을 재사용

- `.vox -> StaticMesh` 경로를 새로 발명하기보다 저장소 내 검증된 `VoxImporter` 플러그인을 우선 재사용한다.
- 플러그인 편입이 가능하면 커맨드렛에서 자동 import task로 호출한다.

### 2026-04-04 | 빌드 실행 | Live Coding mutex가 있으면 `-NoHotReloadFromIDE`를 사용

- 동일 엔진 버전의 다른 Unreal Editor 인스턴스가 실행 중이면 일반 UBT 빌드가 Live Coding mutex에 막힐 수 있다.
- 이 경우 에디터 프로세스를 강제 종료하지 않고 `Build.bat ... -NoHotReloadFromIDE`와 `UnrealEditor-Cmd.exe ... -NoHotReloadFromIDE`를 기본 실행 인자로 사용한다.

### 2026-04-04 | 플레이어 가시 애셋 연결 | BP 기본값은 `DefaultVisualMesh`, 실제 컴포넌트 반영은 네이티브 `VisualMeshComponent`

- 헤드리스 경로에서 Character BP 내부 컴포넌트 템플릿을 직접 편집하는 안정적인 로컬 레퍼런스는 확보하지 못했다.
- 대신 `ACodexHarnessCharacter`와 `ACodexHarnessEnemyCharacter`가 네이티브 `VisualMeshComponent`를 소유하고, BP 기본값으로는 `DefaultVisualMesh`를 저장한 뒤 `RefreshVisualMeshDefaults()`로 실제 컴포넌트에 반영하는 구조를 채택했다.

### 2026-04-04 | 프로젝트 연결 범위 | `GameMapsSettings`와 `BasicMap` `WorldSettings`를 함께 고정

- 프로젝트 기본 연결만 맞추고 맵 `WorldSettings`를 비워두면 이후 `BasicMap` 편집 상태에 따라 기본 게임 모드 해석이 흔들릴 수 있다.
- 그래서 `GameInstanceClass`, `GlobalDefaultGameMode`, `GameDefaultMap`, `EditorStartupMap`은 `GameMapsSettings`에 저장하고, `BasicMap`의 `WorldSettings.DefaultGameMode`도 같은 `BP_CodexHarnessGameMode_C`로 함께 맞춘다.

### 2026-04-04 | 이동 입력 책임 분리 | `PlayerController`가 바인딩하고 `Character`는 이동 계산만 담당

- 입력 자산 참조는 계속 `DA_DefaultInputConfig` 한 곳에만 두기 위해 `IA_Move` 바인딩 책임을 `ACodexHarnessPlayerController`에 둔다.
- `ACodexHarnessCharacter`는 입력 애셋을 직접 알지 않고 `MoveInTopDownPlane`으로 이동 벡터 계산과 `AddMovementInput`만 담당한다.

### 2026-04-04 | 커서 조준 경로 | 상면 조준은 `PlayerController`가 커서 월드 평면 투영, `Character`가 최종 회전 수행

- 마우스 커서 기반 조준은 화면 좌표와 월드 변환이 필요하므로 `PlayerController`가 `DeprojectMousePositionToWorld`와 평면 교차 계산을 담당한다.
- `Character`는 입력 장치를 직접 알지 않고 `AimAtWorldLocation`과 최종 회전만 수행한다.

### 2026-04-04 | 기본 발사 경로 | `PlayerController`가 발사 입력과 조준점 해석, `Character`가 히트스캔과 `ApplyDamage` 수행

- 발사 입력도 이동과 조준과 같은 패턴으로 `ACodexHarnessPlayerController`가 `DA_DefaultInputConfig`를 통해 바인딩한다.
- 실제 공격 판정은 `ACodexHarnessCharacter::FireAtWorldLocation`에 두고, 현재 조준 지점 기준 `PlayerWeaponTrace` 라인트레이스로 최소 전투 루프를 만든다.

### 2026-04-04 | 체력 상태 소유 | 생명주기는 `UCodexHarnessHealthComponent`로 분리

- 플레이어와 적이 같은 피격/사망 경로를 재사용할 수 있도록 체력 상태는 캐릭터 내부 변수 대신 공용 `ActorComponent`로 분리한다.
- `ApplyDamage`의 실제 체력 감소와 사망 판정은 `UCodexHarnessHealthComponent`가 소유하고, 캐릭터는 사망 후 제어 차단과 게임모드 통지만 맡는다.

### 2026-04-04 | 적 스폰 경로 | 적 배치는 맵 고정 배치 대신 `GameMode` 런타임 스폰으로 유지

- 헤드리스 커맨드렛이 `BasicMap`을 직접 열고 저장하는 구조라서, 적 액터를 맵에 영구 배치하면 재실행 때 중복 배치 리스크가 커진다.
- 그래서 적 1종 스폰은 `ACodexHarnessGameMode`가 런타임에 처리하고, 커맨드렛은 적 BP 래퍼와 기본 클래스 연결만 보장한다.

### 2026-04-04 | 적 이동 방식 | `AIController`와 내비게이션 대신 적 캐릭터가 직접 플레이어를 추적

- 현재 범위와 모듈 의존성에서는 `AIController + Navigation`보다 적 캐릭터가 직접 플레이어 위치를 향해 이동 입력을 넣는 편이 더 작고 안전하다.
- 공격도 별도 행동트리 없이 거리와 쿨다운 기준으로 `ApplyDamage`를 호출하는 최소 구조로 유지한다.

### 2026-04-04 | 웨이브 상태 소유 | 웨이브 번호와 남은 적 수는 `GameMode`가 단일 소유

- `CurrentWave`, `RemainingEnemyCount`, `PendingEnemySpawnCount`, `AliveEnemyCount`는 `ACodexHarnessGameMode`가 단일 소유한다.
- 적 사망과 다음 웨이브 시작 타이머도 `GameMode` 한 곳에서만 관리한다.

### 2026-04-04 | HUD 표시 경로 | UMG 대신 `Canvas HUD`를 우선 사용

- 현재 목표 범위에서는 위젯 자산과 런타임 모듈 의존성을 늘리는 것보다 `ACodexHarnessHUD::DrawHUD()`로 직접 그리는 편이 더 작고 안전하다.
- 체력, 웨이브, 남은 적 수, 게임오버 메시지는 모두 같은 HUD 경로에서 그린다.

### 2026-04-04 | 재시작 경로 | 부분 초기화 대신 현재 맵 `OpenLevel` 재오픈

- 게임오버 후 재시작은 개별 상태를 수동 초기화하기보다 현재 맵을 다시 여는 편이 웨이브, 적, 체력, 타이머 상태를 한 번에 안전하게 초기화한다.
- 재시작 입력도 기존 `EnhancedInput` 경로를 유지하기 위해 `IA_Restart`와 `DA_DefaultInputConfig`를 통해 연결한다.

### 2026-04-04 | 공격 판정 채널 | 적 명중용 전용 채널은 `PlayerWeaponTrace`(`ECC_GameTraceChannel1`)로 고정

- 플레이어 공격 판정은 월드 공용 가시성 채널 대신 전용 게임 트레이스 채널을 사용한다.
- `Config/DefaultEngine.ini`에 `PlayerWeaponTrace` 이름을 `ECC_GameTraceChannel1`로 등록하고, 적 캐릭터 캡슐은 이 채널에 `Block` 응답을 갖도록 한다.
- 이렇게 해서 향후 월드 클릭/가시성 검사와 공격 판정을 분리한다.

### 2026-04-04 | 카메라 흔들림 애셋 경로 | 피격 카메라 흔들림은 C++ 베이스와 BP 래퍼를 함께 사용

- 실제 프로젝트 애셋 요구를 만족하기 위해 카메라 흔들림은 `UCodexHarnessPlayerHitCameraShake` C++ 베이스 위에 `BP_CodexHarnessPlayerHitCameraShake` BP 래퍼를 생성해 사용한다.
- `DA_DefaultEffectsConfig`는 이 BP의 생성 클래스를 참조하고, 플레이어 피격 시 `PlayerController::ClientStartCameraShake`로 재생한다.

### 2026-04-04 | 캐릭터 외형 식별 | 플레이어와 적은 서로 다른 닭 VOX 메시를 사용

- 플레이어와 적이 같은 테스트 메시를 공유하면 전투 중 식별성이 떨어지므로 외형을 분리한다.
- 플레이어는 `SourceArt/Vox/SM_Vox_PlayerChicken_White.vox`에서 생성한 흰 닭 메시 `/Game/CodexHarness/Vox/SM_Vox_PlayerChicken_White`를 사용한다.
- 적은 `SourceArt/Vox/SM_Vox_EnemyChicken_Red.vox`에서 생성한 붉은 닭 메시 `/Game/CodexHarness/Vox/SM_Vox_EnemyChicken_Red`를 사용한다.
- 두 메시의 생성, import, `DefaultVisualMesh` 연결은 모두 `CodexHarnessHeadlessSetup` 커맨드렛이 담당한다.
