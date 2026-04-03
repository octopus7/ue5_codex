# PinkSurvivor 현재 단계

최종 수정: 2026-04-04

## 현재 단계

- 단계 ID: `DOCS-PARITY-LOCK`
- 단계명: `현재 구현 동결 및 동일 구현 재현 문서화`
- 상태: `완료`

## 이번 단계 목표

기존 `TopDownTestOne` 기준 문서를 제거하고, `PinkSurvivor` 실제 코드와 자산 연결을 기준으로 재현 문서를 새로 고정한다.

## 완료 기준

- 프로젝트명, 엔진 버전, 맵, 인스턴스 클래스가 실제 프로젝트와 일치한다.
- 전투, 성장, 저장, HUD, 상자 연출 흐름이 문서에 반영된다.
- 블루프린트와 위젯 자산 경로가 실제 `Content/` 구조와 맞는다.
- 재현자가 무엇을 먼저 만들어야 하는지 단계별 우선순위를 문서만 보고 알 수 있다.

## 이번 단계 결과

- `Docs/Harness` 전체를 `PinkSurvivor` 기준으로 재작성했다.
- `APSVGameMode`, `APSVPlayerCharacter`, `APSVEnemyCharacter`, 각 컴포넌트와 픽업, HUD, 저장 흐름을 문서화했다.
- `BP_Player`, `BP_Enemy`, `BP_SVGameMode`, `WBP_HUD`, `WBP_PlayerHUD`, `WBP_Gameover`의 존재와 참조 관계를 정리했다.
- 기존 문서의 다른 프로젝트명과 잘못된 엔진 버전, 깨진 인코딩 상태를 제거했다.

## 동일 구현 재현 우선순위

1. `Config/DefaultEngine.ini`의 기본 맵과 `GameInstanceClass`를 맞춘다.
2. C++ 클래스 구조를 먼저 만든다.
3. `BP_SVGameMode`, `BP_Player`, `BP_Enemy`, `WBP_HUD`를 자산 경로까지 동일하게 만든다.
4. `BP_Player`에 `IMC_Default`, `IA_Move`, `IA_Confirm`, `BP_Projectile`, `Hoshino`를 연결한다.
5. `BP_Enemy`에 `BP_Gem`, `BP_Coin`, `Hoshino`를 연결한다.
6. `WBP_HUD`에 `WBP_PlayerHUD`, `WBP_Gameover`를 연결한다.
7. 스폰 스크립트와 기본 수치를 문서와 동일하게 맞춘다.

## 다음 구현 우선순위

- `GAMEPLAY-POLISH-01`: 상자 룰렛 결과를 실제 업그레이드로 연결
- `GAMEPLAY-POLISH-02`: 20초 이후에도 이어지는 지속 웨이브/난이도 상승 루프 추가
- `GAMEPLAY-POLISH-03`: HUD에 경험치/레벨/골드 표현을 더 명확하게 보강
- `GAMEPLAY-POLISH-04`: 적/플레이어 피격 피드백과 드롭 연출 강화

## 보류 또는 확인 필요 항목

- `BP_EnemySpawner` 자산은 존재하지만 현재 핵심 경로 참조 여부를 문서 기준으로 확정하지 못했다.
- `BattleMap`의 월드 세팅에서 실제로 어떤 GameMode가 지정되어 있는지는 에디터 확인이 있으면 더 확정적이다.
- `IA_Move`, `IA_Confirm`의 실제 키 바인딩은 자산 본문을 에디터에서 열어야 정확히 확인할 수 있다.
