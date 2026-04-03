# PinkSurvivor Content 재구축 플레이북

최종 수정: 2026-04-04

## 목적

이 문서는 `PinkSurvivor/Content`를 지운 뒤 다시 만드는 절차를 단계별로 정리한다.

이 플레이북의 기본 전제는 다음과 같다.

- `Source/`와 `Config/`는 살아 있다.
- 가능하면 `Saved/Autosaves/`도 살아 있다.
- 목표는 먼저 `Gameplay-equivalent`, 그다음 `Asset-path-equivalent` 복원이다.

## 복원 경로 선택

### 경로 A | 정확도 우선

다음 자산이나 백업이 남아 있으면 최대한 활용한다.

- `Saved/Autosaves/Game/Blueprints/BP_Player_Auto1.uasset`
- `Saved/Autosaves/Game/Blueprints/BP_Enemy_Auto3.uasset`
- `Saved/Autosaves/Game/Blueprints/BP_Chest_Auto1.uasset`
- `Saved/Autosaves/Game/Blueprints/WBP_PlayerHUD_Auto1.uasset`
- `Saved/Autosaves/Game/UI/WBP_PlayerHUD_Auto2.uasset`
- `Saved/Autosaves/Game/Maps/BattleMap_Auto2.umap`
- 삭제 전에 따로 보관한 `Content/Character/Hoshino/*`
- 삭제 전에 따로 보관한 `Content/Props/Chest/*`
- 삭제 전에 따로 보관한 `Content/Meshes/Materials/*`

이 경로는 수작업 복원보다 정확하다.

### 경로 B | 문서 기반 재구축

백업 자산이 없으면 이 문서와 `CONTENT_ASSET_MANIFEST_KO.md`를 기준으로 새 자산을 만든다.

이 경로는 플레이 구조는 복원하지만, 외형은 일부 대체될 수 있다.

## 0. 삭제 전 체크

- 정확한 외형까지 복원해야 하면 `Content/Character/Hoshino`, `Content/Props/Chest`, `Content/Meshes/Materials`를 먼저 백업한다.
- `Saved/Autosaves`가 있으면 그대로 보존한다.
- `DefaultEngine.ini`, `DefaultInput.ini`, `Source/PinkSurvivor`는 건드리지 않는다.

## 1. 폴더 구조 재생성

아래 폴더를 다시 만든다.

- `/Game/Blueprints`
- `/Game/Character/Hoshino`
- `/Game/Character/Hoshino/Materials`
- `/Game/Inputs`
- `/Game/Maps`
- `/Game/Meshes/Materials`
- `/Game/Props/Chest`
- `/Game/UI`

## 2. 생존 백업 먼저 복구

`Saved/Autosaves`가 남아 있으면 수동 재작성 전에 먼저 복사한다.

- `BP_Player_Auto1.uasset` -> `/Game/Blueprints/BP_Player`
- `BP_Enemy_Auto3.uasset` -> `/Game/Blueprints/BP_Enemy`
- `BP_Chest_Auto1.uasset` -> `/Game/Blueprints/BP_Chest`
- `WBP_PlayerHUD_Auto*.uasset` -> `/Game/UI/WBP_PlayerHUD`
- `BattleMap_Auto2.umap` -> `/Game/Maps/BattleMap`

자동 저장본이 없는 자산은 다음 단계에서 수작업으로 만든다.

## 3. 아트 자산 복원

### 정확 복원 경로

백업이 있으면 아래 경로를 그대로 되살린다.

- `/Game/Character/Hoshino/Hoshino`
- `/Game/Character/Hoshino/SK_Hoshino`
- `/Game/Character/Hoshino/PA_Hoshino`
- `/Game/Character/Hoshino/Materials/M_PinkBody`
- `/Game/Props/Chest/Chest`
- `/Game/Props/Chest/M_Chest`
- `/Game/Props/Chest/chest_texture_albedo`
- `/Game/Props/Chest/chest_texture_mo`
- `/Game/Props/Chest/chest_texture_normal`
- `/Game/Meshes/Materials/M_Gold`
- `/Game/Meshes/Materials/M_GreenGem`
- `/Game/Meshes/Materials/M_PinkBall`
- `/Game/Meshes/Materials/M_YellowBall`
- `/Game/Meshes/Materials/M_YellowBody`

### 대체 복원 경로

백업이 없으면 아래처럼 최소 대체 자산을 만든다.

