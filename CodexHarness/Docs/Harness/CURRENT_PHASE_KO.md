# 현재 프로젝트 단계

최종 수정일: 2026-04-04

## 현재 단계 식별자

- 단계 ID: `M1-P1`
- 단계명: 게임플레이 기반 클래스와 실체 BP 연결 생성

## 직전 단계 완료 상태

- 직전 단계 `M0-P1` 헤드리스 애셋 제작 기반 정립은 완료됐다.
- `<PROJECT_EDITOR_MODULE>` 에디터 모듈과 `<PROJECT_HEADLESS_SETUP_COMMANDLET>` 커맨드렛이 추가됐다.
- `/Game/<PROJECT_CONTENT_ROOT>/Materials/M_VoxBase`와 `SourceArt/Vox/SM_Vox_TestCube_01.vox`가 생성됐다.
- `<PROJECT_EDITOR_MODULE>` Development Editor 빌드가 통과했다.
- `UnrealEditor-Cmd.exe ... -run=<PROJECT_HEADLESS_SETUP_COMMANDLET> -unattended -nop4 -nosplash -nullrhi` 재실행이 무경고로 통과했다.

## 이번 단계 목표

헤드리스 제작 기반 위에 실제 게임플레이 기반 클래스와 실체 Blueprint 연결, 가시 플레이어 애셋을 얹을 최소 런타임 기반을 만든다.

이번 단계의 완료 기준은 아래와 같다.
- `GameMode`, `PlayerController`, `Character` C++ 베이스 클래스가 추가된다.
- 커맨드렛이 `BP_*` GameMode, PlayerController, Character 애셋을 실제로 생성 또는 갱신한다.
- 프로젝트 또는 맵의 기본 연결 지점은 구체적인 Blueprint 애셋을 가리키며 직접 C++ 클래스를 가리키지 않는다.
- 기본 스폰 흐름을 담을 최소 클래스 연결 구조가 실제 애셋 기준으로 마련된다.
- 플레이어는 VOX 기반으로 import 된 `StaticMesh` 또는 동등한 실제 렌더링 애셋을 통해 화면에서 보인다.
- `BP_*Character`는 내부 메시 컴포넌트 또는 동등한 렌더링 컴포넌트에 프로젝트 내 생성된 메시 애셋을 실제 할당한다.
- `IA_*`, `IMC_*`, `DA_*InputConfig`가 실제 애셋으로 생성 또는 갱신된다.
- 플레이어 런타임 클래스는 `IA_*`나 `IMC_*`를 직접 참조하지 않고 `DA_*InputConfig`만 참조한다.
- 다음 단계에서 이동과 입력을 바로 올릴 수 있을 정도의 가시 런타임 뼈대가 준비된다.

## 이번 단계 포함 범위

- `GameMode` C++ 베이스 클래스 추가
- `PlayerController` C++ 베이스 클래스 추가
- `Character` C++ 베이스 클래스 추가
- 커맨드렛 기반 Blueprint GameMode, PlayerController, Character 생성 또는 갱신
- Blueprint GameMode의 `DefaultPawnClass`, `PlayerControllerClass` 등 기본 연결 완성
- 기본 카메라 또는 카메라 컴포넌트 구조 설계
- VOX 소스에서 플레이어 가시 표현용 `StaticMesh` import 경로 마련 및 실제 애셋 연결
- 캐릭터 Blueprint의 메시 컴포넌트에 import 된 메시 애셋 실제 할당
- 커맨드렛 기반 `IA_*`, `IMC_*`, `DA_*InputConfig` 입력 애셋 생성 또는 갱신
- `DA_*InputConfig`에 `IA_*`, `IMC_*` 참조 집계 및 플레이어 연결
- Blueprint 파생 클래스 기준 연결 전략의 실제 적용
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
- 커맨드렛으로 생성 또는 갱신되는 구체적인 Blueprint 연결 애셋
- 플레이어 가시 표현용 VOX `StaticMesh` import 및 메시 연결 구조
- 메시 컴포넌트에 프로젝트 메시 애셋이 실제 할당된 캐릭터 Blueprint
- 실제 입력 애셋인 `IA_*`, `IMC_*`, `DA_*InputConfig`
- 플레이어가 `DA_*InputConfig`만 참조하는 입력 연결 구조
- 다음 단계 입력 및 이동 구현을 위한 최소 가시 런타임 구조
- 단계 수행 시간 기록
- 단계 완료 후 갱신된 상태 문서

## 이번 단계 전제 조건

