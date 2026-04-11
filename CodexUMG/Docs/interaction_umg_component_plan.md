# 상호작용 UMG 컴포넌트 구현 계획

## 문서 목적
- 플레이어가 대상에 가까워질수록 상호작용 가능 여부를 월드 대상에 붙는 screen-space UMG로 표시하는 컴포넌트 구조를 정의한다.
- 실제 `Widget Blueprint` 애셋과 원형 마커용 텍스처 애셋은 에디터 모듈 코드로 생성하고, 생성 실행은 `Commandlet`로 처리하는 것을 고정 원칙으로 둔다.
- 상호작용 종류는 컴포넌트에서 선택 가능하게 두되, 실제 액션 실행 진입은 전역 서브시스템으로만 전달한다.
- 테스트용 사과/딸기 Blueprint까지 포함해, 바로 검증 가능한 최소 수직 슬라이스를 계획한다.
- 리드타임 단축을 위해 병렬 처리 가능한 작업을 에이전트 단위로 나누는 운영 지침도 함께 정의한다.

## 목표 상태
1. 월드 오브젝트에 `UCodexInteractionComponent`를 붙이면 플레이어와의 거리 기반으로 screen-space UMG 마커가 나타난다.
2. 위젯 상태는 `보이지 않음`, `가시거리`, `상호작용가능상태` 3가지만 사용한다.
3. `보이지 않음 -> 가시거리` 전환 시 채워진 원과 동심원 이미지가 페이드 인되고, 동심원은 더 크고 더 투명한 상태에서 시작해 채워진 원을 감싸는 크기까지 줄어든다.
4. `가시거리 -> 보이지 않음` 전환 시 위 애니메이션이 정확히 역방향으로 재생된다.
5. `상호작용가능상태`가 되면 원 마커 오른쪽에 흰 배경 박스와 검은 텍스트로 상호작용 문구가 표시된다.
6. 실제로 가장 가까운 대상 하나만 `상호작용가능상태`를 가질 수 있으므로, 흰 박스 + 검은 텍스트 프롬프트는 항상 하나만 보인다.
7. 상호작용 입력이 발생하면 컴포넌트는 직접 처리하지 않고 전역 서브시스템에 요청만 전달한다.
8. 전역 서브시스템은 상호작용 요청, 상호작용 종료, 후보 없음 같은 중요한 이벤트만 스크린 디버그 스트링으로 출력하고, 포커스 시작/종료는 로그로만 남긴다.
9. 테스트용 `BP_Interactable_Apple`, `BP_Interactable_Strawberry`가 실제로 존재하고, 둘 다 `먹기` 상호작용으로 동작한다.
10. 관련 텍스처, 위젯 애셋, 테스트 Blueprint 애셋은 모두 에디터 모듈 + 커맨드렛 경로로 생성 가능해야 한다.

## 현재 프로젝트 기준 접점
- 런타임 입력 접근은 이미 [Source/CodexUMG/Public/CodexTopDownInputConfigDataAsset.h](../Source/CodexUMG/Public/CodexTopDownInputConfigDataAsset.h), [Source/CodexUMG/Private/CodexTopDownPlayerController.cpp](../Source/CodexUMG/Private/CodexTopDownPlayerController.cpp), [Source/CodexUMG/Public/CodexGameInstance.h](../Source/CodexUMG/Public/CodexGameInstance.h) 경로를 통해 정리되어 있다.
- 에디터 모듈 기반 애셋 생성 패턴은 [Source/CodexUMGBootstrapEditor/Private/CodexUMGBootstrapEditorModule.cpp](../Source/CodexUMGBootstrapEditor/Private/CodexUMGBootstrapEditorModule.cpp)에 이미 존재한다.
- 커맨드렛 실행 패턴은 [Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexVoxAssetBuildCommandlet.cpp](../Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexVoxAssetBuildCommandlet.cpp) 구현을 기준으로 맞추면 된다.
- 테스트용 음식 메시로는 이미 존재하는 `/Game/Vox/Meshes/Food/SM_Vox_Apple`, `/Game/Vox/Meshes/Food/SM_Vox_Strawberry`를 재사용할 수 있다.

