# PinkSurvivor Content 자산 매니페스트

최종 수정: 2026-04-04

## 사용 규칙

- 이 문서는 `Content` 자산의 경로, 부모 클래스, 연결 대상, 복원 규칙을 자산별로 고정한다.
- `확정`은 코드 또는 자산 문자열로 직접 확인한 항목이다.
- `강한 추정`은 자산 문자열과 코드 흐름을 종합한 항목이다.
- `대체 허용`은 동일 플레이 구조만 유지되면 임시 자산으로 바꿔도 되는 항목이다.

## 필수 폴더

- `/Game/Blueprints`
- `/Game/Character/Hoshino`
- `/Game/Character/Hoshino/Materials`
- `/Game/Inputs`
- `/Game/Maps`
- `/Game/Meshes/Materials`
- `/Game/Props/Chest`
- `/Game/UI`

## 게임플레이 핵심 자산

| 경로 | 종류 | 부모/베이스 | 필수 연결 | 복원 규칙 | 상태 |
| --- | --- | --- | --- | --- | --- |
| `/Game/Blueprints/BP_SVGameMode` | BP | `APSVGameMode` | `BP_Player`, `BP_Enemy`, `WBP_HUD` | `DefaultPawnClass = BP_Player`, `DefaultEnemyClass = BP_Enemy`, `HUDClass = WBP_HUD` | 확정 |
| `/Game/Blueprints/BP_Player` | BP | `APSVPlayerCharacter` | `IMC_Default`, `IA_Move`, `IA_Confirm`, `BP_Projectile`, `Hoshino` | 입력 3종과 발사체 클래스를 연결하고, 메시를 `Hoshino`로 맞춘다 | 확정 |
| `/Game/Blueprints/BP_Enemy` | BP | `APSVEnemyCharacter` | `BP_Gem`, `BP_Coin`, `Hoshino`, `M_YellowBody` | `ExperienceGemClasses[0] = BP_Gem`, `GoldCoinClass = BP_Coin`, 메시는 `Hoshino`, 본체는 노랑 머티리얼 | 확정 |
| `/Game/Blueprints/BP_Projectile` | BP | `APSVProjectile` | `Sphere`, `M_PinkBall` | `ProjectileMesh`에 엔진 `Sphere`, 머티리얼 `M_PinkBall` 적용 | 확정 |
| `/Game/Blueprints/BP_Gem` | BP | `APSVExperienceGem` | `Cube`, `M_GreenGem` | `MeshComponent`에 엔진 `Cube`, 머티리얼 `M_GreenGem` 적용 | 확정 |
| `/Game/Blueprints/BP_Coin` | BP | `APSVGoldCoin` | `Cylinder`, `M_Gold` | `MeshComponent`에 엔진 `Cylinder`, 머티리얼 `M_Gold` 적용 | 확정 |
| `/Game/Blueprints/BP_Chest` | BP | `APSVRewardChest` | `/Game/Props/Chest/Chest` | `MeshComponent`에 상자 메시를 연결한다 | 확정 |

## 입력 자산

| 경로 | 종류 | 핵심 값 | 복원 규칙 | 상태 |
| --- | --- | --- | --- | --- |
| `/Game/Inputs/IA_Move` | Input Action | `ValueType = Axis2D` | 플레이어 이동용 2D 벡터 액션 | 확정 |
| `/Game/Inputs/IA_Confirm` | Input Action | `ValueType = Boolean` | 상자 룰렛 확인용 불리언 액션 | 확정 |
| `/Game/Inputs/IMC_Default` | Input Mapping Context | `priority = 0` | `IA_Move` 3개, `IA_Confirm` 1개 매핑. 가장 유력한 키는 `W/A/S/D`, `SpaceBar` | 강한 추정 |

## HUD 및 UI 자산

| 경로 | 종류 | 부모/베이스 | 필수 연결 | 복원 규칙 | 상태 |
| --- | --- | --- | --- | --- | --- |
| `/Game/UI/WBP_HUD` | HUD BP | `APSVHUD` | `WBP_PlayerHUD`, `WBP_Gameover` | 클래스 디폴트에서 `PlayerHUDWidgetClass`, `GameOverWidgetClass`를 각각 연결한다 | 확정 |
| `/Game/UI/WBP_PlayerHUD` | Widget BP | `UPSVPlayerHUDWidget` | `ProgressBar_HP`, `TextBlock_HP` | `OnHealthChanged`, `OnPlayerDied`, `OnExperienceChanged`, `OnLevelUp`, `OnPersistentGoldChanged`를 구현한다 | 확정 |
| `/Game/UI/WBP_Gameover` | Widget BP | `UPSVGameOverWidget` | `OkButton` | `OkButton` 이름을 정확히 맞추고, `GameOver`와 `OK` 텍스트를 배치한다 | 확정 |

### `WBP_PlayerHUD` 내부 단서

문자열 기준으로 확인되는 이름과 표시 항목:

- 위젯 이름:
  - `ProgressBar_HP`
  - `TextBlock_HP`
- 표시 항목 단서:
  - `Current Health`
  - `Max Health`
  - `Current Experience`
  - `Current Level`
  - `Experience to Next Level`
  - `Total Gold`

정확한 배치와 스타일은 `Saved/Autosaves` 복원이 가장 안전하다.

