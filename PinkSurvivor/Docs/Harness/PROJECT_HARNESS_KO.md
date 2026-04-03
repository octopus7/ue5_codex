# PinkSurvivor 프로젝트 하네스

최종 수정: 2026-04-04

## 문서 목적

이 문서는 `PinkSurvivor`의 현재 구현을 다른 작업자가 최대한 동일하게 재현할 수 있도록 프로젝트 목표, 고정 조건, 핵심 자산, 재현 기준을 정리한 기준 문서다.

프로젝트 방향은 다음 한 줄로 요약된다.

- `UE 5.5 기반의 3D Vampire Survivors 유사 게임을 만들되, 전투 루프와 성장 구조는 익숙하게 유지하고 표현과 카메라만 3D에 맞게 확장한다.`

## 프로젝트 요약

- 프로젝트명: `PinkSurvivor`
- 엔진 버전: `UE 5.5`
- 플랫폼 전제: `PC`
- 플레이 모델: `Single Player`
- 기본 맵: `/Game/Maps/BattleMap.BattleMap`
- 게임 인스턴스: `/Script/PinkSurvivor.PSVGameInstance`
- 현재 구현 상태: `전투 루프, 경험치/레벨, 영구 골드, HUD, 게임 오버, 상자 룰렛 플레이스홀더까지 구현`

## 동일 구현의 기준

다음 항목이 유지되면 현재 구현과 사실상 같은 플레이 감각으로 본다.

- 플레이어는 고정된 상공 시점 카메라 아래에서 자유 이동한다.
- 플레이어는 별도 조준 입력 없이 전방 자동 발사한다.
- 적은 플레이어를 단순 추적하고 근접 거리에서 주기적으로 피해를 준다.
- 적 사망 시 경험치 젬은 항상 떨어지고 골드 코인은 확률적으로 떨어진다.
- 경험치는 런 내부 성장이고, 골드는 `SaveGame`으로 세션 간 유지된다.
- HUD는 체력, 경험치, 레벨, 골드를 표시하고, 사망 시 게임 오버 UI를 띄운다.
- 상자는 게임을 일시정지하고 룰렛 텍스트를 돌린 뒤 확인 입력을 기다린다.
- 상자 결과는 아직 실제 보상을 적용하지 않는 플레이스홀더 상태다.

## 핵심 클래스와 자산 매핑

| 경로/이름 | 종류 | 역할 |
| --- | --- | --- |
| `APSVGameMode` | C++ | 기본 스폰 스크립트와 적 스폰 계산 |
| `/Game/Blueprints/BP_SVGameMode` | BP | 실제 플레이용 GameMode. `BP_Player`, `BP_Enemy`, `WBP_HUD` 참조 |
| `APSVPlayerCharacter` | C++ | 이동, 입력, 카메라, 체력, 경험치, 자동 발사, 사망 처리 |
| `/Game/Blueprints/BP_Player` | BP | 실제 플레이어 자산. `Hoshino`, `BP_Projectile`, 입력 자산 연결 |
| `APSVEnemyCharacter` | C++ | 추적, 근접 공격, 사망 드롭 |
| `/Game/Blueprints/BP_Enemy` | BP | 실제 적 자산. `Hoshino`, `BP_Gem`, `BP_Coin` 연결 |
| `UPSVAutoFireComponent` | C++ 컴포넌트 | 자동 발사 타이머와 발사체 생성 |
| `APSVProjectile` | C++ | 발사체 이동, 충돌 피해, 충격 임펄스 |
| `/Game/Blueprints/BP_Projectile` | BP | 실제 발사체 외형 |
| `UPSVExperienceComponent` | C++ 컴포넌트 | 경험치, 레벨, 임계치 계산 |
| `APSVExperienceGem` | C++ | 경험치 픽업 |
| `/Game/Blueprints/BP_Gem` | BP | 실제 경험치 젬 외형 |
| `UPSVGameInstance` | C++ | 영구 골드 로드/세이브 |
| `UPSVSaveGame` | C++ | `PersistentGold` 저장 |
| `APSVGoldCoin` | C++ | 영구 골드 픽업 |
| `/Game/Blueprints/BP_Coin` | BP | 실제 코인 외형 |
| `UPSVChestRouletteSubsystem` | C++ | 상자 룰렛 진행과 일시정지/재개 |
| `APSVRewardChest` | C++ | 상자 트리거 |
| `/Game/Blueprints/BP_Chest` | BP | 실제 상자 외형 |
| `APSVHUD` | C++ | HUD 위젯 생성, 룰렛 텍스트, 게임 오버 처리 |
| `/Game/UI/WBP_HUD` | BP | 실제 HUD 클래스. 이름은 WBP지만 `AHUD` 기반 |
| `/Game/UI/WBP_PlayerHUD` | Widget BP | 플레이 중 체력/경험치/골드 표시 |
| `/Game/UI/WBP_Gameover` | Widget BP | 게임 오버 확인 버튼 |