## 핵심 원칙
- 상호작용 컴포넌트는 거리/표시용 메타데이터를 보관하고, 서브시스템에 등록되며, 상태 반영만 수행한다.
- 가장 가까운 대상 판정은 각 컴포넌트가 따로 하지 않고 전역 서브시스템이 중앙에서 수행한다.
- 상호작용 프롬프트는 서브시스템이 선택한 단 하나의 대상에만 열린다.
- 실제 `Widget Blueprint` 애셋과 원형 마커용 `Texture2D` 애셋은 반드시 프로젝트 `Content` 아래에 생성되어 존재해야 하며, 수동 생성이 아니라 에디터 코드 + 커맨드렛으로 만들어진다.
- 채워진 원과 링용 동심원은 외부 소스 이미지를 가져오지 않고 에디터 모듈 코드가 픽셀 데이터를 직접 만들어 생성한다.
- 두 텍스처는 모두 흰색 RGB를 사용하고, 형상은 알파 채널로만 정의한다.
- 입력 확장은 반드시 기존 `Enhanced Input` 구현 유무를 먼저 확인한 뒤, 이미 존재하는 구조를 그대로 따라가야 한다.
- 기존 `Enhanced Input` 구현이 없다면 상호작용 기능 구현을 즉시 멈추고, 사용자에게 최소 `Enhanced Input` 기반을 먼저 구현해 달라고 요구한다.
- 위젯 상태 전환 애니메이션은 커맨드렛에서 복잡한 `WidgetAnimation` 트랙을 조립하기보다, C++ 베이스 위젯에서 스케일/투명도 보간으로 처리하는 방식을 우선 채택한다.
- 상호작용 타입은 컴포넌트의 `enum` 값으로 선택하지만, 컴포넌트가 직접 `먹기` 같은 게임플레이를 실행하지 않는다.
- 1차 구현의 전역 수신자는 월드 단위 상태 판정과 틱이 필요하므로 `UWorldSubsystem`을 우선 채택한다.
- 설정성 데이터와 입력 자산 참조는 기존 `GameInstance + DataAsset` 패턴을 최대한 유지한다.

## 산출물

### 런타임 C++ 산출물
- `ECodexInteractionType`
  - 예: `Eat`, `Pickup`, `Talk`, `Use`
- `ECodexInteractionWidgetState`
  - `Hidden`
  - `VisibleRange`
  - `Interactable`
- `FCodexInteractionRequest`
  - 요청자, 대상 컴포넌트, 상호작용 타입, 표시 문구를 묶는 최소 구조체
- `UCodexInteractionComponent`
  - 월드 대상에 부착되는 핵심 컴포넌트
- `UCodexInteractionIndicatorWidget`
  - 상태 반영과 보간 애니메이션을 담당하는 `UUserWidget` 베이스 클래스
- `UCodexInteractionSubsystem`
  - 등록된 후보를 추적하고 가장 가까운 대상을 선택하는 전역 서브시스템
- `UCodexInteractionTarget` 인터페이스
  - 테스트용 사과/딸기가 상호작용 요청을 수신하기 위한 최소 훅

### 에디터 / 커맨드렛 산출물
- `UCodexInteractionAssetBuildCommandlet`
- `CodexUMGBootstrapEditor` 내 상호작용 애셋 생성 함수군
- `Texture2D` 생성 코드
- `Widget Blueprint` 생성 코드
- 테스트 Blueprint 생성 및 기본값 주입 코드

### 실제 콘텐츠 산출물
- `/Game/UI/Interaction/T_InteractionFilledCircle`
- `/Game/UI/Interaction/T_InteractionOuterRing`
- `/Game/UI/Interaction/WBP_InteractionIndicator`
- `/Game/Input/Actions/IA_Interact`
- `/Game/Input/Contexts/IMC_TopDown` 갱신
- `/Game/Data/Input/DA_TopDownInputConfig` 갱신
- `/Game/Blueprints/Interaction/BP_Interactable_Apple`
- `/Game/Blueprints/Interaction/BP_Interactable_Strawberry`
- `/Game/Maps/BasicMap` 내 테스트용 사과/딸기 배치 갱신