- `M_PinkBall`: 단색 분홍 머티리얼
- `M_GreenGem`: 단색 초록 머티리얼
- `M_Gold`: 단색 금색 머티리얼
- `M_YellowBody`: 단색 노랑 계열 머티리얼
- `M_PinkBody`: 단색 분홍 계열 머티리얼
- `Hoshino` 계열이 없으면 임시 humanoid skeletal mesh를 사용한다.
- `Chest` 메시가 없으면 임시 Static Mesh를 넣고 자산 이름만 `Chest`로 맞춘다.

이 경우 플레이는 복원되지만 외형은 다를 수 있다.

## 4. 입력 자산 재생성

### `/Game/Inputs/IA_Move`

- 타입: `Input Action`
- `ValueType`: `Axis2D`

### `/Game/Inputs/IA_Confirm`

- 타입: `Input Action`
- `ValueType`: `Boolean`

### `/Game/Inputs/IMC_Default`

- 타입: `Input Mapping Context`
- 권장 priority: `0`
- 매핑 수: `IA_Move` 3개, `IA_Confirm` 1개

복원 규칙:

- `IA_Move`는 `W/A/S/D` 조합으로 만든다.
- 가장 유력한 구성은 `D`, `A + Negate`, `W + SwizzleAxis`, `S + Negate + SwizzleAxis`다.
- `IA_Confirm`은 `SpaceBar`에 묶는다.
- 게임패드 키는 현재 자료에서 확정하지 못했으므로 비워 두거나 별도 확인 후 추가한다.

코드가 기대하는 조건:

- `BP_Player.DefaultInputMapping = IMC_Default`
- `BP_Player.MoveAction = IA_Move`
- `BP_Player.ConfirmAction = IA_Confirm`
- `ConfirmAction->bTriggerWhenPaused = true`
- `MoveAction`은 `Triggered`, `Completed`
- `ConfirmAction`은 `Triggered`

## 5. 게임플레이 블루프린트 재생성

자산별 상세 설정은 `CONTENT_ASSET_MANIFEST_KO.md`를 따른다.

필수 생성 순서:

1. `BP_Projectile`
2. `BP_Gem`
3. `BP_Coin`
4. `BP_Player`
5. `BP_Enemy`
6. `BP_Chest`
7. `WBP_PlayerHUD`
8. `WBP_Gameover`
9. `WBP_HUD`
10. `BP_SVGameMode`

핵심 연결:

- `BP_Player.AutoFireComponent.ProjectileClass = BP_Projectile`
- `BP_Player.DefaultInputMapping = IMC_Default`
- `BP_Player.MoveAction = IA_Move`
- `BP_Player.ConfirmAction = IA_Confirm`
- `BP_Player.Mesh = Hoshino`
- `BP_Enemy.ExperienceGemClasses[0] = BP_Gem`
- `BP_Enemy.GoldCoinClass = BP_Coin`
- `BP_Enemy.Mesh = Hoshino`
- `BP_Enemy` 본체 머티리얼은 `M_YellowBody`
- `BP_Chest.MeshComponent.StaticMesh = /Game/Props/Chest/Chest`
- `WBP_HUD.PlayerHUDWidgetClass = WBP_PlayerHUD`
- `WBP_HUD.GameOverWidgetClass = WBP_Gameover`
- `BP_SVGameMode.DefaultPawnClass = BP_Player`
- `BP_SVGameMode.DefaultEnemyClass = BP_Enemy`
- `BP_SVGameMode.HUDClass = WBP_HUD`

## 6. 위젯 재생성

### `/Game/UI/WBP_PlayerHUD`

- 부모 클래스: `UPSVPlayerHUDWidget`
- 최소 확인 위젯 이름:
  - `ProgressBar_HP`
  - `TextBlock_HP`
- 구현해야 하는 이벤트:
  - `OnHealthChanged`
  - `OnPlayerDied`
  - `OnExperienceChanged`
  - `OnLevelUp`
  - `OnPersistentGoldChanged`

문자열 단서상 포함해야 하는 표시 항목:

- 현재 체력
- 최대 체력
- 현재 경험치
- 현재 레벨
- 다음 레벨까지 필요 경험치
- 누적 골드

정확한 레이아웃을 보존하려면 `Saved/Autosaves`의 `WBP_PlayerHUD`를 우선 복구한다.

### `/Game/UI/WBP_Gameover`

- 부모 클래스: `UPSVGameOverWidget`
- 필수 위젯 이름:
  - `OkButton`
- 권장 표시:
  - `GameOver` 텍스트
  - `OK` 라벨

`OkButton` 이름이 맞아야 `BindWidget`가 동작한다.