- `M0-P1` 검증이 완료되어 있어야 한다.
- 프로젝트는 `UE 5.7` 기준으로 에디터 타겟 빌드가 가능해야 한다.
- Unreal Editor GUI는 열지 않는다.
- Blueprint 생성과 VOX `StaticMesh` import가 필요하면 `UnrealEditor-Cmd` 또는 동등한 헤드리스 경로를 사용한다.

## 구현 메모

- 현재 단계 안에서 탐색, 구현, 검증을 병렬로 나누는 편이 더 빠르면 병렬 수행을 허용한다.
- 새로 생기는 클래스 연결 지점은 나중에 에디터에서 교체하기 쉽도록 Blueprint 파생 클래스 기준으로 마감한다.
- `GameMode`, `DefaultPawnClass`, `PlayerControllerClass` 등 기본 연결 지점에는 직접 C++ 클래스를 꽂지 않는다.
- 필요한 Blueprint 애셋과 기본값 연결은 커맨드렛 실행 결과로 실제 생성 또는 갱신되어야 한다.
- 플레이어는 보이는 런타임 오브젝트여야 하므로 VOX import 결과 또는 동등한 실제 메시 애셋이 연결되어야 한다.
- 캐릭터 Blueprint는 메시 컴포넌트가 비어 있으면 안 되며, 프로젝트에 생성된 메시 애셋을 실제 참조해야 한다.
- `IA_*`, `IMC_*`, `DA_*InputConfig`는 이름만 준비하는 것이 아니라 실제 프로젝트 애셋으로 생성되어야 한다.
- 플레이어 런타임 클래스는 `IA_*`와 `IMC_*`를 직접 물지 않고 `DA_*InputConfig` 하나를 통해 입력 구성을 받는다.
- 단계 시작과 종료 시각, 소요 시간, 병렬 수행 여부는 `WORK_TIME_LOG_KO.md`에 append 한다.
- `generator/evaluator` 분리 운영을 사용했다면 단계 완료 시 역할 분리 요약을 이 문서에 남기고, 실제 중계 내역은 `WORK_TIME_LOG_KO.md` 메모에 남긴다.

## 검증 체크리스트

- `GameMode`, `PlayerController`, `Character` 골격 클래스가 추가되었는가
- 커맨드렛이 Blueprint GameMode, PlayerController, Character 애셋을 실제 생성 또는 갱신했는가
- `GameMode`, `DefaultPawnClass`, `PlayerControllerClass`가 구체적인 Blueprint 애셋을 가리키고 직접 C++ 클래스를 가리키지 않는가
- 플레이어가 사용할 VOX 기반 `StaticMesh` 또는 동등한 실제 렌더링 애셋이 import 및 연결되었는가
- 캐릭터 Blueprint의 메시 컴포넌트가 그 프로젝트 메시 애셋을 실제 참조하는가
- `IA_*`, `IMC_*`, `DA_*InputConfig` 애셋이 실제 생성 또는 갱신되었는가
- `DA_*InputConfig`가 필요한 `IA_*`, `IMC_*` 참조를 실제로 보유하는가
- 플레이어 런타임 클래스가 `IA_*`/`IMC_*` 대신 `DA_*InputConfig`를 참조하는가
- 플레이어가 화면에서 보이는 상태인가
- 에디터 타겟이 계속 빌드되는가
- 다음 단계 입력/이동 구현을 바로 얹을 수 있는가
- Blueprint 파생 연결 전략이 문서와 코드 기준에서 일치하는가

## 검증 결과

- 아직 미실행

## 여기서 멈출 기준

- 게임플레이 기반 클래스, 구체적인 Blueprint 연결, 가시 플레이어 애셋이 준비된 시점까지
- 실제 이동, 입력, 조준 구현은 다음 단계로 넘긴다

## 다음 단계 후보

- `M1-P2` `EnhancedInput` 기반 이동 및 카메라 연결
- `M2-P1` 마우스 조준과 바라보기 회전

## 단계 완료 후 반드시 갱신할 항목

- 검증 결과
- 실제 변경 파일 목록
- 명령 실행 결과
- 실제 생성 또는 갱신된 Blueprint 애셋 경로
- 실제 import 된 VOX `StaticMesh` 애셋 경로
- 캐릭터 Blueprint 내부 메시 컴포넌트에 연결된 메시 애셋 경로
- 실제 생성 또는 갱신된 `IA_*`, `IMC_*`, `DA_*InputConfig` 애셋 경로
- 플레이어에 연결된 `DA_*InputConfig` 애셋 경로
- 새로 드러난 리스크
- 다음 단계 추천
- `generator/evaluator` 분리 운영 사용 시 역할 분리 요약
- `WORK_TIME_LOG_KO.md` 추가 기록
