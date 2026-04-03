# TopDownTestOne 현재 단계

최종 수정일: 2026-04-04

## 현재 단계 식별자

- 단계 ID: `M1-P1`
- 단계명: 게임플레이 기반 클래스 골격 생성

## 직전 단계 완료 상태

- 직전 단계 `M0-P1` 헤드리스 애셋 제작 기반 정립은 완료됐다.
- `TopDownTestOneEditor` 에디터 모듈과 `TopDownTestOneHeadlessSetup` 커맨드렛이 추가됐다.
- `/Game/TopDownShooter/Materials/M_VoxBase`와 `SourceArt/Vox/SM_Vox_TestCube_01.vox`가 생성됐다.
- `TopDownTestOneEditor` Development Editor 빌드가 통과했다.
- `UnrealEditor-Cmd.exe ... -run=TopDownTestOneHeadlessSetup -unattended -nop4 -nosplash -nullrhi` 재실행이 무경고로 통과했다.

## 이번 단계 목표

헤드리스 제작 기반 위에 실제 게임플레이 기반 클래스를 얹을 최소 골격을 만든다.

이번 단계의 완료 기준은 아래와 같다.
- `GameMode`, `PlayerController`, `Character` C++ 베이스 클래스가 추가된다.
- 기본 스폰 흐름을 담을 최소 클래스 연결 구조가 마련된다.
- 향후 에디터 교체를 위해 Blueprint 파생 클래스를 기준으로 연결할 구조가 정해진다.
- 다음 단계에서 이동과 입력을 바로 올릴 수 있을 정도의 런타임 뼈대가 준비된다.

## 이번 단계 포함 범위

- `GameMode` C++ 베이스 클래스 추가
- `PlayerController` C++ 베이스 클래스 추가
- `Character` C++ 베이스 클래스 추가
- 기본 카메라 또는 카메라 컴포넌트 구조 설계
- Blueprint 파생 클래스 기준 연결 전략 확정
- 필요 시 기본 설정 파일의 최소 보강

## 이번 단계 제외 범위

- 실제 이동 입력 구현
- 조준 구현
- 발사 구현
- 적 AI 구현
- HUD 구현
- 웨이브 구현
- 게임 오버 구현

## 이번 단계 산출물

- 게임플레이 기반 클래스 골격
- Blueprint 친화적 클래스 연결 원칙의 실제 적용 준비
- 다음 단계 입력 및 이동 구현을 위한 최소 런타임 구조
- 단계 수행 시간 기록
- 단계 완료 후 갱신된 상태 문서

## 이번 단계 전제 조건

- `M0-P1` 검증이 완료되어 있어야 한다.
- 프로젝트는 `UE 5.7` 기준으로 에디터 타겟 빌드가 가능해야 한다.
- Unreal Editor GUI는 열지 않는다.
- Blueprint 생성이 필요하면 `UnrealEditor-Cmd` 또는 동등한 헤드리스 경로를 사용한다.

## 구현 메모

- 현재 단계 안에서 탐색, 구현, 검증을 병렬로 나누는 편이 더 빠르면 병렬 수행을 허용한다.
- 새로 생기는 클래스 연결 지점은 나중에 에디터에서 교체하기 쉽도록 Blueprint 파생 클래스 기준으로 마감한다.
- `GameMode`도 직접 C++ 클래스를 기본값으로 꽂는 대신 Blueprint 파생 클래스를 기본 연결 대상으로 둘 준비를 한다.
- 단계 시작과 종료 시각, 소요 시간, 병렬 수행 여부는 `WORK_TIME_LOG_KO.md`에 append 한다.
- `generator/evaluator` 분리 운영을 사용했다면 단계 완료 시 역할 분리 요약을 이 문서에 남기고, 실제 중계 내역은 `WORK_TIME_LOG_KO.md` 메모에 남긴다.

## 검증 체크리스트

- `GameMode`, `PlayerController`, `Character` 골격 클래스가 추가되었는가
- 에디터 타겟이 계속 빌드되는가
- 다음 단계 입력/이동 구현을 바로 얹을 수 있는가
- Blueprint 파생 연결 전략이 문서와 코드 기준에서 일치하는가

## 검증 결과

- 아직 미실행

## 여기서 멈출 기준

- 게임플레이 기반 클래스 골격이 준비된 시점까지
- 실제 이동, 입력, 조준 구현은 다음 단계로 넘긴다

## 다음 단계 후보

- `M1-P2` `EnhancedInput` 기반 이동 및 카메라 연결
- `M2-P1` 마우스 조준과 바라보기 회전

## 단계 완료 후 반드시 갱신할 항목

- 검증 결과
- 실제 변경 파일 목록
- 명령 실행 결과
- 새로 드러난 리스크
- 다음 단계 추천
- `generator/evaluator` 분리 운영 사용 시 역할 분리 요약
- `WORK_TIME_LOG_KO.md` 추가 기록
