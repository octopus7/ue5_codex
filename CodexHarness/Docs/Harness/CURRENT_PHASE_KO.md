# CodexHarness 현재 단계

최종 수정일: 2026-04-04

## 현재 단계 식별자

- 단계 ID: `M5-P2`
- 단계명: 게임 오버와 재시작 흐름

## 이번 단계 목표

플레이어 사망 시 게임 오버 상태를 확정하고, 화면 표시와 재시작 입력을 연결해 다시 시작 가능한 흐름을 완성한다.

이번 단계의 완료 기준은 아래와 같다.
- 플레이어 사망 시 게임 오버 상태가 확정된다.
- HUD에 게임 오버 문구와 재시작 안내가 표시된다.
- 재시작 입력 경로가 존재한다.
- 헤드리스 스모크 테스트에서 재시작 후 플레이어가 다시 스폰되는 것이 검증된다.

## 이번 단계 포함 범위

- 게임 오버 상태 플래그
- 재시작 입력 경로
- 레벨 재로드 기반 재시작
- 재시작 스모크 검증 경로

## 이번 단계 제외 범위

- 추가 UI 폴리시
- 사운드와 시각 연출 보강

## 이번 단계 산출물

- 게임 오버 상태 로직
- 재시작 입력 및 실행 경로
- 재시작 스모크 테스트 경로
- 단계 완료 후 갱신된 상태 문서

## 이번 단계 전제 조건

- `M5-P1` HUD와 게임 상태 표시가 완료되어 있어야 한다.
- 프로젝트는 `UE 5.7` 기준으로 게임/에디터 타겟 빌드가 가능해야 한다.
- Unreal Editor GUI는 열지 않는다.

## 구현 메모

- 게임 오버 상태는 게임 모드가 들고, HUD는 그 상태를 읽어 표시한다.
- 재시작은 우선 레벨 재로드 기반으로 구현한다.
- 헤드리스 검증은 자동 치명 피해와 재시작 요청을 묶은 스모크 경로로 수행한다.

## 검증 체크리스트

- 게임 타겟과 에디터 타겟이 빌드되는가
- 플레이어 사망 시 게임 오버 상태가 확정되는가
- 게임 오버 HUD 문구를 그릴 수 있는가
- 재시작 입력 경로가 동작하는가
- 헤드리스 스모크 테스트로 재스폰을 검증할 수 있는가

## 검증 결과

- 상태: 완료
- 실제 변경 파일:
  - `Source/CodexHarness/Public/Game/CHTopDownGameMode.h`
  - `Source/CodexHarness/Private/Game/CHTopDownGameMode.cpp`
  - `Source/CodexHarness/Public/Player/CHPlayerController.h`
  - `Source/CodexHarness/Private/Player/CHPlayerController.cpp`
  - `Source/CodexHarness/Private/UI/CHGameHUD.cpp`
- 빌드 검증:
  - `Build.bat CodexHarnessEditor Win64 Development d:\github\ue5_codex\CodexHarness\CodexHarness.uproject -NoHotReloadFromIDE -WaitMutex` 성공
  - `Build.bat CodexHarness Win64 Development d:\github\ue5_codex\CodexHarness\CodexHarness.uproject -NoHotReloadFromIDE -WaitMutex` 성공
- 런타임 검증:
  - `UnrealEditor-Cmd.exe d:\github\ue5_codex\CodexHarness\CodexHarness.uproject /Game/Maps/BasicMap -game -nullrhi -unattended -nop4 -nosplash -CHRestartSmoke -AbsLog=d:\github\ue5_codex\CodexHarness\Saved\Logs\M5_P2_RestartSmoke.log` 성공
  - 로그 확인: `LogCHTopDownGameMode: Display: CHGameOverState=1`
  - 로그 확인: `LogCHPlayerController: Display: CHRestartSmokeRequest`
  - 로그 확인: `LogCHPlayerController: Display: CHRestartSmokeRespawned Health=100.00 Wave=1 Alive=1 GameOver=0`
- 장기 목표 판정:
  - `PROJECT_HARNESS_KO.md`의 최초 완성 기준인 자동 스폰, `WASD` 이동, 마우스 조준 회전, 기본 발사, 적 1종 추적/공격, 체력/사망, HUD, 게임 오버/재시작 흐름이 현재 코드와 스모크 기준으로 충족되었다.
- 남은 선택 작업:
  - 이동 충돌 스윕과 감각 보정
  - HUD 시각 polish와 연출 보강
  - VOX 메시 실제 변환 파이프라인 마무리

## 여기서 멈출 기준

- 게임 오버와 재시작 흐름이 검증된 시점까지
- 장기 목표 달성 후에는 `M6` 정리 작업만 남긴다

## 다음 단계 후보

- `M6-P1` 폴리시와 검증 정리
- `M6-P2` 선택적 구조 정리와 연출 보강

## 직전 완료 단계 요약

- 완료 단계: `M5-P1`
- 실제 변경 파일:
  - `Source/CodexHarness/Public/UI/CHGameHUD.h`
  - `Source/CodexHarness/Private/UI/CHGameHUD.cpp`
  - `Source/CodexHarness/Private/Game/CHTopDownGameMode.cpp`
- 빌드 검증:
  - `Build.bat CodexHarnessEditor Win64 Development d:\github\ue5_codex\CodexHarness\CodexHarness.uproject -NoHotReloadFromIDE -WaitMutex` 성공
  - `Build.bat CodexHarness Win64 Development d:\github\ue5_codex\CodexHarness\CodexHarness.uproject -NoHotReloadFromIDE -WaitMutex` 성공
- 런타임 검증:
  - `UnrealEditor-Cmd.exe d:\github\ue5_codex\CodexHarness\CodexHarness.uproject /Game/Maps/BasicMap -game -nullrhi -unattended -nop4 -nosplash -CHHudSmoke -CHDamageSmoke -AbsLog=d:\github\ue5_codex\CodexHarness\Saved\Logs\M5_P1_HudSmoke.log` 성공
  - 로그 확인: `LogCHPlayerController: Display: CHDamageSmokeStep=1 Applied=40.00 Health=60.00 Dead=0 DeathCount=0`
  - 로그 확인: `LogCHGameHUD: Display: CHHudValues Health=60.00 MaxHealth=100.00 Wave=1 Alive=1`
- 새로 드러난 리스크:
  - 게임 오버와 재시작 입력 흐름은 아직 구현 전이었다.
  - 이동은 여전히 직접 월드 오프셋 방식이라 충돌 스윕과 감각 보정은 남아 있다.

## 단계 완료 후 반드시 갱신할 항목

- 검증 결과
- 실제 변경 파일 목록
- 명령 실행 결과
- 새로 드러난 리스크
- 다음 단계 추천