## 권장 런타임 구조

### 1. `UCodexInteractionComponent`
- 소유 데이터
  - 상호작용 타입 `ECodexInteractionType`
  - 표시 문구 `FText`
  - 가시 거리
  - 상호작용 가능 거리
  - 위젯 오프셋
  - 생성된 `Widget Blueprint` 클래스 참조
- 책임
  - BeginPlay 시 서브시스템에 자신을 등록
  - EndPlay 시 서브시스템에서 자신을 해제
  - 소유 액터에 `UWidgetComponent`를 런타임에 생성/부착
  - 서브시스템이 계산한 상태를 위젯에 반영
  - 입력 요청이 자신에게 들어오면 직접 처리하지 않고 서브시스템 호출로 전달
- 비책임
  - 가장 가까운 후보 판정
  - `먹기` 실제 실행
  - 다른 후보와의 우선순위 비교

### 2. `UCodexInteractionIndicatorWidget`
- `WBP_InteractionIndicator`의 C++ 베이스 클래스다.
- `BindWidget` 대상
  - `IMG_FilledCircle`
  - `IMG_OuterRing`
  - `BOR_PromptBackground`
  - `TXT_Prompt`
- 내부 상태
  - 현재 상태
  - 목표 상태
  - 보간 진행도
- 책임
  - `Hidden`, `VisibleRange`, `Interactable` 간 시각 상태 전환
  - 원형 마커 2종의 `RenderOpacity`, `RenderScale` 보간
  - 프롬프트 박스의 텍스트, 표시 여부, 간단한 페이드/슬라이드 보간
- 구현 원칙
  - `NativeConstruct`에서 기본 숨김 상태로 초기화한다.
  - `NativeTick` 또는 타이머 기반 보간으로 상태 전환을 처리한다.
  - `Hidden <-> VisibleRange` 전환은 동일한 파라미터를 역재생할 수 있어야 한다.
  - `VisibleRange -> Interactable` 전환은 원 마커는 유지하고 프롬프트 박스만 추가로 열리는 구조를 사용한다.

### 3. `UCodexInteractionSubsystem`
- `UWorldSubsystem` 기반 전역 수신자다.
- 책임
  - 등록된 `UCodexInteractionComponent` 목록 유지
  - 로컬 플레이어 기준 현재 후보 거리 계산
  - 현재 가장 가까운 상호작용 가능 후보 선택
  - 각 컴포넌트에 목표 위젯 상태 배포
  - 상호작용 요청 수신 및 디버그 출력
  - 필요 시 대상 액터의 `UCodexInteractionTarget` 인터페이스 호출
- 선택 이유
  - 월드 단위 틱이 필요하다.
  - 가장 가까운 대상 판정을 중앙에서 수행해야 한다.
  - 디버그 출력과 포커스 상태는 월드 문맥과 강하게 연결된다.
- 포커스 결정 규칙
  - `거리 > 가시 거리`면 `Hidden`
  - `상호작용 거리 < 거리 <= 가시 거리`면 `VisibleRange`
  - `거리 <= 상호작용 거리`면 상호작용 후보군에 포함
  - 후보군 중 가장 가까운 하나만 `Interactable`
  - 나머지 후보는 `VisibleRange`
- 동일 거리 안정성 규칙
  - 거리 오차가 매우 작을 때는 등록 순서 또는 액터 이름 기반의 안정적 tie-break 규칙을 둔다.
  - 프레임마다 대상이 번갈아 바뀌는 플리커를 허용하지 않는다.

### 4. `UCodexInteractionTarget` 인터페이스
- 목적
  - 상호작용 컴포넌트가 직접 게임플레이를 실행하지 않기 위한 수신 지점
  - 테스트용 사과/딸기 BP가 `먹기`에 반응하는 최소 경로
- 권장 함수
  - `HandleInteractionRequested(const FCodexInteractionRequest&)`
  - `HandleInteractionEnded(const FCodexInteractionRequest&)`
- 1차 테스트 동작
  - 요청 수신 시 디버그 출력
  - 짧은 지연 뒤 액터 숨김 또는 `DestroyActor`

