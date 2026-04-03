# PinkSurvivor 상태 스냅샷

기준 시각: 2026-04-04

## 스냅샷 기준

- 이 문서는 `Source/`, `Config/`, `Content/` 경로와 블루프린트 참조 문자열 분석을 기준으로 작성되었다.
- 에디터를 직접 열어야만 확정되는 일부 월드 세팅은 `권장 재현값`으로 표시한다.

## 환경 및 설정

| 항목 | 현재 값 |
| --- | --- |
| 엔진 | `UE 5.5` |
| 프로젝트 파일 | `PinkSurvivor.uproject` |
| 기본 맵 | `/Game/Maps/BattleMap.BattleMap` |
| 에디터 시작 맵 | `/Game/Maps/BattleMap.BattleMap` |
| 게임 인스턴스 | `/Script/PinkSurvivor.PSVGameInstance` |
| 저장 슬롯 | `PlayerProgress` |
| 입력 시스템 | `Enhanced Input` |

## 런타임 흐름

1. 게임 시작
2. `UPSVGameInstance::Init()`가 세이브를 로드하거나 생성
3. `BattleMap` 진입
4. 권장 GameMode인 `/Game/Blueprints/BP_SVGameMode`가 플레이어/HUD/적 클래스를 지정
5. 플레이어가 `IMC_Default`를 등록하고 자동 발사를 시작
6. GameMode가 시간 기반 스폰 스크립트로 적을 생성
7. 적이 플레이어를 추적하고 근접 공격
8. 플레이어 발사체가 적을 처치하면 젬/코인 드롭
9. 젬은 런 경험치를 올리고, 코인은 즉시 저장되는 영구 골드를 늘림
10. HUD가 체력/경험치/레벨/골드를 갱신
11. 상자와 겹치면 게임을 멈추고 룰렛 텍스트를 돌림
12. 플레이어 사망 시 게임 오버 UI를 띄우고 일시정지

## 핵심 C++ 시스템

| 시스템 | 현재 구현 |
| --- | --- |
| `APSVGameMode` | 플레이어 주변 원형 위치 계산으로 적 스폰 |
| `APSVPlayerCharacter` | 입력, 카메라, 체력/경험치 초기화, 넉백, 사망 처리 |
| `APSVEnemyCharacter` | Tick 추적, 사거리 판정, 타이머 근접 공격, 드롭 |
| `UPSVAutoFireComponent` | 일정 간격 자동 발사, 8방향 치트 지원 |
| `APSVProjectile` | 충돌 피해, 물리 임펄스, 3초 수명 |
| `UPSVHealthComponent` | 체력 값, 피해/회복, 사망 이벤트 |
| `UPSVExperienceComponent` | 누적 경험치, 레벨업, 임계치 계산 |
| `APSVExperienceGem` | 플레이어 겹침 시 경험치 지급 |
| `APSVGoldCoin` | 플레이어 겹침 시 영구 골드 증가 후 저장 |
| `UPSVGameInstance` | `UPSVSaveGame` 로드/생성/저장 |
| `UPSVChestRouletteSubsystem` | 룰렛 진행, HUD 표시, 일시정지/재개 |
| `APSVHUD` | 위젯 생성, 캐시 전달, 룰렛 텍스트 직접 드로우 |

## 기본 수치

| 항목 | 값 |
| --- | --- |
| 플레이어 체력 | `100` |
| 플레이어 이동 속도 | `600` |
| 플레이어 카메라 붐 길이 | `800` |
| 플레이어 카메라 회전 | `Pitch -55 / Yaw 0 / Roll 0` |
| 플레이어 피격 넉백 | `600` |
| 사망 임펄스 강도 | `450` |
| 사망 임펄스 상방 보정 | `0.15` |
| 자동 발사 간격 | `0.5초` |
| 발사 위치 오프셋 | `X 100 / Y 0 / Z 80` |
| 발사체 속도 | `1600` |
| 발사체 피해 | `10` |
| 발사체 충격 임펄스 | `1000` |
| 적 체력 | `40` |
| 적 이동 속도 | `300` |
| 적 공격 범위 | `150` |
| 적 공격 허용 여유 | `50` |
| 적 공격 간격 | `1.0초` |
| 적 공격 피해 | `8` |
| 적 피격 넉백 | `600` |
| 골드 드롭 확률 | `0.05` |
| 드롭 Z 오프셋 | `30` |
| 경험치 임계치 | `10, 25, 45, 70` |
| 상자 룰렛 길이 | `2.0초` |
| 룰렛 전환 간격 | `0.1초` |

