# Harness README

이 문서는 `CodexHarness`의 장기 작업을 하네스 문서 세트로 지시하는 방법을 설명한다.

## 사용 목적

이 문서 세트는 한 번의 단발성 요청이 아니라, 여러 단계에 걸친 장기 작업을 안정적으로 이어가기 위한 기준점이다.

핵심 아이디어는 아래와 같다.
- `PROJECT_HARNESS_KO.md`로 전체 목표와 전역 제약을 고정한다.
- `CURRENT_PHASE_KO.md`로 이번 실행 범위를 자른다.
- `DECISION_LOG_KO.md`로 새 규칙과 예외를 누적한다.
- `STATE_SNAPSHOT_KO.md`로 현재 실제 상태를 기록한다.

## 읽게 할 문서

작업을 지시할 때는 기본적으로 아래 4개 문서를 읽게 하면 된다.

- `d:\github\ue5_codex\CodexHarness\Docs\Harness\PROJECT_HARNESS_KO.md`
- `d:\github\ue5_codex\CodexHarness\Docs\Harness\CURRENT_PHASE_KO.md`
- `d:\github\ue5_codex\CodexHarness\Docs\Harness\DECISION_LOG_KO.md`
- `d:\github\ue5_codex\CodexHarness\Docs\Harness\STATE_SNAPSHOT_KO.md`

## 가장 기본적인 지시 방식

가장 기본적인 요청은 아래처럼 하면 된다.

```text
다음 문서를 읽고 작업해:
- d:\github\ue5_codex\CodexHarness\Docs\Harness\PROJECT_HARNESS_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\CURRENT_PHASE_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\DECISION_LOG_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\STATE_SNAPSHOT_KO.md

문서 규칙을 따르고, 현재 단계 범위까지만 구현해.
작업 전에 계획을 요약하고, 완료 후 문서도 갱신해.
```

## 추천 지시 패턴

### 1. 현재 단계만 수행

```text
다음 문서들을 읽고 현재 단계만 수행해:
- d:\github\ue5_codex\CodexHarness\Docs\Harness\PROJECT_HARNESS_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\CURRENT_PHASE_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\DECISION_LOG_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\STATE_SNAPSHOT_KO.md

CURRENT_PHASE_KO.md의 멈출 기준에서 멈춰.
작업 전 계획 요약, 작업 후 문서 갱신과 검증 결과 보고까지 해.
```

### 2. 다음 단계까지 이어서 수행

```text
다음 문서들을 읽고 작업해:
- d:\github\ue5_codex\CodexHarness\Docs\Harness\PROJECT_HARNESS_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\CURRENT_PHASE_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\DECISION_LOG_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\STATE_SNAPSHOT_KO.md

현재 단계가 끝났다면 CURRENT_PHASE_KO.md를 다음 추천 단계로 갱신한 뒤 그 단계도 수행해.
각 단계마다 멈출 기준을 지키고, 단계 완료 후 문서를 갱신해.
막히면 한 줄 질문만 해.
```

### 3. 프로젝트 목표 달성까지 계속 수행

```text
다음 문서들을 작업 기준으로 사용해:
- d:\github\ue5_codex\CodexHarness\Docs\Harness\PROJECT_HARNESS_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\CURRENT_PHASE_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\DECISION_LOG_KO.md
- d:\github\ue5_codex\CodexHarness\Docs\Harness\STATE_SNAPSHOT_KO.md

프로젝트 목표 달성까지 단계별로 계속 진행해.
각 단계 시작 전 계획 요약, 단계 완료 후 문서를 갱신해.
명시된 전역 제약과 멈출 기준은 지켜.
치명적 모호함이 있을 때만 질문해.
```

## 지시문에 같이 붙이면 좋은 한 줄

아래 중 하나를 같이 붙이면 실행 경계가 더 명확해진다.

- `현재 단계까지만 해`
- `다음 단계까지 자동 진행해`
- `프로젝트 목표 달성까지 반복해`

## 작업이 실제로 진행되는 방식

문서를 읽고 작업하라고 지시하면, 일반적으로 아래 순서로 진행된다.

1. `PROJECT_HARNESS_KO.md`에서 전체 목표와 전역 제약을 파악한다.
2. `CURRENT_PHASE_KO.md`에서 이번 실행 경계와 멈출 기준을 확인한다.
3. `DECISION_LOG_KO.md`에서 누적된 규칙과 예외를 확인한다.
4. `STATE_SNAPSHOT_KO.md`에서 현재 실제 코드와 자산 상태를 확인한다.
5. 그 기준에 맞게 구현한다.
6. 작업 결과를 다시 문서에 반영한다.

## 문서 갱신 규칙

장기 작업이 유지되려면 구현만 하지 말고 문서를 같이 갱신해야 한다.

- 현재 단계가 끝나면 `CURRENT_PHASE_KO.md`를 갱신한다.
- 새 제약이나 새 규칙이 생기면 `DECISION_LOG_KO.md`에 기록한다.
- 실제 코드, 설정, 애셋 상태가 달라지면 `STATE_SNAPSHOT_KO.md`를 갱신한다.
- 큰 방향이나 마일스톤이 바뀌면 `PROJECT_HARNESS_KO.md`를 갱신한다.

## 중요한 주의점

- `어느 파일을 읽어라`만 말하면 기본 방향은 잡힌다.
- 하지만 장기 작업에서는 `어디까지 진행할지`도 함께 말하는 것이 좋다.
- 가장 안정적인 형식은 `읽을 문서 + 이번 실행 경계 + 작업 후 문서 갱신 요구`의 3요소를 같이 주는 방식이다.