## 위젯 상태 정의

### 1. `보이지 않음`
- 채워진 원 이미지 `Opacity = 0`
- 동심원 이미지 `Opacity = 0`
- 동심원 이미지 `Scale > 1.0`
- 프롬프트 박스 숨김

### 2. `가시거리`
- 채워진 원 이미지가 서서히 나타난다.
- 동심원 이미지가 더 큰 크기에서 시작해 채워진 원을 감싸는 크기까지 줄어든다.
- 동심원 이미지는 채워진 원보다 더 투명하게 유지한다.
- 프롬프트 박스는 열지 않는다.

### 3. `상호작용가능상태`
- `가시거리` 상태의 원 마커를 유지한다.
- 원 마커 오른쪽에 흰 배경 박스 + 검은 텍스트를 표시한다.
- 텍스트는 컴포넌트가 가진 문구를 사용한다.
- 이 상태는 가장 가까운 대상 하나에만 허용한다.

## 마커 텍스처 명세

### 공통 규칙
- 텍스처는 에디터 모듈 코드가 픽셀 단위로 직접 생성한다.
- 생성 실행은 `UCodexInteractionAssetBuildCommandlet`가 담당한다.
- 두 텍스처 모두 RGB는 순수 흰색 `255,255,255`를 사용한다.
- 형상은 알파 채널로만 만든다.
- 도형 바깥은 `Alpha = 0`이다.
- 도형 내부는 `Alpha = 255`를 기본값으로 사용한다.
- 가장자리 안티앨리어싱이 필요하면 경계 1픽셀 안에서만 알파를 보간하되, 형상 기준 반경 자체는 아래 수치를 고정한다.

### `T_InteractionFilledCircle`
- 용도
  - `IMG_FilledCircle` 브러시 텍스처
- 해상도
  - `64x64`
- 형상
  - 중심 기준 반경 `16`
  - 반경 `16` 내부는 흰색 + 알파 유효 영역
  - 반경 `16` 바깥은 완전 투명

### `T_InteractionOuterRing`
- 용도
  - `IMG_OuterRing` 브러시 텍스처
- 해상도
  - `64x64`
- 형상
  - 중심 기준 바깥 반경 `24`
  - 링 두께 `4`
  - 따라서 안쪽 반경은 `20`
  - 반경 `20 ~ 24` 구간만 흰색 + 알파 유효 영역
  - 반경 `< 20` 및 `> 24` 영역은 완전 투명

## 권장 시각 파라미터
- 채워진 원
  - 사용 텍스처: `T_InteractionFilledCircle`
  - 브러시 크기: `48x48`
  - `Hidden -> VisibleRange`: `Opacity 0.0 -> 1.0`
- 동심원
  - 사용 텍스처: `T_InteractionOuterRing`
  - 브러시 크기: `48x48`
  - `Hidden -> VisibleRange`: `Opacity 0.0 -> 0.6`
  - `Hidden -> VisibleRange`: `Scale 1.6 -> 1.0`
- 프롬프트 박스
  - `VisibleRange -> Interactable`: `Opacity 0.0 -> 1.0`
  - `VisibleRange -> Interactable`: X 오프셋 `+12 -> 0`
- 위젯 컴포넌트
  - `DrawSize`: `280x72`
  - 피벗: 마커 중심이 액터 위치에 오도록 X 기준 약 `24 / 280`
- 전환 시간
  - 0.15초에서 0.25초 사이의 짧은 보간을 기본값으로 둔다.
- 역방향 전환
  - 멀어질 때는 위 값을 반대로 보간한다.

## 입력 및 요청 흐름
1. 먼저 프로젝트에 기존 `Enhanced Input` 구현이 있는지 확인한다.
2. 확인 기준은 최소 아래 항목이다.
   - `EnhancedInputComponent` 바인딩 경로가 존재할 것
   - `UEnhancedInputLocalPlayerSubsystem`에 `IMC_`를 추가하는 경로가 존재할 것
   - 기존 `IA_`, `IMC_`, `DA_` 참조 구조가 실제로 연결되어 있을 것
