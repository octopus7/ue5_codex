# CodexHarness 현재 단계

최종 수정일: 2026-04-04

## 현재 단계 식별자

- 단계 ID: `M6-P2`
- 단계명: VOX 닭 외형 식별 강화 완료

## 완료 상태 요약

- `Harness_T2` 작업 트리에 `Harness_T1`의 검증된 `CodexHarness` 구현 스냅샷을 선택 통합했다.
- 통합 직후 현재 하네스 기준과 비교해 비어 있던 `PlayerWeaponTrace` 전용 라인트레이스 채널과 플레이어 피격 카메라 흔들림 애셋/연결을 로컬에서 추가했다.
- 헤드리스 경로에서 플레이어용 흰 닭 VOX와 적 캐릭터용 붉은 닭 VOX를 각각 생성해 서로 다른 `StaticMesh`로 import하고 각 Blueprint 기본 메시로 연결했다.
- 현재 작업 트리 기준으로 빌드와 헤드리스 자동화 검증을 다시 통과시켰다.
- 활성 구현 단계는 없다.

## 최종 검증 기준

- `CodexHarnessEditor` 빌드 통과
- `CodexHarnessHeadlessSetup` 재실행 `0 error(s), 0 warning(s)` 통과
- 자동화 보고서에 플레이어/적 분리 `StaticMesh`, `EnemyCharacter`, `RestartAction`, `PlayerHitCameraShake`, `EffectsConfig` 반영 확인
- `PlayerWeaponTrace`(`ECC_GameTraceChannel1`) 채널 정의와 적 응답 경로 존재 확인
- 상태 스냅샷과 시간 로그 최신화 확인

## 검증 결과

- `Build.bat CodexHarnessEditor Win64 Development -Project='D:\github\ue5_codex\CodexHarness\CodexHarness.uproject' -WaitMutex -FromMsBuild -NoHotReloadFromIDE` 성공
- `UnrealEditor-Cmd.exe 'D:\github\ue5_codex\CodexHarness\CodexHarness.uproject' -run=CodexHarnessHeadlessSetup -unattended -nop4 -nosplash -nullrhi -NoHotReloadFromIDE` 성공
- `Saved/HeadlessSetup/CodexHarnessHeadlessSetupReport.txt`에 `SM_Vox_PlayerChicken_White`, `SM_Vox_EnemyChicken_Red`, `EnemyCharacter`, `RestartAction`, `PlayerHitCameraShake`, `EffectsConfig` 반영 확인
- `Content/CodexHarness/Blueprints/Enemies/BP_CodexHarnessEnemyCharacter.uasset` 존재 확인
- `Content/CodexHarness/Input/Actions/IA_Restart.uasset` 존재 확인
- `Content/CodexHarness/Effects/BP_CodexHarnessPlayerHitCameraShake.uasset` 존재 확인
- `Content/CodexHarness/Vox/SM_Vox_PlayerChicken_White.uasset` 존재 확인
- `Content/CodexHarness/Vox/SM_Vox_EnemyChicken_Red.uasset` 존재 확인
- `Config/DefaultEngine.ini`에 `PlayerWeaponTrace`(`ECC_GameTraceChannel1`) 정의 반영 확인

## 완료 판정 메모

- 아래 최초 완성 기준 항목이 모두 충족된다.
  - 플레이어 자동 스폰
  - 가시 플레이어 메시 연결
  - `WASD` 이동
  - 마우스 커서 기준 회전
  - 좌클릭 발사
  - 적 1종 스폰/추적/공격
  - 플레이어와 적의 체력/피격/사망
  - 플레이어 피격 반동, `Niagara System`, 카메라 흔들림
  - HUD 체력/웨이브/남은 적 수 표시
  - 게임오버와 재시작 경로
- 수동 플레이 테스트는 수행하지 않았다.
- 최종 검증은 빌드, 헤드리스 커맨드렛, 생성 애셋 존재, 자동화 보고서, 설정/코드 정합성 기준이다.

## 실제 생성/검증 경로

- 플레이어 VOX 메시: `/Game/CodexHarness/Vox/SM_Vox_PlayerChicken_White`
- 적 VOX 메시: `/Game/CodexHarness/Vox/SM_Vox_EnemyChicken_Red`
- 머터리얼: `/Game/CodexHarness/Materials/M_VoxBase`
- GameInstance BP: `/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessGameInstance`
- GameMode BP: `/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessGameMode`
- PlayerController BP: `/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessPlayerController`
- Character BP: `/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessCharacter`
- EnemyCharacter BP: `/Game/CodexHarness/Blueprints/Enemies/BP_CodexHarnessEnemyCharacter`
- HUD BP: `/Game/CodexHarness/Blueprints/Core/BP_CodexHarnessHUD`
- 입력 애셋: `/Game/CodexHarness/Input/Actions/IA_Move`, `/Game/CodexHarness/Input/Actions/IA_Look`, `/Game/CodexHarness/Input/Actions/IA_Fire`, `/Game/CodexHarness/Input/Actions/IA_Restart`
- 입력 DA: `/Game/CodexHarness/Input/Configs/DA_DefaultInputConfig`
- 피드백 애셋: `/Game/CodexHarness/Effects/NS_PlayerHitReaction`, `/Game/CodexHarness/Effects/BP_CodexHarnessPlayerHitCameraShake`, `/Game/CodexHarness/Effects/DA_DefaultEffectsConfig`
- 공격 판정 채널: `PlayerWeaponTrace`(`ECC_GameTraceChannel1`)

## 남은 리스크

- 수동 플레이 테스트를 수행하지 않았으므로 실제 입력 감각과 카메라 흔들림 강도는 런타임 체감 검증이 남아 있다.
- 적 추적은 내비게이션이 아닌 직접 추적 방식이어서 복잡한 지형에서는 확장 리스크가 있다.
- 플레이어/적 가시 메시 연결은 `DefaultVisualMesh -> VisualMeshComponent` 반영 패턴에 의존하므로 이후 BP 편집 시 이 경로를 깨지 않도록 유지해야 한다.

## 다음 단계 후보

- 없음. 후속 개선이 필요하면 새 범위를 정의해 별도 단계로 시작한다.

## 역할 분리 메모

- 읽기 전용 `evaluator` 컨텍스트를 사용해 `Harness_T1` 구현 범위, 검증 공백, 병합 충돌 범위를 사전 점검했다.
- 실제 통합, 누락 기능 보강, 빌드/커맨드렛 재검증, 문서 갱신은 메인 에이전트가 수행했다.
