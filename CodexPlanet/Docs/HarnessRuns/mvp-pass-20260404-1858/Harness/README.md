# CodexPlanet Harness README

`CodexPlanet`의 장기 작업 문서 세트다. 상위 `CodexHarness`에서는 문서 구조만 참고했고, 내용은 현재 프로젝트의 실제 방향에 맞게 다시 작성했다.

## 문서 구성

- `Docs/Harness/PROJECT_HARNESS_KO.md`
- `Docs/Harness/CURRENT_PHASE_KO.md`
- `Docs/Harness/DECISION_LOG_KO.md`
- `Docs/Harness/STATE_SNAPSHOT_KO.md`
- `Docs/Harness/WORK_TIME_LOG_KO.md`

## 각 문서 역할

- `PROJECT_HARNESS_KO.md`: 프로젝트 목표, 범위, 원칙, 마일스톤
- `CURRENT_PHASE_KO.md`: 지금 진행해야 할 단계의 목표와 멈출 기준
- `DECISION_LOG_KO.md`: 구조, 범위, 규칙 관련 결정 기록
- `STATE_SNAPSHOT_KO.md`: 코드, 설정, 콘텐츠의 현재 상태 요약
- `WORK_TIME_LOG_KO.md`: 단계별 작업 시간 append 로그

## 운영 규칙

- 작업 전에는 위 5개 문서를 모두 읽는다.
- 실제 작업은 `CURRENT_PHASE_KO.md` 범위 안에서만 진행한다.
- 새 규칙이나 예외는 `DECISION_LOG_KO.md`에 먼저 남긴다.
- 코드, 설정, 애셋, 폴더 구조가 바뀌면 `STATE_SNAPSHOT_KO.md`를 갱신한다.
- 단계가 끝나면 `CURRENT_PHASE_KO.md`와 `WORK_TIME_LOG_KO.md`를 함께 갱신한다.
- 작업 시간을 줄일 수 있으면 탐색, 구현, 검증을 병렬 수행한다.
- 병렬 수행 여부 판단에서는 토큰 비용보다 전체 리드타임 단축을 우선한다.
- 필요하면 `generator`와 `evaluator`를 분리된 컨텍스트로 운영한다.
- `generator`는 초안 구현과 코드 생산, `evaluator`는 리뷰, 리스크 점검, 검증 관점 평가를 맡는다.
- 두 에이전트는 직접 통신하지 않고 메인 에이전트가 결과와 피드백을 중계한다.

## 저장 구조

- `Docs/HarnessSeed`
  - 재현성 확인용 고정 seed 문서
  - 초기 기준 상태를 보존한다.
- `Docs/Harness`
  - 실제 작업 중 계속 갱신되는 live 문서
- `Docs/HarnessRuns`
  - run별 결과 아카이브

## 재현성 운영 방식

- 새로 작업을 시작하거나 다시 수행해 보고 싶을 때는 `Docs/Harness`를 직접 수동 정리하지 않는다.
- 대신 `Docs/HarnessSeed`를 `Docs/Harness`로 복원하는 방식으로 초기화한다.
- 실행이 끝난 결과는 `Docs/HarnessRuns/<run-id>`로 보관해 seed 대비 결과 비교가 가능해야 한다.
- seed는 고정 입력, harness는 가변 출력으로 취급한다.
- 수행 중 생성되는 문서 변경과 작업 기록은 `Docs/Harness`의 live 문서에서만 반영한다.
- `Docs/HarnessSeed`는 보존용 원본이므로 수행 중 직접 수정하지 않는다.
- seed 갱신이 필요하면 run 종료 후 별도 기준선 갱신 작업으로만 다룬다.
- `generator/evaluator` 분리 운영을 사용했다면 역할 분리와 메인 에이전트 중계 방식은 `CURRENT_PHASE_KO.md`와 `WORK_TIME_LOG_KO.md`에 남긴다.

## 추가 실행 문서

- `Docs/Harness/FULL_EXECUTION_INSTRUCTION_KO.md`
  - 새 대화에서 전체 계획을 이어서 수행할 때 붙여 넣는 실행 지시문

## 관련 스크립트

- `Tools/harness/reset_harness.py`
  - `Docs/HarnessSeed`를 기준으로 `Docs/Harness`를 초기화
- `Tools/harness/archive_harness.py`
  - 현재 `Docs/Harness`를 `Docs/HarnessRuns/<run-id>`로 아카이브

## 현재 문서 기준

- 이 프로젝트는 탑다운 슈터와 무관하다.
- 현재 컨셉은 `행성 하나를 관찰하고 회전시키며 표면에 프랍을 배치하는 경영/배치 프로토타입`이다.
- 핵심 상호작용은 `드래그 기반 트랙볼 회전`과 `행성 표면 배치`다.
- 프랍은 행성 표면뿐 아니라 이미 배치된 오브젝트 표면에도 적층 가능해야 한다.
- 배치 시 오브젝트는 접촉한 표면 삼각형의 노멀 방향을 따라 붙도록 정렬되는 것을 기본 규칙으로 둔다.
- 행성 본체는 `코드로 생성한 약간의 노이즈가 있는 스피어`를 초기 기준으로 사용한다.
- 주요 콘텐츠 방향은 `행성은 코드 생성`, `프랍은 cute 스타일의 VOX 제작 및 게임 애셋화`다.
- 행성의 `고리 Niagara System` 또는 `위성`을 추가하는 UI는 런타임 조립형이 아니라 에디터에서 확인 가능한 실존 `WBP` 애셋이어야 한다.
