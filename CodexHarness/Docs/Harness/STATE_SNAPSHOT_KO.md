# CodexHarness 상태 스냅샷

기준 시각: 2026-04-04

## 프로젝트 메타데이터

- 프로젝트 경로: `현재 작업 디렉터리 기준 프로젝트 루트`
- 엔진 버전: `5.7`
- 프로젝트 파일: `CodexHarness.uproject`
- 런타임 모듈: `CodexHarness`
- 에디터 자동화 모듈: `CodexHarnessEditor`
- 활성 플러그인: `ModelingToolsEditorMode`, `VoxImporter`, `Niagara`

## 코드 상태

- `Source/CodexHarness/`에 아래 런타임 베이스 클래스가 존재한다.
  - `UCodexHarnessGameInstance`
  - `UCodexHarnessInputConfigDataAsset`
  - `UCodexHarnessEffectsConfigDataAsset`
  - `UCodexHarnessPlayerHitCameraShake`
  - `UCodexHarnessPlayerHitCameraShakePattern`
  - `ACodexHarnessGameMode`
  - `UCodexHarnessHealthComponent`
  - `ACodexHarnessPlayerController`
  - `ACodexHarnessCharacter`
  - `ACodexHarnessEnemyCharacter`
  - `ACodexHarnessHUD`
- `ACodexHarnessCharacter`는 `CameraBoom`, `FollowCamera`, `VisualMeshComponent`를 가진다.
- `BP_CodexHarnessCharacter`의 기본 가시 메시 경로는 흰 닭 VOX `SM_Vox_PlayerChicken_White -> DefaultVisualMesh -> VisualMeshComponent`다.
- `ACodexHarnessCharacter`는 `UCodexHarnessHealthComponent`를 소유하고, 사망 시 이동을 중지하며 `GameMode`에 통지한다.
- `ACodexHarnessCharacter`는 `MoveInTopDownPlane`으로 카메라 기준 평면 이동 벡터를 계산한다.
- `ACodexHarnessCharacter`는 `AimAtWorldLocation`으로 커서 기준 상면 회전을 수행한다.
- `ACodexHarnessCharacter`는 `FireAtWorldLocation`으로 `PlayerWeaponTrace`(`ECC_GameTraceChannel1`) 기준 히트스캔 라인트레이스를 수행하고, 체력 컴포넌트가 있는 대상에만 `ApplyDamage`를 전달한다.
- `ACodexHarnessCharacter`는 피격 시 반동, `NS_PlayerHitReaction`, `BP_CodexHarnessPlayerHitCameraShake`를 재생한다.
- `ACodexHarnessEnemyCharacter`는 `UCodexHarnessHealthComponent`와 `VisualMeshComponent`를 소유하고, 플레이어를 직접 추적해 일정 주기마다 공격한다.
- `BP_CodexHarnessEnemyCharacter`의 기본 가시 메시 경로는 붉은 닭 VOX `SM_Vox_EnemyChicken_Red -> DefaultVisualMesh -> VisualMeshComponent`다.
- `ACodexHarnessEnemyCharacter`의 캡슐은 `PlayerWeaponTrace` 채널에 `Block` 응답을 갖는다.
- `ACodexHarnessPlayerController`는 `DA_DefaultInputConfig`를 경유해 입력 매핑 컨텍스트를 적용하고 `IA_Move`, `IA_Fire`, `IA_Restart`를 실제 바인딩한다.
- `ACodexHarnessPlayerController`는 매 틱 `UpdateAimFromCursor()`로 커서를 월드 평면에 투영하고 같은 결과를 발사 입력에 재사용한다.
- `ACodexHarnessGameMode`는 `EnemyCharacterClass`, `CurrentWave`, `RemainingEnemyCount`, `PendingEnemySpawnCount`, `AliveEnemyCount`, `bIsGameOver`를 소유하고, 웨이브별 적 스폰을 타이머로 제어한다.
- `ACodexHarnessGameMode`는 `RequestRestart()`로 현재 맵을 다시 열어 전체 상태를 초기화한다.
- `ACodexHarnessHUD`는 `DrawHUD()`에서 플레이어 체력, 현재 웨이브, 남은 적 수를 `Canvas HUD`로 직접 그리고, 게임오버 상태에서는 재시작 안내를 오버레이로 표시한다.
- `Source/CodexHarnessEditor/Private/Commandlets/CodexHarnessHeadlessSetupCommandlet.cpp`는 아래를 한 번에 처리한다.
  - 플레이어/적 닭 `.vox` 보장
  - `M_VoxBase` 보장
  - `.vox -> StaticMesh` import
  - `BP_*` 래퍼 생성/갱신
  - `IA_*`, `IMC_*`, `DA_DefaultInputConfig` 생성/갱신
  - `DA_DefaultEffectsConfig`, `NS_PlayerHitReaction`, `BP_CodexHarnessPlayerHitCameraShake` 생성/갱신
  - `GameMapsSettings`와 `BasicMap` 기본 연결 적용
  - 자동화 보고서 작성

