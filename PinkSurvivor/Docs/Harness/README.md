# PinkSurvivor Docs Harness

최종 수정: 2026-04-04

## 목적

`Docs/Harness`는 현재 `PinkSurvivor` 프로젝트의 실제 구현을 다시 만들거나 다른 브랜치로 옮겨도 같은 결과를 재현할 수 있도록 남기는 기준 문서 묶음이다.

이 문서 세트는 다음 정보를 고정한다.

- 어떤 클래스와 자산이 핵심 런타임 경로에 참여하는지
- 전투, 성장, 보상, 저장, UI가 어떤 순서로 연결되는지
- 현재 구현의 기본 수치와 재현 우선순위가 무엇인지
- 무엇이 이미 구현되었고 무엇이 아직 플레이스홀더인지

## 읽는 순서

1. `Docs/Harness/PROJECT_HARNESS_KO.md`
2. `Docs/Harness/CURRENT_PHASE_KO.md`
3. `Docs/Harness/DECISION_LOG_KO.md`
4. `Docs/Harness/STATE_SNAPSHOT_KO.md`
5. `Docs/Harness/WORK_TIME_LOG_KO.md`

## 문서별 역할

- `PROJECT_HARNESS_KO.md`: 프로젝트 목표, 재현 기준, 핵심 자산/클래스 매핑
- `CURRENT_PHASE_KO.md`: 현재 문서 기준으로 고정된 단계와 다음 우선순위
- `DECISION_LOG_KO.md`: 구현 방향과 예외 규칙
- `STATE_SNAPSHOT_KO.md`: 코드, 설정, 자산, 기본 수치의 현재 스냅샷
- `WORK_TIME_LOG_KO.md`: 문서/구현 갱신 이력

## 진실 소스 우선순위

- 1순위: `Source/`, `Config/`의 실제 코드와 설정
- 2순위: `Content/` 자산 경로와 블루프린트 참조 문자열 분석 결과
- 3순위: 이 문서

즉, 문서는 코드와 자산을 설명하는 수단이고, 모순이 생기면 실제 구현을 우선한다.

## 빠른 재현 체크

- 기본 맵은 `/Game/Maps/BattleMap.BattleMap` 이다.
- `GameInstanceClass`는 `/Script/PinkSurvivor.PSVGameInstance` 이다.
- 실제 플레이용 GameMode는 `/Game/Blueprints/BP_SVGameMode` 기준으로 잡는 것을 권장한다.
- 실제 플레이어는 `/Game/Blueprints/BP_Player` 기준으로 잡는 것을 권장한다.
- HUD는 이름과 달리 `WBP_HUD`가 `APSVHUD` 기반 블루프린트다.
- 플레이어는 `IA_Move`, `IA_Confirm`, `IMC_Default`를 사용한다.
- 적 처치 시 경험치 젬은 확정 드롭, 골드 코인은 확률 드롭이다.
- 경험치는 런 단위로 초기화되고, 골드는 세션 간 저장된다.
- 상자 룰렛은 현재 보상 적용 없는 연출용 플레이스홀더다.

## 주의

- 이전 문서에는 다른 프로젝트 이름과 잘못된 엔진 버전이 섞여 있었고, 이 폴더는 2026-04-04 기준으로 `PinkSurvivor`에 맞춰 재기준화되었다.
- `BattleMap`의 월드 세팅처럼 에디터에서 직접 열어야 완전 확정되는 정보는, 코드와 자산 참조 문자열로 확인 가능한 범위까지만 기록했다.
