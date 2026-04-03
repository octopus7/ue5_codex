# PinkSurvivor 현재 단계

최종 수정: 2026-04-04

## 현재 단계

- 단계 ID: `DOCS-REBUILD-RUNBOOK`
- 단계명: `Content 삭제 후 재구축 가능한 수준으로 문서 보강`
- 상태: `완료`

## 이번 단계 목표

기존 동일 구현 문서를 넘어서, `PinkSurvivor/Content`를 지운 뒤 다시 만들어도 플레이 가능한 수준까지 올라오는 재구축 문서를 남긴다.

## 완료 기준

- `Content` 삭제 후 어떤 순서로 무엇을 만들어야 하는지 플레이북이 존재한다.
- 핵심 자산마다 부모 클래스, 경로, 연결 대상이 자산 매니페스트로 정리된다.
- 입력, HUD, 맵, 백업 가능 자산, exact 복원 불가 영역이 분리 기록된다.
- 재현자가 `Gameplay-equivalent`와 `Asset-path-equivalent` 목표를 문서만 보고 달성할 수 있다.

## 이번 단계 결과

- `CONTENT_REBUILD_PLAYBOOK_KO.md`를 추가해 삭제 후 복원 순서를 고정했다.
- `CONTENT_ASSET_MANIFEST_KO.md`를 추가해 자산별 경로, 부모, 연결, 복원 규칙을 정리했다.
- 입력 자산은 `IA_Move = Axis2D`, `IA_Confirm = Boolean`, `IMC_Default`의 유력 키 구성을 문서화했다.
- `BattleMap`은 월드 세팅, 액터 목록, 기본 지오메트리 단서를 문서화했다.
- `Saved/Autosaves`에 남아 있는 실복원용 백업 후보를 문서화했다.
- exact 복원이 어려운 영역과 대체 복원 허용 영역을 분리했다.

## 다음 구현 우선순위

- `DOCS-AUTOMATION-01`: 문서 내용을 실제 자산 생성 스크립트 또는 에디터 자동화로 내리는 작업
- `ART-BACKUP-01`: `Hoshino`, `Chest` 원본 아트의 별도 백업 경로 확정
- `GAMEPLAY-POLISH-01`: 상자 룰렛 결과를 실제 업그레이드로 연결
- `GAMEPLAY-POLISH-02`: 20초 이후에도 이어지는 지속 웨이브, 난이도 상승 루프 추가
- `GAMEPLAY-POLISH-03`: HUD에 경험치, 레벨, 골드 표현을 더 명확하게 보강
- `GAMEPLAY-POLISH-04`: 적, 플레이어 피격 피드백과 드롭 연출 강화

## 보류 또는 확인 필요 항목

- `Hoshino`, `Chest` 원본 아트는 저장소 밖 원본이 없으면 exact 복원 불가다.
- `WBP_PlayerHUD`의 정확한 위치, 간격, 스타일은 현재 자료상 일부만 확정된다.
- `BattleMap`의 정확한 transform은 에디터 확인이 있으면 더 확정된다.
- `IA_Move`의 `W/A/S/D + Negate/SwizzleAxis` 조합은 강한 추정이지만, modifier가 어느 키에 붙는지는 에디터가 가장 확실하다.
- `BP_EnemySpawner`는 현재 clean rebuild에서 제외하지만, 자산 자체를 완전히 폐기할지 여부는 아직 결정하지 않았다.
