# Skill Box System Design (Minimal Roulette Chest)

## 목표 및 제약
- 레벨 디자이너가 직접 배치하는 보상 상자만 지원한다. (런타임 드랍/스폰 없음)
- 플레이 중 상자를 획득하면 게임을 일시 정지하고 간단한 룰렛 연출 후 결과가 결정된다.
- 결과는 문자 `A/B/C/D/E` 중 하나이며, 플레이어가 `OK` 입력을 누르면 게임을 재개한다.
- 새로운 UMG 위젯 제작 없이 C++ HUD 드로우와 입력만으로 구성한다.

## 핵심 흐름
1. 플레이어가 상자(`APSVRewardChest`)의 오버랩 트리거에 진입하면 상호작용 없이 즉시 `HandleChestTriggered()` 실행.
2. `UPSVChestRouletteSubsystem`(또는 게임모드 소유 UObject)이 Roulette 시퀀스를 시작하고 `UGameplayStatics::SetGamePaused(true)`로 게임을 정지.
3. 정지 상태에서도 동작 가능한 `FTimerManager::SetTimer(Timer, ..., /*bLoop=*/true, Interval, /*FirstDelay=*/0.f, /*bPauseTimer=*/false)`를 이용해 HUD에 표시되는 문자 후보를 빠르게 변경하며 룰렛 효과 연출.
4. 2~3초 후 최종 문자(랜덤 A~E)를 확정하고, HUD에 “결과: X / OK를 누르세요” 문구를 표시.
5. 플레이어가 `ConfirmAction`(예: `Enter` 또는 `E`)을 입력하면 시퀀스를 종료, HUD 메시지를 지우고 `SetGamePaused(false)`로 게임을 재개.
6. 상자는 `bConsumed` 플래그를 세우고 Destroy하거나 Hidden 상태로 유지한다.

## 필요한 클래스/컴포넌트

### APSVRewardChest (AActor)
- **컴포넌트**: `UStaticMeshComponent`(상자), `USphereComponent`(Trigger)
- **주요 프로퍼티**
  - `bool bConsumed`
  - `TObjectPtr<USoundBase> ChestOpenSound` (선택)
  - `float RouletteDuration = 2.0f;`
- **주요 함수**
  - `NotifyActorBeginOverlap(AActor* OtherActor)` → 플레이어 체크 후 `TriggerChest(APSVPlayerCharacter*)`
  - `TriggerChest`에서 `UPSVChestRouletteSubsystem::StartRoulette()` 호출 및 `bConsumed` 처리

### UPSVChestRouletteSubsystem (UObject, GameInstanceSubsystem 권장)
- **멤버**
  - `TArray<FString> CandidateTexts = {TEXT("A"), TEXT("B"), TEXT("C"), TEXT("D"), TEXT("E")};`
  - `FTimerHandle RouletteTimerHandle;`
  - `float RouletteInterval = 0.1f;`
  - `float RouletteDuration = 2.0f;`
  - 현재 대상 HUD/Player 포인터
  - `FString CurrentDisplay`
  - 상태 플래그(`bIsRouletteActive`)
- **주요 함수**
  - `StartRoulette(APSVPlayerCharacter* Player, float InDuration)`
    - 게임 일시정지, HUD에 “룰렛 시작” 메시지 출력
    - `RouletteTimerHandle`로 주기적으로 `AdvanceRoulette()` 호출
    - 마지막에는 `FinalizeResult()`로 확정
  - `AdvanceRoulette()`
    - `CandidateTexts`에서 랜덤 선택 → `CurrentDisplay` 업데이트 → HUD Draw 요청
  - `FinalizeResult()`
    - `CurrentDisplay` 고정, HUD에 “결과: X / OK 입력(ConfirmAction)” 안내
    - `EnableConfirmInput()` 호출
  - `HandleConfirm()`
    - 입력 수신 시 HUD 정리, 타이머 청소, `SetGamePaused(false)`

### APSVPlayerCharacter 확장
- `ConfirmAction` 입력(`Enhanced Input`) 추가. 게임 정지 중에도 작동하도록 `UEnhancedInputComponent::BindAction` 시 `ETriggerEvent::Triggered` 사용 + `bTriggerWhenPaused = true` 옵션.
- 상자 오버랩 시 Subsystem에 자신 포인터를 전달.

### APSVHUD 확장
- `ShowRouletteMessage(const FString& Text)` 및 `ClearRouletteMessage()` 추가.
- Canvas 드로우(예: `DrawText`) 또는 기존 알림 방식을 재사용.
- 룰렛 진행 중에는 `DrawHUD()`에서 `CurrentDisplay`를 중앙에 큰 폰트로 출력하며, 최종 결과 시 “OK 입력” 안내 문구를 함께 표기.

## 실행 시나리오
1. 플레이어가 상자에 닿음 → 상자에서 Subsystem 호출.
2. Subsystem이 게임을 일시정지하고 HUD에 “룰렛 중…” 표시 후 0.1초 간격으로 문자 업데이트.
3. 2초 경과 후 최종 문자 확정 → HUD에 “결과: C / OK를 누르세요” 표기.
4. 플레이어가 `ConfirmAction` 입력 → Subsystem이 HUD 정리, 게임 재개, 상자 정리 완료.

## 간단 체크리스트
- [ ] 상자 액터에 Trigger 콜리전과 `bConsumed` 처리 추가
- [ ] GameInstanceSubsystem으로 Roulette 제어 구현
- [ ] Pause 중에도 작동하는 Timer / Input 설정 확인
- [ ] HUD Draw 테스트 (Canvas 텍스트 사이즈 조정)
- [ ] 다중 상자 대응: 이미 룰렛 중이면 추가 트리거 무시 또는 큐잉