### `WBP_Gameover` 내부 단서

- `OkButton`
- `OK`
- `GameOver`
- `CanvasPanel`
- `Button`
- `TextBlock`

## 맵 자산

| 경로 | 종류 | 핵심 연결 | 복원 규칙 | 상태 |
| --- | --- | --- | --- | --- |
| `/Game/Maps/BattleMap` | Map | `BP_SVGameMode`, `BP_Chest`, `PlayerStart`, 기본 환경 액터 | 테스트용 단순 전장 맵. 월드 세팅에서 `GameMode Override = BP_SVGameMode`가 가장 유력 | 강한 추정 |

### `BattleMap` 안에서 확인된 액터

- `DirectionalLight_0`
- `SkyAtmosphere_0`
- `SkyLight_0`
- `ExponentialHeightFog_0`
- `VolumetricCloud_0`
- `PlayerStart_0`
- `StaticMeshActor_0`
- `Floor_0`
- `BP_Chest_C_1`
- `WorldSettings1`

### `BattleMap` 지오메트리 단서

- `StaticMeshActor_0 -> SM_SkySphere`
- `Floor_0 -> SM_Template_Map_Floor`
- 관련 머티리얼 단서:
  - `MI_ProcGrid`
  - `M_SimpleSkyDome`

### `BattleMap` 경계 단서

- `LevelBoundsExtent = V(X=4000.00, Y=4000.00, Z=310.25)`
- `LevelBoundsLocation = V(X=-0.00, Y=0.00, Z=-90.25)`

## 아트 자산

| 경로 | 종류 | 용도 | 복원 난이도 | 상태 |
| --- | --- | --- | --- | --- |
| `/Game/Character/Hoshino/Hoshino` | Skeletal Mesh 관련 자산 | 플레이어/적 외형 | 원본 없으면 정확 복원 불가 | 확정 |
| `/Game/Character/Hoshino/SK_Hoshino` | Skeletal Mesh | 플레이어/적 메시 | 원본 없으면 정확 복원 불가 | 확정 |
| `/Game/Character/Hoshino/PA_Hoshino` | Animation 관련 자산 | 캐릭터 애님 | 원본 없으면 정확 복원 불가 | 확정 |
| `/Game/Character/Hoshino/Materials/M_PinkBody` | Material | 플레이어 계열 외형 | 자동 저장본 존재 | 확정 |
| `/Game/Meshes/Materials/M_Gold` | Material | 코인 | 단색 대체 가능 | 확정 |
| `/Game/Meshes/Materials/M_GreenGem` | Material | 젬 | 단색 대체 가능 | 확정 |
| `/Game/Meshes/Materials/M_PinkBall` | Material | 발사체 | 단색 대체 가능 | 확정 |
| `/Game/Meshes/Materials/M_YellowBall` | Material | 여분/미사용 가능성 | 대체 허용 | 확정 |
| `/Game/Meshes/Materials/M_YellowBody` | Material | 적 외형 | 단색 대체 가능 | 확정 |
| `/Game/Props/Chest/Chest` | Static Mesh | 상자 외형 | 원본 없으면 정확 복원 불가 | 확정 |
| `/Game/Props/Chest/M_Chest` | Material | 상자 외형 | 원본 없으면 정확 복원 불가 | 확정 |
| `/Game/Props/Chest/chest_texture_albedo` | Texture | 상자 텍스처 | 원본 없으면 정확 복원 불가 | 확정 |
| `/Game/Props/Chest/chest_texture_mo` | Texture | 상자 텍스처 | 원본 없으면 정확 복원 불가 | 확정 |
| `/Game/Props/Chest/chest_texture_normal` | Texture | 상자 텍스처 | 원본 없으면 정확 복원 불가 | 확정 |

## 저장소 안의 복원 보조 자산

`Saved/Autosaves`에서 확인된 백업 후보:

- `Saved/Autosaves/Game/Blueprints/BP_Chest_Auto1.uasset`
- `Saved/Autosaves/Game/Blueprints/BP_Enemy_Auto3.uasset`
- `Saved/Autosaves/Game/Blueprints/BP_Player_Auto1.uasset`
- `Saved/Autosaves/Game/Blueprints/WBP_PlayerHUD_Auto1.uasset`
- `Saved/Autosaves/Game/Character/Hoshino/Materials/M_PinkBody_Auto1.uasset`
- `Saved/Autosaves/Game/Maps/BattleMap_Auto2.umap`
- `Saved/Autosaves/Game/UI/WBP_PlayerHUD_Auto2.uasset`

이 파일들은 `Content` 삭제 후 exact restore를 부분적으로 도와준다.

## 제외 자산

| 경로 | 이유 | 상태 |
| --- | --- | --- |
| `/Game/Blueprints/BP_EnemySpawner` | 저장소 안에 `PSVEnemySpawner` C++ 클래스가 없고, 맵 배치 흔적도 없다 | clean rebuild 제외 |

## 자산 복원 우선순위

1. 입력 3종
2. `BP_Projectile`, `BP_Gem`, `BP_Coin`
3. `BP_Player`, `BP_Enemy`, `BP_Chest`
4. `WBP_PlayerHUD`, `WBP_Gameover`, `WBP_HUD`
5. `BP_SVGameMode`
6. `BattleMap`
7. 외형 정밀 보정