3. 위 조건이 충족되면 새 입력도 반드시 그 기존 구조를 따라 확장한다.
4. 기존 `Enhanced Input` 구현이 없다면 여기서 작업을 멈추고, 사용자에게 최소 `Enhanced Input` 기반부터 먼저 구현해 달라고 요청한다.
5. `UCodexTopDownInputConfigDataAsset`에 `InteractAction` 참조를 추가한다.
6. 에디터 모듈 + 커맨드렛으로 `IA_Interact`를 생성한다.
7. 기존 `IMC_TopDown`에 `IA_Interact`를 추가한다.
8. `IA_Interact`의 키 매핑은 `F` 키로 고정한다.
9. [Source/CodexUMG/Private/CodexTopDownPlayerController.cpp](../Source/CodexUMG/Private/CodexTopDownPlayerController.cpp)에 기존 바인딩 패턴과 동일한 방식으로 상호작용 바인딩을 추가한다.
10. 플레이어 컨트롤러는 직접 대상 액터를 찾지 않고 서브시스템에 "현재 상호작용 요청"만 보낸다.
11. 서브시스템은 현재 `Interactable` 상태인 컴포넌트가 있으면 `FCodexInteractionRequest`를 만든다.
12. 서브시스템은 상호작용 요청/종료와 후보 없음에 대해서만 스크린 디버그 스트링을 출력한다.
13. 서브시스템은 대상 액터가 `UCodexInteractionTarget` 인터페이스를 구현한 경우에만 후속 콜을 전달한다.
14. 테스트용 사과/딸기 BP는 이 콜을 받아 `먹기` 동작을 수행한다.

## 애셋 생성 계획

### 1. 빌드 의존성
- [Source/CodexUMG/CodexUMG.Build.cs](../Source/CodexUMG/CodexUMG.Build.cs)
  - `UMG`를 런타임 의존성에 추가한다.
- [Source/CodexUMGBootstrapEditor/CodexUMGBootstrapEditor.Build.cs](../Source/CodexUMGBootstrapEditor/CodexUMGBootstrapEditor.Build.cs)
  - `UMG`
  - `UMGEditor`
  - 필요 시 `Slate`, `SlateCore` 유지

### 2. 커맨드렛 책임
- 새 커맨드렛 이름은 `UCodexInteractionAssetBuildCommandlet`를 권장한다.
- 책임
  - 기존 `Enhanced Input` 구현 흔적이 있는지 먼저 검사
  - 구현이 없으면 즉시 실패 처리하고 사용자에게 선행 구현 필요 상태를 알릴 수 있는 메시지 남김
  - 상호작용 관련 디렉터리 생성
  - `T_InteractionFilledCircle`, `T_InteractionOuterRing` 생성 또는 갱신
  - `IA_Interact` 생성 또는 갱신
  - 기존 `IMC_TopDown`에 `IA_Interact`를 `F` 키로 매핑
  - `DA_TopDownInputConfig`에 `InteractAction` 연결
  - `WBP_InteractionIndicator` 생성 또는 갱신
  - 테스트용 `BP_Interactable_Apple`, `BP_Interactable_Strawberry` 생성 또는 갱신
  - `BasicMap`을 로드해 테스트용 사과/딸기를 플레이어 시작 지점 근처에 배치 또는 재배치
  - 수정된 `BasicMap` 저장
  - 애셋 컴파일 및 저장
- 실행 전제
  - 에디터 UI 세션이 열려 있지 않은 상태에서 실행한다.
  - 충돌 가능성이 보이면 즉시 중단하고 사용자에게 알린다.

### 3. `WBP_InteractionIndicator` 생성 방식
- `UWidgetBlueprintFactory`로 실제 `Widget Blueprint` 애셋을 만든다.
- 베이스 클래스는 `UCodexInteractionIndicatorWidget`로 고정한다.
- 기본 위젯 트리
  - Root: `CanvasPanel`
  - Marker Group: `Overlay`
  - `IMG_FilledCircle`
  - `IMG_OuterRing`
  - `BOR_PromptBackground`
  - `TXT_Prompt`