## 핵심 루프

1. 게임 시작 시 `UPSVGameInstance`가 `PlayerProgress` 슬롯을 로드하거나 생성한다.
2. `BattleMap`에서 GameMode가 시작되면 기본 스폰 스크립트를 예약한다.
3. 플레이어는 `IMC_Default`를 로컬 Enhanced Input 서브시스템에 등록한다.
4. 플레이어의 `UPSVAutoFireComponent`가 시작 즉시 발사체를 전방으로 주기 발사한다.
5. 적은 플레이어를 향해 이동하고, 사거리 안으로 들어오면 타이머 기반 근접 공격을 반복한다.
6. 적이 죽으면 경험치 젬과 확률적 골드 코인을 드롭한다.
7. 경험치 획득은 현재 런의 레벨을 올리고, 골드 획득은 즉시 저장된다.
8. 플레이어 사망 시 HUD가 게임 오버 위젯을 띄우고 게임이 일시정지된다.

## 재현 원칙

- 실제 플레이에는 가급적 C++ 기본 클래스가 아니라 프로젝트 자산이 연결된 블루프린트를 사용한다.
- `WBP_HUD`는 이름 때문에 위젯처럼 보이지만 실제로는 `APSVHUD` 기반 블루프린트다. 이 점을 그대로 유지한다.
- 입력은 `IA_Move`, `IA_Confirm`, `IMC_Default` 구조를 유지한다.
- 상자 룰렛은 `일시정지 상태에서도 확인 입력이 먹어야 한다`는 조건이 중요하다.
- 경험치는 `런 단위`, 골드는 `세션 단위`라는 이중 구조를 유지한다.
- 현재 적 AI는 Behavior Tree가 아니라 `Tick + Timer` 조합의 단순 구현이다.

## 현재 의도적 단순화

- 플레이어 무기 강화, 상자 보상 적용, 스탯 업그레이드는 아직 없다.
- 상자 룰렛 후보 텍스트는 `A/B/C/D/E`만 사용한다.
- 무한 웨이브 디렉터 대신 시간 기반 스폰 스크립트 4개만 내장되어 있다.
- 적 AI는 단순 추적과 근접 타격만 지원한다.
- HUD는 체력/경험치/골드/게임오버 중심이며, 화려한 피드백은 제한적이다.

## 재현 성공 조건

- `BattleMap` 진입 후 플레이어가 보이고 움직일 수 있다.
- 플레이어가 입력 없이 자동 발사하고 적에게 피해를 준다.
- 적이 플레이어를 추적하고 근접 피해를 준다.
- 적 처치 시 젬/코인이 정상 드롭된다.
- 경험치 바와 골드 표시가 HUD에 반영된다.
- 플레이어 사망 시 게임 오버 UI가 뜨고, 확인 버튼으로 게임이 재개된다.
- 상자 충돌 시 게임이 멈추고 룰렛 텍스트가 돈 뒤 확인을 요구한다.