### `/Game/UI/WBP_HUD`

- 부모 클래스: `APSVHUD` 기반 Blueprint
- UMG Widget Blueprint가 아니라 `HUD Blueprint`로 만든다.
- 클래스 디폴트에서 아래를 연결한다.
  - `PlayerHUDWidgetClass = WBP_PlayerHUD`
  - `GameOverWidgetClass = WBP_Gameover`

## 7. 맵 재생성

### 기본 맵

- 경로: `/Game/Maps/BattleMap`
- 프로젝트 기본 맵과 에디터 시작 맵 둘 다 이 맵으로 맞춘다.

### 월드 세팅

- 가장 유력한 복원값은 `GameMode Override = BP_SVGameMode`
- 근거는 `BattleMap` 문자열 안의 `DefaultGameMode`, `BP_SVGameMode`, `WorldSettings1`

### 액터 구성

맵 안에서 직접 확인되는 액터는 아래다.

- `DirectionalLight_0`
- `SkyAtmosphere_0`
- `SkyLight_0`
- `ExponentialHeightFog_0`
- `VolumetricCloud_0`
- `PlayerStart_0`
- `StaticMeshActor_0`
- `Floor_0`
- `BP_Chest_C_1`

복원 절차:

1. 기본 테스트 맵 수준의 환경 라이트 세트를 배치한다.
2. `StaticMeshActor_0`에는 `SM_SkySphere`를 넣는다.
3. `Floor_0`에는 `SM_Template_Map_Floor`를 넣고 `MI_ProcGrid`를 적용한다.
4. `PlayerStart_0`를 배치한다.
5. `BP_Chest`를 바닥 근처에 1개 배치한다.
6. 플레이어는 맵에 직접 두지 않고 `PlayerStart`에서 `BP_Player`가 스폰되게 둔다.

추가 단서:

- `LevelBoundsExtent = V(X=4000.00, Y=4000.00, Z=310.25)`
- `LevelBoundsLocation = V(X=-0.00, Y=0.00, Z=-90.25)`

정확한 좌표와 회전은 에디터가 없으면 완전 확정이 어렵다. 같은 플레이 감각을 우선한다.

## 8. 프로젝트 설정 확인

`Config/DefaultEngine.ini`

- `GameDefaultMap=/Game/Maps/BattleMap.BattleMap`
- `EditorStartupMap=/Game/Maps/BattleMap.BattleMap`
- `GameInstanceClass=/Script/PinkSurvivor.PSVGameInstance`

`Config/DefaultInput.ini`

- `DefaultPlayerInputClass=/Script/EnhancedInput.EnhancedPlayerInput`
- `DefaultInputComponentClass=/Script/EnhancedInput.EnhancedInputComponent`

## 9. 제외 대상

- `BP_EnemySpawner`는 현재 clean rebuild 필수 대상이 아니다.
- 저장소 안에 `PSVEnemySpawner` C++ 클래스가 없고, 맵 배치 흔적도 없다.
- 완전한 폴더 재현이 목적이더라도 우선순위는 맨 마지막이다.

## 10. 스모크 테스트

아래 조건을 모두 통과하면 `Gameplay-equivalent` 복원으로 본다.

1. `BattleMap` 시작 시 플레이어가 `PlayerStart`에서 스폰된다.
2. 플레이어가 이동 가능하다.
3. 입력 없이 자동 발사가 시작된다.
4. 적이 플레이어 주변에 시간 기반으로 스폰된다.
5. 적이 플레이어를 추적하고 근접 공격한다.
6. 적 사망 시 젬은 항상, 코인은 확률적으로 드롭된다.
7. 체력/경험치/레벨/골드가 HUD에 반영된다.
8. 플레이어 사망 시 `WBP_Gameover`가 뜨고 `OkButton`으로 해제된다.
9. 상자와 겹치면 게임이 멈추고 룰렛 텍스트가 돈다.
10. `Confirm` 입력으로 룰렛이 종료되고 게임이 재개된다.

## 11. 남는 한계

- `Hoshino`, `Chest` 원본 아트가 없으면 외형은 완전 동일하게 못 만든다.
- `WBP_PlayerHUD`의 정확한 위치/간격/스타일은 현 자료만으로 100% 확정하지 못했다.
- `BattleMap`의 정확한 액터 transform도 완전 확정하지 못했다.

즉, 이 플레이북은 `삭제 후에도 다시 플레이 가능한 상태`를 목표로 하고, 완전한 바이너리 복원은 백업 자산이 있을 때만 가능하다.