- `IMG_FilledCircle`는 `T_InteractionFilledCircle`를 브러시로 사용한다.
- `IMG_OuterRing`는 `T_InteractionOuterRing`를 브러시로 사용한다.
- `IMG_FilledCircle`, `IMG_OuterRing`의 브러시 크기는 자산 생성 시 `Brush.ImageSize = 48x48`로 직접 저장한다.
- `SetDesiredSizeOverride()`만 호출하는 방식에 의존하지 않는다.
  - 이유: 에디터 코드로 `Widget Blueprint`를 생성할 때는 디자이너 자산에 크기가 저장되지 않아 `Image Size = 0,0`이 남을 수 있다.
- 프롬프트 박스는 원 마커 오른쪽에 배치한다.
- 기본 가시성은 숨김 상태여야 한다.
- 위젯 트리 생성은 코드로 끝내고, 애니메이션은 런타임 C++ 보간으로 처리한다.

### 4. 텍스처 생성 방식
- 에디터 모듈은 `UTexture2D` 애셋을 직접 생성하거나 갱신한다.
- 각 텍스처의 픽셀은 중심 좌표와 픽셀 중심 간 거리를 계산해 채운다.
- `T_InteractionFilledCircle`
  - 크기 `64x64`
  - 거리 `<= 16`인 픽셀은 `RGBA = 255,255,255,255`
  - 그 외 픽셀은 `RGBA = 255,255,255,0`
- `T_InteractionOuterRing`
  - 크기 `64x64`
  - 거리 `>= 20 && <= 24`인 픽셀은 `RGBA = 255,255,255,255`
  - 그 외 픽셀은 `RGBA = 255,255,255,0`
- 위젯은 머터리얼 없이 기본 이미지 브러시만으로도 원하는 형상을 표시할 수 있어야 한다.

### 5. 테스트 Blueprint 생성 방식
- `BP_Interactable_Apple`
  - 부모는 테스트용 베이스 액터 또는 `AActor` 기반 BP
  - `StaticMeshComponent`는 `/Game/Vox/Meshes/Food/SM_Vox_Apple`
  - `UCodexInteractionComponent` 부착
  - 상호작용 타입은 `Eat`
  - 표시 문구는 `먹기`
- `BP_Interactable_Strawberry`
  - `StaticMeshComponent`는 `/Game/Vox/Meshes/Food/SM_Vox_Strawberry`
  - 나머지 기준은 사과와 동일
- 두 BP 모두 `UCodexInteractionTarget` 구현 경로를 가져야 한다.

### 6. 권장 커맨드 예시
```powershell
$ProjectRoot = (Resolve-Path .).Path
$ProjectPath = Join-Path $ProjectRoot 'CodexUMG.uproject'
UnrealEditor-Cmd.exe $ProjectPath -run=CodexUMGBootstrapEditor.CodexInteractionAssetBuildCommandlet -unattended -nop4 -nosplash
```

## 테스트 대상 배치 계획
- 기본 검증 맵은 기존 `Content/Maps/BasicMap.umap`을 우선 사용한다.
- 커맨드렛이 `BasicMap`을 직접 열고 테스트용 사과/딸기를 플레이어 이동 경로 근처에 배치한다.
- 일부러 서로 가까운 거리에도 배치해, 두 대상이 동시에 상호작용 거리 안에 들어와도 프롬프트가 하나만 열리는지 확인한다.
- 시작 직후에는 가시 거리 안이지만 상호작용 거리는 아닌 지점에 두고, 이동 후 프롬프트 단일 표시를 확인한다.

## 권장 작업 순서
1. 런타임 타입과 인터페이스를 먼저 고정한다.
2. 기존 `Enhanced Input` 구현 유무를 확인하고, 기존 입력 파이프라인이 실제로 동작 중인지 검증한다.
3. 기존 `Enhanced Input`이 없으면 여기서 작업을 중단하고 사용자에게 선행 구현을 요청한다.
4. `UCodexInteractionSubsystem`의 후보 등록, 거리 판정, 단일 포커스 선택 로직을 구현한다.
5. `UCodexInteractionComponent`와 `UWidgetComponent` 생성/등록 흐름을 구현한다.
6. `UCodexInteractionIndicatorWidget`의 상태 보간 로직을 구현한다.
7. 입력 설정 확장과 플레이어 컨트롤러 바인딩을 기존 패턴에 맞춰 추가한다.
8. 에디터 모듈에 위젯/테스트 BP 생성 코드를 추가한다.
9. `UCodexInteractionAssetBuildCommandlet`를 구현하고 애셋을 실제 생성한다.
10. 커맨드렛으로 `BasicMap`까지 갱신한 뒤 PIE 검증을 수행한다.