## 스폰 스크립트

`APSVGameMode`는 아래 명령을 내장한다.

| 시간 | 원본 적 수 | 반경 | 개별 간격 | 기본 스칼라 0.25 적용 시 실제 수 |
| --- | --- | --- | --- | --- |
| `0초` | `6` | `600` | `0` | `1` |
| `5초` | `10` | `750` | `0.2초` | `2` |
| `12초` | `12` | `850` | `0.15초` | `3` |
| `20초` | `16` | `900` | `0.08초` | `4` |

추가 규칙:

- 적은 플레이어 위치를 기준으로 원형 바깥에서 생성된다.
- 생성 높이는 플레이어 Z에서 `+50` 이다.
- 생성 직후 플레이어 방향을 바라보도록 회전한다.

## 블루프린트 및 위젯 자산 매핑

| 자산 | 부모/형태 | 확인된 참조 또는 용도 |
| --- | --- | --- |
| `/Game/Blueprints/BP_SVGameMode` | `APSVGameMode` BP | `BP_Player`, `BP_Enemy`, `WBP_HUD` 참조 |
| `/Game/Blueprints/BP_Player` | `APSVPlayerCharacter` BP | `BP_Projectile`, `Hoshino`, `IA_Move`, `IA_Confirm`, `IMC_Default` 참조 |
| `/Game/Blueprints/BP_Enemy` | `APSVEnemyCharacter` BP | `BP_Gem`, `BP_Coin`, `Hoshino`, `M_YellowBody` 참조 |
| `/Game/Blueprints/BP_Projectile` | `APSVProjectile` BP | 엔진 기본 `Sphere` 메시 + `M_PinkBall` |
| `/Game/Blueprints/BP_Gem` | `APSVExperienceGem` BP | 엔진 기본 `Cube` 메시 + `M_GreenGem` |
| `/Game/Blueprints/BP_Coin` | `APSVGoldCoin` BP | 엔진 기본 `Cylinder` 메시 + `M_Gold` |
| `/Game/Blueprints/BP_Chest` | `APSVRewardChest` BP | `/Game/Props/Chest/Chest` 메시 사용 |
| `/Game/UI/WBP_HUD` | `APSVHUD` BP | `WBP_PlayerHUD`, `WBP_Gameover` 클래스 연결 |
| `/Game/UI/WBP_PlayerHUD` | `UPSVPlayerHUDWidget` Widget BP | 체력, 경험치, 레벨, 골드 이벤트 처리 |
| `/Game/UI/WBP_Gameover` | `UPSVGameOverWidget` Widget BP | `OkButton`으로 일시정지 해제 |

## 메시와 아트 자산

| 자산 | 용도 |
| --- | --- |
| `/Game/Character/Hoshino/Hoshino` | 플레이어/적 스켈레탈 메시 기반 |
| `/Game/Character/Hoshino/SK_Hoshino` | Hoshino 스켈레탈 메시 자산 |
| `/Game/Character/Hoshino/PA_Hoshino` | Hoshino 관련 애님 자산 |
| `/Game/Character/Hoshino/Materials/M_PinkBody` | 플레이어 계열 외형용 머티리얼 자산 |
| `/Game/Meshes/Materials/M_YellowBody` | 적 외형용 머티리얼 |
| `/Game/Meshes/Materials/M_PinkBall` | 발사체 머티리얼 |
| `/Game/Meshes/Materials/M_GreenGem` | 경험치 젬 머티리얼 |
| `/Game/Meshes/Materials/M_Gold` | 코인 머티리얼 |
| `/Game/Props/Chest/Chest` | 상자 메시 |
| `/Game/Props/Chest/M_Chest` | 상자 머티리얼 |

