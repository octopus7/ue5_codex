# Harness README

이 문서는 `TopDownTestOne`의 장기 작업을 하네스 문서 세트로 지시하는 방법을 설명한다.

## 사용 목적

이 문서 세트는 여러 단계에 걸친 작업을 대화 기록에만 의존하지 않고 이어가기 위한 기준점이다.

핵심 문서는 아래 5개다.
- `PROJECT_HARNESS_KO.md`: 전체 목표, 전역 제약, 구현 원칙
- `CURRENT_PHASE_KO.md`: 이번 실행 범위와 멈출 기준
- `DECISION_LOG_KO.md`: 새 규칙과 예외, 운영 결정
- `STATE_SNAPSHOT_KO.md`: 현재 실제 코드, 설정, 애셋 상태
- `WORK_TIME_LOG_KO.md`: 단계별 시간과 병렬 수행 여부를 append 형식으로 누적

## 읽게 할 문서

작업을 지시할 때는 기본적으로 아래 5개 문서를 함께 읽게 하면 된다.

- `Docs/Harness/PROJECT_HARNESS_KO.md`
- `Docs/Harness/CURRENT_PHASE_KO.md`
- `Docs/Harness/DECISION_LOG_KO.md`
- `Docs/Harness/STATE_SNAPSHOT_KO.md`
- `Docs/Harness/WORK_TIME_LOG_KO.md`

## 가장 기본적인 지시 방식

```text
다음 문서를 읽고 작업해:
- Docs/Harness/PROJECT_HARNESS_KO.md
- Docs/Harness/CURRENT_PHASE_KO.md
- Docs/Harness/DECISION_LOG_KO.md
- Docs/Harness/STATE_SNAPSHOT_KO.md
- Docs/Harness/WORK_TIME_LOG_KO.md

문서 규칙을 따르고, 현재 단계 범위까지만 구현해.
작업 전에 계획을 요약하고, 완료 후 문서도 갱신해.
병렬 수행이 작업 시간을 줄이면 허용하고, 단계별 소요 시간은 WORK_TIME_LOG_KO.md에 append 해.
```

## 추천 지시 패턴

### 1. 현재 단계만 수행

```text
다음 문서들을 읽고 현재 단계만 수행해:
- Docs/Harness/PROJECT_HARNESS_KO.md
- Docs/Harness/CURRENT_PHASE_KO.md
- Docs/Harness/DECISION_LOG_KO.md
- Docs/Harness/STATE_SNAPSHOT_KO.md
- Docs/Harness/WORK_TIME_LOG_KO.md

CURRENT_PHASE_KO.md의 멈출 기준에서 멈춰.
작업 전 계획 요약, 작업 후 문서 갱신과 검증 결과 보고까지 해.
병렬화가 더 빠르면 사용하고, 단계 시간 기록도 남겨.
```

### 2. 다음 단계까지 이어서 수행

```text
다음 문서들을 읽고 작업해:
- Docs/Harness/PROJECT_HARNESS_KO.md
- Docs/Harness/CURRENT_PHASE_KO.md
- Docs/Harness/DECISION_LOG_KO.md
- Docs/Harness/STATE_SNAPSHOT_KO.md
- Docs/Harness/WORK_TIME_LOG_KO.md

현재 단계가 끝났다면 CURRENT_PHASE_KO.md를 다음 추천 단계로 갱신한 뒤 그 단계도 수행해.
각 단계마다 멈출 기준을 지키고, 단계 완료 후 문서를 갱신해.
작업 시간을 줄일 수 있으면 병렬 수행하고, 단계별 시간 기록을 append 해.
막히면 한 줄 질문만 해.
```

### 3. 프로젝트 목표 달성까지 계속 수행

```text
다음 문서들을 작업 기준으로 사용해:
- Docs/Harness/PROJECT_HARNESS_KO.md
- Docs/Harness/CURRENT_PHASE_KO.md
- Docs/Harness/DECISION_LOG_KO.md
- Docs/Harness/STATE_SNAPSHOT_KO.md
- Docs/Harness/WORK_TIME_LOG_KO.md

프로젝트 목표 달성까지 단계별로 계속 진행해.
각 단계 시작 전 계획 요약, 단계 완료 후 문서를 갱신해.
작업 시간이 줄어들면 병렬 수행을 허용하고, 단계별 시간 기록을 누적해.
명시된 전역 제약과 멈출 기준은 지켜.
치명적 모호함이 있을 때만 질문해.
```

## 작업이 실제로 진행되는 방식

문서를 읽고 작업하라고 지시하면, 일반적으로 아래 순서로 진행된다.

1. `PROJECT_HARNESS_KO.md`에서 전체 목표와 전역 제약을 파악한다.
2. `CURRENT_PHASE_KO.md`에서 이번 실행 경계와 멈출 기준을 확인한다.
3. `DECISION_LOG_KO.md`에서 누적된 규칙과 예외를 확인한다.
4. `STATE_SNAPSHOT_KO.md`에서 실제 코드와 자산 상태를 확인한다.
5. `WORK_TIME_LOG_KO.md`에서 이전 작업 시간 기록과 기록 형식을 확인한다.
6. 시간이 줄어드는 구간은 병렬 수행으로 나눈 뒤 구현한다.
7. 작업 결과와 검증 결과를 다시 문서에 반영하고, 단계 시간 기록을 append 한다.

## 문서 갱신 규칙

- 현재 단계가 끝나면 `CURRENT_PHASE_KO.md`를 갱신한다.
- 새 제약이나 새 규칙이 생기면 `DECISION_LOG_KO.md`에 기록한다.
- 실제 코드, 설정, 애셋 상태가 달라지면 `STATE_SNAPSHOT_KO.md`를 갱신한다.
- 큰 방향이나 마일스톤이 바뀌면 `PROJECT_HARNESS_KO.md`를 갱신한다.
- 각 단계의 시작 시각, 종료 시각, 소요 시간, 병렬 수행 여부는 `WORK_TIME_LOG_KO.md`에 append 한다.

## 중요한 주의점

- `어느 파일을 읽어라`만 말해도 기본 방향은 잡힌다.
- 하지만 장기 작업에서는 `어디까지 진행할지`도 함께 말하는 편이 안정적이다.
- 가장 안정적인 형식은 `읽을 문서 + 이번 실행 경계 + 병렬 수행 허용 여부 + 작업 후 문서 갱신 요구`의 4요소를 같이 주는 방식이다.
