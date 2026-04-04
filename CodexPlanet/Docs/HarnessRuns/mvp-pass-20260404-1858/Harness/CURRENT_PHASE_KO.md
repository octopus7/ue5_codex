# CodexPlanet 현재 단계

최종 수정일: 2026-04-04

## 현재 단계 식별자

- 단계 ID: `M7-P0`
- 단계명: 정리와 최종 정합성 점검

## 선행 정리 상태

- `M1-P0`, `M1-P1`, `M1-P2`, `M1-P3`, `M1-P4`, `M6-P0`가 완료됐다.
- `CodexPlanetGameMode`, `CodexPlanetPlayerController`, `CodexPlanetViewPawn`, `CodexPlanetActor`, `UNoisyPlanetMeshComponent`, `ICodexPlacementSurface`가 추가됐다.
- `BasicMap`은 유지하고 `GlobalDefaultGameMode`로 기본 진입 시 행성 뷰 골격이 뜨도록 연결했다.
- `PlanetActor`는 코드 생성된 약간의 노이즈 스피어를 초기 행성 표현으로 가진다.
- `PlayerController`는 코드 생성 `EnhancedInput` 매핑을 통해 좌클릭 드래그와 트랙볼 회전을 처리한다.
- `PlayerController`는 `Tab` 배치 모드, 좌클릭 확정, 우클릭 취소, 커서 기반 프리뷰를 처리한다.
- `CodexPlaceablePropActor`가 행성과 같은 표면 계약을 구현해 적층 가능한 기본 프랍 역할을 한다.
- 5종 프랍 source가 `SourceArt/Vox/Props` 아래에 `.vox`와 `.obj`로 생성됐다.
- 5종 static mesh 애셋이 `/Game/Props/Vox`에 import됐고 카탈로그로 로드된다.
- `/Game/UI/WBP_PlanetOrbitControls` 실존 `WBP` asset이 생성됐고 widget tree가 저장돼 있다.
- runtime에서 `WBP_PlanetOrbitControls`가 viewport에 올라오고 `FooterText`로 선택 프랍/남은 슬롯을 표시한다.
- 최소 규칙은 `남은 슬롯 수` 카운트이며, 슬롯이 0이면 배치가 막힌다.
- 헤드리스 검증에서 `/Game/Maps/BasicMap`이 `CodexPlanetGameMode`로 로드되고 trimesh 경고 없이 시작됨을 확인했다.
- 자동화 테스트 `CodexPlanet.M1P1.TrackballRotationMath`가 성공했다.
- 자동화 테스트 `CodexPlanet.M1P2.SurfaceAlignmentMath`가 성공했다.
- 자동화 테스트 `CodexPlanet.M1P3.PropCatalogAssets`가 성공했다.
- 자동화 테스트 `CodexPlanet.M1P4.OrbitControlsWidgetAsset`가 성공했다.
- headless map load에서 orbit widget 생성과 HUD 상태 로그를 확인했다.

## 이번 단계 목표

현재 상태를 문서, 코드, asset, 검증 결과 기준으로 최종 정리하고 run 종료 기준을 만족하는지 확인한다.

이번 단계의 완료 기준은 아래와 같다.
- live 문서와 실제 프로젝트 상태가 일치한다.
- 핵심 검증 결과가 문서에 반영된다.
- run 종료에 필요한 아카이브 여부가 결정된다.
- 다음 확장 phase가 필요하면 최소한의 추천만 남긴다.

## 이번 단계 포함 범위

- 문서 정합성 점검
- 검증 결과 정리
- 아카이브 실행 여부 판단 및 수행
- 종료 메모 정리

## 이번 단계 제외 범위

- 신규 기능 확장
- 복잡한 경제 수치
- 실제 고리 NS/위성 런타임 동작

## 이번 단계 산출물

- 최신 live 문서
- 검증 결과 요약
- 필요 시 Harness archive

## 이번 단계 전제 조건

- `M1-P0`부터 `M6-P0`까지의 MVP 경로는 이미 추가돼 있다.
- 플레이어 캐릭터보다 `행성 조작 시점`이 우선이다.
- 이후 배치 규칙은 적층 가능, 표면 삼각형 노멀 정렬을 기본값으로 사용한다.
- 이후 행성 `고리 NS`/`위성` 추가 UI는 런타임 조립형이 아니라 에디터에서 확인 가능한 실존 `WBP` 애셋으로 구현한다.

## 구현 메모

- 이 단계에서는 새 기능을 늘리기보다 현재 상태를 정확히 고정한다.
- 수동 플레이 감각 튜닝은 다음 확장 phase로 넘길 수 있다.

## 검증 체크리스트

- 문서와 실제 상태가 일치하는가
- 핵심 빌드/테스트/헤드리스 검증 결과가 정리됐는가
- archive가 필요하면 생성됐는가
- 문서와 코드/애셋 구조 설명이 일치하는가

## 검증 결과

- 아직 미실행

## 여기서 멈출 기준

- live 문서, 검증, archive 정리가 끝난 시점까지

## 다음 단계 후보

- 배치 손맛/스케일 튜닝
- orbit controls 버튼의 실제 런타임 기능 연결
- 경영 규칙 확장

## 단계 완료 후 반드시 갱신할 항목

- 최종 검증 결과
- archive 실행 여부
- 다음 단계 추천
- 다음 단계 추천
- `WORK_TIME_LOG_KO.md` 추가 기록