## 리드타임 단축을 위한 에이전트 운영 방식

### 기본 방침
- 병렬 처리 가능한 작업은 적극적으로 에이전트에 분리한다.
- 단, 같은 파일과 같은 Blueprint를 동시에 수정하는 병렬 작업은 금지한다.
- 공유 헤더, `Build.cs`, 공용 데이터 자산 구조 변경은 메인 작업자가 소유한다.

### 권장 역할 분리
- Agent A: 런타임 상호작용 타입과 컴포넌트 담당
  - `UCodexInteractionComponent`
  - `ECodexInteractionType`
  - `ECodexInteractionWidgetState`
- Agent B: 전역 서브시스템과 입력 라우팅 담당
  - `UCodexInteractionSubsystem`
  - 플레이어 컨트롤러 입력 바인딩
  - 가장 가까운 대상 판정 로직
- Agent C: UMG 베이스 위젯과 상태 보간 담당
  - `UCodexInteractionIndicatorWidget`
  - 위젯 상태별 페이드/스케일 규칙
- Agent D: 에디터 모듈과 커맨드렛 담당
  - `T_InteractionFilledCircle`, `T_InteractionOuterRing` 생성 코드
  - `WBP_InteractionIndicator` 생성 코드
  - `BP_Interactable_Apple`, `BP_Interactable_Strawberry` 생성 코드
  - `UCodexInteractionAssetBuildCommandlet`
- Agent E: 테스트 자산 검증 담당
  - PIE에서 상태 전환 확인
  - 프롬프트 단일 표시 확인
  - 요청/종료 디버그 스트링 확인
- Agent F: 평가 전용 담당
  - 구현 에이전트와 분리
  - 구조 위반, 누락, 병렬화 미흡 지점 검토

### 병렬 처리 시작 조건
- 먼저 메인 작업자가 아래 인터페이스를 고정한다.
  - `ECodexInteractionType`
  - `ECodexInteractionWidgetState`
  - `FCodexInteractionRequest`
  - `UCodexInteractionTarget` 함수 시그니처
- 이 인터페이스가 고정된 뒤에 Agent A/B/C/D를 병렬로 투입한다.
- `Build.cs`, 공용 애셋 경로 상수, 공용 헤더 이름 변경은 메인 작업자가 순차 반영한다.

## 스크린 디버그 출력 기준
- 포커스 시작/종료
  - 스크린 디버그 스트링으로 출력하지 않는다.
  - 필요 시 Output Log에만 `Interaction Focus Start: Apple / Eat`, `Interaction Focus End: Apple / Eat` 형식으로 남긴다.
- 요청 시작
  - `Interaction Requested: Apple / Eat`
- 요청 종료
  - `Interaction Ended: Apple / Eat`
- 후보 없음
  - `Interaction Requested: None`