## HUD 상태

- `APSVHUD`는 `PlayerHUDWidgetClass`, `GameOverWidgetClass`를 통해 위젯을 생성한다.
- `WBP_HUD`가 이 두 클래스를 실제 자산으로 연결하는 중심 자산이다.
- `WBP_PlayerHUD`에서는 최소한 `ProgressBar_HP`, `TextBlock_HP` 위젯이 확인된다.
- `WBP_PlayerHUD`는 `OnHealthChanged`, `OnExperienceChanged`, `OnLevelUp`, `OnPersistentGoldChanged`, `OnPlayerDied` 이벤트를 처리한다.
- 룰렛 메시지는 `UUserWidget`이 아니라 `APSVHUD::DrawHUD()`에서 직접 텍스트로 그린다.

## 상자 룰렛 상태

- 후보 텍스트: `A, B, C, D, E`
- 룰렛 시작 시 게임을 일시정지한다.
- `FTSTicker`를 사용하므로 일시정지 중에도 후보 텍스트가 바뀐다.
- 시간이 끝나면 `Press X / OK to confirm` 메시지를 띄운다.
- `ConfirmAction`은 `bTriggerWhenPaused = true` 로 설정된다.
- 확인하면 게임을 재개하고 HUD 메시지를 비운다.
- 실제 업그레이드 적용은 아직 없다.

## 저장 상태

- 저장 데이터는 `UPSVSaveGame::PersistentGold` 단일 값이다.
- 골드 코인 습득 시 즉시 저장된다.
- 경험치/레벨은 저장되지 않는다.

## 동일 구현 재현 절차

1. `DefaultEngine.ini`에서 기본 맵과 `GameInstanceClass`를 맞춘다.
2. C++ 클래스 구조를 현재 `Source/PinkSurvivor`와 동일하게 만든다.
3. `BP_Player`, `BP_Enemy`, `BP_SVGameMode`, `WBP_HUD`를 같은 경로에 만든다.
4. `BP_Player`에 `Hoshino`, `BP_Projectile`, `IA_Move`, `IA_Confirm`, `IMC_Default`를 연결한다.
5. `BP_Enemy`에 `Hoshino`, `BP_Gem`, `BP_Coin`을 연결한다.
6. `WBP_HUD`에 `WBP_PlayerHUD`, `WBP_Gameover`를 연결한다.
7. 스폰 수치, 경험치 임계치, 자동 발사 주기, 공격 범위를 표와 동일하게 맞춘다.
8. 상자 룰렛은 실제 보상 없이 일시정지 연출만 먼저 맞춘다.

## 확인되지 않았거나 제한된 항목

- `IA_Move`, `IA_Confirm`의 실제 키 바인딩은 에디터 확인이 있으면 더 정확하다.
- `BattleMap` 월드 세팅의 GameMode 지정은 맵을 열어 확인하면 가장 확실하다.
- `BP_EnemySpawner`는 자산이 존재하지만 현재 핵심 구현 경로에서의 역할을 문서 기준으로 확정하지 못했다.
- `WBP_PlayerHUD`의 세부 레이아웃은 자산 이벤트와 일부 위젯 이름까지만 확인했다.

## 재현 리스크

- `APSVEnemyCharacter`의 `MoveSpeed`를 블루프린트에서 바꿔도 `CharacterMovement->MaxWalkSpeed`와 별도로 관리되므로, 값 변경 시 둘을 같이 맞추는 편이 안전하다.
- `APSVGameMode`는 C++ 기본값만으로도 동작하지만, 실제 외형과 연결을 맞추려면 `BP_SVGameMode` 기반으로 플레이해야 한다.
- `WBP_HUD`의 이름이 일반 위젯처럼 보여도 실제 타입은 `AHUD` 기반이라, 단순 Widget BP로 대체하면 구조가 달라진다.
