# PinkSurvivor Docs Harness

최종 수정: 2026-04-04

## 목적

`Docs/Harness`는 `PinkSurvivor/Content`를 지운 뒤 다시 만드는 상황까지 고려한 복원 문서 세트다.

이 문서 세트는 세 가지 목표를 가진다.

- 현재 `Source/`, `Config/`, `Content/`가 어떤 구조로 맞물려 동작하는지 고정한다.
- `Content` 삭제 후에도 최소한 `플레이 감각이 같은 상태`까지 다시 세우는 절차를 남긴다.
- 어디까지가 `정확 복원 가능`, 어디부터가 `대체 복원`인지 경계를 분명히 적는다.

## 복원 목표 등급

- `Gameplay-equivalent`: 전투 루프, 성장, 저장, HUD, 상자 연출이 현재와 같은 방식으로 동작한다.
- `Asset-path-equivalent`: 핵심 자산 이름, 경로, 부모 클래스, 클래스 참조가 현재와 같게 복원된다.
- `Binary-identical`: 원본 `uasset`와 같은 바이너리 상태. 이 등급은 문서만으로는 보장하지 않는다.

## 읽는 순서

1. `Docs/Harness/CONTENT_REBUILD_PLAYBOOK_KO.md`
2. `Docs/Harness/CONTENT_ASSET_MANIFEST_KO.md`
3. `Docs/Harness/PROJECT_HARNESS_KO.md`
4. `Docs/Harness/STATE_SNAPSHOT_KO.md`
5. `Docs/Harness/CURRENT_PHASE_KO.md`
6. `Docs/Harness/DECISION_LOG_KO.md`
7. `Docs/Harness/WORK_TIME_LOG_KO.md`

## 문서별 역할

- `CONTENT_REBUILD_PLAYBOOK_KO.md`: `Content`를 지운 뒤 다시 세우는 순서와 검증 절차
- `CONTENT_ASSET_MANIFEST_KO.md`: 자산별 경로, 부모 클래스, 연결 대상, 복원 규칙
- `PROJECT_HARNESS_KO.md`: 프로젝트 목표, 동일 구현 기준, 복원 계약
- `STATE_SNAPSHOT_KO.md`: 코드/설정/자산/맵/입력의 현재 상태 스냅샷
- `CURRENT_PHASE_KO.md`: 문서화 단계와 남은 리스크
- `DECISION_LOG_KO.md`: 재현에 영향을 주는 설계 결정
- `WORK_TIME_LOG_KO.md`: 문서/구현 갱신 이력

## 진실 소스 우선순위

- 1순위: `Source/`, `Config/`
- 2순위: `Saved/Autosaves/` 안의 생존 자산
- 3순위: `Content/` 자산 경로와 참조 문자열 분석 결과
- 4순위: 이 문서

즉, 문서는 복원 지침이고, 실제 코드/설정/생존 자산이 있으면 그것을 우선한다.

## 삭제 전 핵심 원칙

- `Content`를 지우더라도 `Source`, `Config`, `Saved`는 남기는 것을 기본 전제로 한다.
- 정확한 외형 복원이 필요하면 `Content/Character/Hoshino`, `Content/Props/Chest`, `Content/Meshes/Materials`를 먼저 백업하거나 원본 소스를 별도로 보존해야 한다.
- `Saved/Autosaves`에는 `BP_Player`, `BP_Enemy`, `BP_Chest`, `WBP_PlayerHUD`, `BattleMap` 등의 일부 백업본이 있으므로, `Content` 삭제 후 가장 먼저 확인한다.
- `BP_EnemySpawner`는 현재 코드 기준 핵심 런타임 경로에 없고, 대응 C++ 클래스도 저장소에 없으므로 clean rebuild 목표에서 제외한다.

## 빠른 체크

- 엔진 기준은 `UE 5.5`다.
- 기본 맵은 `/Game/Maps/BattleMap.BattleMap`이다.
- `GameInstanceClass`는 `/Script/PinkSurvivor.PSVGameInstance`다.
- 실제 플레이 기준 GameMode는 `/Game/Blueprints/BP_SVGameMode`다.
- 실제 플레이 기준 Player는 `/Game/Blueprints/BP_Player`다.
- `WBP_HUD`는 이름과 달리 `APSVHUD` 기반 블루프린트다.
- 입력 자산은 `IA_Move`, `IA_Confirm`, `IMC_Default` 3개다.
- 적 처치 시 경험치 젬은 확정 드롭, 골드 코인은 확률 드롭이다.
- 경험치는 런 단위로 초기화되고, 골드는 세션 간 저장된다.
- 상자 룰렛은 현재 보상 적용 없는 연출용 플레이스홀더다.

## 주의

- 정확한 `uasset` 복원과 같은 바이너리 상태는 문서만으로 보장할 수 없다.
- 특히 `Hoshino`와 `Chest` 계열 아트 자산은 외부 원본이나 생존 백업이 없으면 `동일 외형` 복원이 불가능하다.
- 이 문서의 목표는 그 한계를 공개한 상태에서 `같은 게임 구조`를 다시 세우는 것이다.
