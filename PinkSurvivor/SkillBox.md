# Skill Box System Design (UE5.5)

## 목표
- 맵 곳곳에서 획득 가능한 보상 상자를 통해 뱀파이어 서바이버스의 랜덤 상자 경험을 3D 환경에서 재현한다.
- UI는 현행 HUD 파이프라인과 연동하되, 새로운 UMG 위젯을 제작하지 않고 C++과 DataAsset 위주로 구성한다.
- 밸런스 조정과 보상 확장을 고려한 모듈식 구조를 마련한다.

## 시스템 개요
1. **월드 스폰**
   - `APSVRewardChest` 액터로 상자를 구현하고, 기존 적 처치/웨이브 시스템과 연동된 드랍 또는 레벨 배치로 등장시킨다.
   - 스폰 트리거 예시: 엘리트 처치, 특정 타이머, 누적 킬 수 등. 스폰 서브시스템에 훅을 추가한다.
   - 상자 내부 보상은 `UPSVRewardTable` Data Asset에서 로드한다.

2. **상호작용 흐름**
   - 플레이어가 트리거 범위에 들어오면 상호작용 가능 상태로 전환하고, HUD에 텍스트 알림/사운드를 출력한다.
   - 상호작용 키(`InteractAction`) 입력 시 상자 개방 시퀀스를 실행한다. UMG 없이 메시 출력은 `APSVHUD`의 C++ 드로우/사운드 함수를 활용한다.
   - `bAutoOpenOnOverlap` 옵션을 두어 접촉 즉시 개봉하는 모드도 지원한다.

3. **보상 롤링 구조**
   - `UPSVRewardTable`은 `FPSVRewardEntry` 배열을 보유하며, 각 엔트리는 가중치, 보상 타입(`EPSVRewardType`), 수량, 페이로드 식별자를 포함한다.
   - 상자 개봉 시 `UPSVRewardResolver`가 Data Asset을 참조하여 누적 가중치 방식으로 하나 이상의 보상을 추출한다.
   - 보상 결과는 `FPSVResolvedReward` 구조체로 반환하고, HUD/로그/사운드 처리를 위해 이벤트를 브로드캐스트한다.

4. **보상 적용 절차**
   - 보상 타입에 따라 대상 시스템을 호출한다.
     - 무기/스킬 업그레이드 → `UPSVWeaponManager`(가정) 혹은 플레이어 컴포넌트.
     - 영구 골드 → `UPSVGameInstance`를 통해 저장.
     - 즉시 회복/버프 → `UPSVHealthComponent`, `UPSVExperienceComponent` 등에 직접 적용.
   - 적용 완료 후 HUD에 `ShowRewardAnnouncement`(신규 함수)로 텍스트/사운드를 출력한다.

## 핵심 클래스 설계

### APSVRewardChest (AActor)
- **컴포넌트**: `UStaticMeshComponent` 또는 `USkeletalMeshComponent`, `USphereComponent`(상호작용 트리거), 필요 시 `UNiagaraComponent`.
- **주요 프로퍼티**
  - `TObjectPtr<UPSVRewardTable> RewardTable`
  - `bool bConsumed`, `bool bAutoOpenOnOverlap`
  - `float InteractionRadius`
  - 개방/대기 사운드, 파티클 레퍼런스
- **주요 함수**
  - `BeginPlay()`에서 트리거 이벤트 설정
  - `NotifyActorBeginOverlap()` → 플레이어 확인 후 자동개봉 또는 프롬프트 출력
  - `HandleInteract(APSVPlayerCharacter* Player)` → 보상 계산, 애니메이션 트리거, Destroy/Reuse 처리

### UPSVRewardTable (PrimaryDataAsset)
- `TArray<FPSVRewardEntry> Entries`
- `FPSVRewardEntry`
  - `float Weight`
  - `EPSVRewardType RewardType`
  - `FName PayloadId`
  - `int32 Quantity`
- 테이블 단위로 최소 드랍 수, 중복 허용 여부, 보너스 조건 등을 옵션화할 수 있다.

### UPSVRewardResolver (UObject)
- `ResolveReward(const UPSVRewardTable* Table, FPSVResolvedReward& OutReward)`
- 내부에서 누적 가중치 테이블을 캐싱하고, `FRandomStream` 사용으로 재현성 확보.
- 여러 개의 보상을 연속 지급해야 할 경우(예: 전설 상자) 반복 호출 및 중복 체크 로직 포함.

### APSVPlayerCharacter 확장
- Enhanced Input에 `InteractAction` 추가.
- Tick 또는 라인트레이스로 가장 가까운 `APSVRewardChest`를 탐색하고 상호작용 UI를 제어.
- 보상 수령 후 HUD 알림을 위한 델리게이트를 브로드캐스트.

### APSVHUD 확장
- `ShowRewardAnnouncement(const FPSVResolvedReward&)` 함수 추가.
- Canvas 기반 텍스트 드로우 또는 기존 배너 시스템 재활용으로 UMG 없는 연출 제공.
- 보상 등급에 따라 사운드/컬러를 다르게 설정.

## 스폰 및 드랍 연동
- `APSVEnemyCharacter::HandleDeath()`에 상자 드랍 확률 로직을 추가하거나, 별도의 `APSVRewardSpawnManager`로 위임.
- 레벨 디자인 시에는 `APSVRewardChest` 액터를 직접 배치하고, `RewardTable`과 스폰 조건을 개별 설정.
- 웨이브 진행 HUD와 연동해 특정 라운드 종료 시 보상 상자가 맵 중앙에 스폰되는 연출 가능.

## 개발 단계 제안
1. Data Asset 구조(`UPSVRewardTable`, `FPSVRewardEntry`) 및 Resolver 구현.
2. `APSVRewardChest` 액터 제작 및 상호작용 흐름 구축.
3. 플레이어 입력/트레이스 로직 추가 및 HUD 알림 확장.
4. 사운드, 파티클, 애니메이션 트리거 연결.
5. 드랍 조건을 기존 웨이브/엘리트 시스템과 연동.
6. QA: 확률 분포, 멀티 상자 동시 처리, 세이브/로드 호환성 점검.

## 테스트 시나리오
- 100회 이상 상자를 열어 확률 분포가 설정과 일치하는지 검증.
- `bAutoOpenOnOverlap` ON/OFF 상황 비교.
- 플레이어 사망/재시작 후 상자 상태 유지 여부 확인.
- 멀티 웨이브와 엘리트 연속 처치 상황에서 상자 중복 스폰, 동시 개봉 처리 확인.