## 검증 체크리스트
- 프로젝트에 기존 `Enhanced Input` 구현이 실제로 존재하는가
- 상호작용 구현이 기존 `Enhanced Input` 구조를 그대로 따라가도록 설계되었는가
- 기존 `Enhanced Input`이 없을 경우 즉시 중단하고 사용자 요청으로 전환하는 규칙이 반영되었는가
- `Commandlet` 실행만으로 `T_InteractionFilledCircle`, `T_InteractionOuterRing`가 실제 애셋으로 생성되는가
- 채워진 원 텍스처가 반경 `16`의 흰색 알파 마스크로 생성되는가
- 링 텍스처가 바깥 반경 `24`, 두께 `4`, 안쪽 반경 `20`의 흰색 알파 마스크로 생성되는가
- `IA_Interact`가 실제 애셋으로 생성되는가
- `IA_Interact`가 기존 `IMC_TopDown`에 추가되는가
- `IA_Interact`의 키 매핑이 `F` 키인가
- `Commandlet` 실행만으로 `WBP_InteractionIndicator`가 실제 애셋으로 생성되는가
- `WBP_InteractionIndicator`의 `IMG_FilledCircle`, `IMG_OuterRing` 브러시 `Image Size`가 `0,0`이 아닌 명시값으로 저장되는가
- `Commandlet` 실행만으로 테스트용 사과/딸기 BP가 실제 애셋으로 생성되는가
- `Commandlet` 실행만으로 `BasicMap`에 테스트용 사과/딸기가 실제로 배치되는가
- 프로젝트 실행 직후 `BasicMap`에서 사과/딸기가 카메라에 보이는가
- 플레이어가 멀리 있을 때 위젯이 전혀 보이지 않는가
- 가시 거리 진입 시 채워진 원과 동심원이 페이드/스케일 보간으로 나타나는가
- 가시 거리 이탈 시 동일 전환이 역방향으로 재생되는가
- 상호작용 거리 안에 여러 대상이 있어도 흰 박스 + 검은 텍스트 프롬프트는 하나만 보이는가
- 가장 가까운 대상이 바뀌면 프롬프트 소유권도 안정적으로 이동하는가
- 상호작용 입력이 들어왔을 때 컴포넌트가 직접 처리하지 않고 서브시스템만 호출하는가
- 서브시스템이 상호작용 요청/종료와 후보 없음에 대해서만 스크린 디버그 스트링을 출력하는가
- 포커스 시작/종료는 스크린 디버그 스트링으로 출력하지 않는가
- 사과/딸기 BP가 `먹기` 반응을 실제로 수행하는가
- 액터가 사라지거나 파괴될 때 서브시스템 등록 해제가 안전하게 일어나는가

## 금지 사항
- 상호작용 컴포넌트가 직접 `먹기` 로직이나 액터 파괴를 수행하는 방식
- 각 컴포넌트가 자기 혼자 가장 가까운 대상인지 판정하는 방식
- 여러 대상에 프롬프트 텍스트 박스를 동시에 띄우는 방식
- 원형 텍스처를 외부 PNG 수동 임포트에 의존하는 방식
- 기존 `Enhanced Input` 구조가 있는데 별도 입력 체계를 병행 도입하는 방식
- 기존 `Enhanced Input` 구현이 없는데도 추정으로 입력 시스템을 새로 박아 넣고 계속 진행하는 방식
- 기존 `IMC_`를 무시하고 별도 상호작용 전용 `IMC_`를 추가하는 방식
- `Widget Blueprint`를 에디터에서 수동으로만 만들어 두고 코드 생성 경로를 생략하는 방식
- 에디터 UI 세션이 열린 상태를 전제로 자산 생성하는 방식
- 상태 전환마다 별도 BP 애니메이션 애셋 수작업을 요구하는 방식

## 메모
- 본 문서는 구현 코드가 아니라 작업 지시용 계획 문서다.
- 실제 클래스명과 경로는 프로젝트 규칙에 맞게 조정할 수 있다.
- 다만 아래 원칙은 유지한다.
  - 마커 텍스처는 에디터 모듈 코드로 생성해야 한다.
  - 실제 위젯 애셋은 존재해야 한다.
  - 위젯 애셋은 에디터 모듈 코드 + 커맨드렛으로 생성해야 한다.
  - 입력은 기존 `Enhanced Input` 구현을 먼저 확인하고 그 구조를 따라야 한다.
  - 기존 `Enhanced Input` 구현이 없으면 작업을 멈추고 사용자에게 선행 구현을 요구해야 한다.
  - `IA_Interact`는 기존 `IMC_`에 추가하며 매핑은 `F` 키를 사용한다.
  - 가장 가까운 대상 하나만 `상호작용가능상태`를 가진다.
  - 컴포넌트는 직접 액션을 처리하지 않고 서브시스템에 전달만 한다.
  - 병렬 처리 가능한 작업은 에이전트 단위로 분리해 리드타임을 줄인다.