## 빌드 의존성 상태

- 런타임 모듈 의존성
  - `Core`
  - `CoreUObject`
  - `Engine`
  - `EnhancedInput`
  - `InputCore`
  - `Niagara`
- 에디터 모듈 의존성
  - `AssetRegistry`
  - `AssetTools`
  - `EngineSettings`
  - `EnhancedInput`
  - `InputEditor`
  - `Kismet`
  - `Niagara`
  - `NiagaraEditor`
  - `UnrealEd`
  - `VoxImporterEditor`

## 설정 상태

- `Config/DefaultEngine.ini`
  - `GameInstanceClass=/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessGameInstance.BP_CodexHarnessGameInstance_C`
  - `GameDefaultMap=/Game/Maps/BasicMap`
  - `EditorStartupMap=/Game/Maps/BasicMap.BasicMap`
  - `GlobalDefaultGameMode=/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessGameMode.BP_CodexHarnessGameMode_C`
  - `PlayerWeaponTrace`가 `ECC_GameTraceChannel1`로 등록됨
- `Config/DefaultInput.ini`
  - `DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput`
  - `DefaultInputComponentClass=/Script/EnhancedInput.EnhancedInputComponent`
- `Content/Maps/BasicMap.umap`
  - `WorldSettings.DefaultGameMode = BP_CodexHarnessGameMode_C`

## 콘텐츠 상태

- VOX/머터리얼
  - `/Game/CodexHarness/Materials/M_VoxBase`
  - `/Game/CodexHarness/Vox/SM_Vox_PlayerChicken_White`
  - `/Game/CodexHarness/Vox/SM_Vox_EnemyChicken_Red`
  - `SourceArt/Vox/SM_Vox_PlayerChicken_White.vox`
  - `SourceArt/Vox/SM_Vox_EnemyChicken_Red.vox`
- Blueprint 래퍼
  - `/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessGameInstance`
  - `/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessGameMode`
  - `/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessPlayerController`
  - `/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessCharacter`
  - `/Game/CodexHarness/Blueprints/Enemies/BP_CodexHarnessEnemyCharacter`
  - `/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessHUD`
- 입력 애셋
  - `/Game/CodexHarness/Input/Actions/IA_Move`
  - `/Game/CodexHarness/Input/Actions/IA_Look`
  - `/Game/CodexHarness/Input/Actions/IA_Fire`
  - `/Game/CodexHarness/Input/Actions/IA_Restart`
  - `/Game/CodexHarness/Input/Contexts/IMC_Default`
  - `/Game/CodexHarness/Input/Configs/DA_DefaultInputConfig`
- 피드백 애셋
  - `/Game/CodexHarness/Effects/NS_PlayerHitReaction`
  - `/Game/CodexHarness/Effects/BP_CodexHarnessPlayerHitCameraShake`
  - `/Game/CodexHarness/Effects/DA_DefaultEffectsConfig`
- 자동화 보고서
  - `Saved/HeadlessSetup/CodexHarnessHeadlessSetupReport.txt`

## 검증 상태

- `Build.bat CodexHarnessEditor Win64 Development -Project='D:\github\ue5_codex\CodexHarness\CodexHarness.uproject' -WaitMutex -FromMsBuild -NoHotReloadFromIDE` 성공
- `UnrealEditor-Cmd.exe 'D:\github\ue5_codex\CodexHarness\CodexHarness.uproject' -run=CodexHarnessHeadlessSetup -unattended -nop4 -nosplash -nullrhi -NoHotReloadFromIDE` 성공
- 자동화 보고서에 `SM_Vox_PlayerChicken_White`, `SM_Vox_EnemyChicken_Red`, `EnemyCharacter`, `RestartAction`, `PlayerHitCameraShake`, `EffectsConfig` 반영 확인

## 문서 운영 상태

- 하네스 문서는 실제 `CodexHarness` 저장소 기준으로 정렬돼 있다.
- `CURRENT_PHASE_KO.md`는 현재 `M6-P2` 완료 상태를 가리킨다.
- `WORK_TIME_LOG_KO.md`는 단계별 append 로그로 누적 중이다.

## 현재 공백

- 최초 완성 기준 대비 기능 공백은 없다. 후속 작업은 개선 범위로만 남아 있다.

## 현재 리스크

- 수동 플레이 테스트를 수행하지 않았으므로 실제 입력 감각, 파형 강도, HUD 배치 체감 검증이 남아 있다.
- 적 추적은 내비게이션이 아닌 직접 추적 방식이어서 복잡한 장애물 지형에서는 행동 품질 리스크가 있다.
- 플레이어와 적의 가시 메시 연결은 `DefaultVisualMesh -> VisualMeshComponent` 반영 패턴에 의존하므로 이후 BP 편집 시 이 경로를 깨지 않도록 유지해야 한다.

## 바로 다음 추천 작업

- 최초 완성 기준은 충족됐다.
- 이후 개선 작업이 필요하면 별도 범위와 새 단계 문서를 연다.
