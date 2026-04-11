# 상호작용 통합 UI 플레이그라운드 팝업 구현 계획

## 문서 목적
- 이 문서는 하나의 상호작용 팝업 안에서 여러 종류의 UMG 컨트롤과 상호작용 패턴을 함께 검증할 수 있는 통합 `UI Playground` 팝업의 구현 계획만 정의한다.
- 실제 C++ 수정, WBP 생성, 애셋 생성, 커맨드렛 실행, 맵 배치 작업은 이 문서에서 수행하지 않는다.
- WBP 생성/참조 규칙은 반드시 [wbp_asset_work_guidelines.md](./wbp_asset_work_guidelines.md)를 따른다.

## 고정 규칙
- 팝업의 실제 위젯 트리는 런타임 코드가 아니라 실제 `WBP_*` 애셋의 `WidgetTree`에 존재해야 한다.
- 다른 시스템이 이 팝업을 사용할 때도 `_C` 클래스 경로가 아니라 WBP 애셋 경로를 기준 참조로 삼는다.
- WBP 및 관련 UI 애셋 생성/갱신 로직은 에디터 모듈 코드에 둔다.
- 실행 진입은 `Commandlet`로만 수행한다.
- 현재 프로젝트의 `UnrealEditor.exe`가 실행 중이라 충돌이 나면 우회하지 말고 즉시 중단 후 사용자에게 알린다.
- 이미 생성된 상호작용 BP, WBP, UI 텍스처가 있고 생성 레시피가 동일하다면 애셋을 새로 만들거나 다시 저장하지 않고 기존 파일을 그대로 유지한다.
- 동일 내용인데 내부 식별자만 달라져 소스 관리 이력이 오염되지 않도록, 생성 로직은 반드시 idempotent하게 설계한다.
- 이 팝업은 `한 팝업 안에 여러 테스트를 담는다`는 목적을 가지지만, `한 화면에 모든 항목을 동시에 노출`하는 방식은 금지한다. 기능군은 반드시 탭, 좌측 내비게이션, `WidgetSwitcher` 같은 섹션 분리 구조로 나눈다.
- 입력 필드가 포커스를 잡고 있을 때는 팝업 닫기 단축키, 게임플레이 입력, 섹션 전환 입력이 의도치 않게 함께 소비되지 않도록 우선순위를 명시적으로 제어한다.
- 섹션 간 포커스 이동 순서와 기본 포커스 대상은 암묵적 엔진 기본값에 맡기지 말고 명시적으로 설계한다.
- `ListView`/`TileView`의 item 내부 필드만 바뀌는 경우 엔트리 위젯이 자동으로 새로 그려진다고 가정하지 않는다. 시각 상태가 바뀌면 `RegenerateAllEntries()` 또는 동등한 확실한 갱신 경로를 호출한다.
- 드래그 앤 드롭은 개별 엔트리마다 제각각 처리하지 말고, 가능한 한 상위 페이지 또는 팝업 위젯이 중앙에서 상태와 수락 규칙을 관리한다.
- 이 팝업은 실제 게임 기능 UI라기보다 회귀 테스트와 학습용 검증 허브다. 따라서 상태 출력용 텍스트, 이벤트 로그, 현재 포커스 표시 같은 디버그 친화적 UI를 허용한다.

## 현재 위치와 설계 방향
- 현재 프로젝트에는 단일 메시지 팝업, 스크롤 메시지 팝업, 듀얼 타일 이동 팝업이 각각 별도 상호작용 액터와 별도 WBP로 구현되어 있다.
- 기존 구조는 `개별 기능을 독립적으로 검증`하는 데 유리하지만, 여러 종류의 컨트롤을 한 번에 회귀 테스트하려면 액터를 여러 개 오가야 한다.
- 이번 문서의 목표는 기존 개별 팝업을 대체하는 것이 아니라, `버튼/텍스트/이미지/입력/리스트/드래그/포커스`를 한 번에 점검할 수 있는 통합 테스트용 팝업을 추가하는 것이다.
- 따라서 기존 메시지 팝업 계열은 유지하고, 그 위에 별도의 `UI Playground` 팝업과 전용 상호작용 액터를 추가하는 방향을 권장한다.

## 목표 상태
1. 플레이어가 전용 테스트용 상호작용 액터에 접근하면 `UI Playground` 팝업이 화면 중앙에 열린다.
2. 팝업은 하나만 열리며, 내부는 `Basic`, `Input`, `Collection`, `Advanced`의 4개 섹션으로 분리된다.
3. 사용자는 마우스뿐 아니라 키보드와 게임패드로도 섹션 전환과 내부 포커스 이동을 검증할 수 있다.
4. `Basic` 섹션에서는 버튼, 텍스트, 이미지, 배경 블러, 라운딩, 상태 출력 같은 기본 UI 요소를 검증한다.
5. `Input` 섹션에서는 `EditableTextBox`, `MultiLineEditableTextBox`, `CheckBox`, `Slider`, `SpinBox`, `ComboBoxString`을 검증한다.
6. `Collection` 섹션에서는 스크롤 가능한 리스트, 재사용 가능한 엔트리 기반 `ListView` 또는 `TileView`, 항목 추가/삭제/선택을 검증한다.
7. `Advanced` 섹션에서는 드래그 앤 드롭과 포커스 경로 표시를 검증한다.
8. 팝업 전체에는 `닫기`, `전체 리셋`, `현재 상태 표시`가 존재해 반복 테스트가 가능해야 한다.
9. 팝업을 닫을 때는 마지막 섹션, 입력 값, 선택 값, 드래그 결과 같은 최소한의 테스트 결과를 상호작용 서브시스템 경유로 회수할 수 있어야 한다.
10. 이 팝업은 실제 설정 저장 기능이나 게임 규칙 변경 기능을 수행하지 않고, 상호작용 UI 검증 전용으로 동작한다.

## 범위
- 이 문서는 팝업 UI 구조, 팝업 내부 상태, 상호작용 서브시스템 연동, 테스트용 액터 설계만 다룬다.
- 1차 구현 범위에서는 모든 섹션이 `고정된 샘플 데이터`를 사용해도 된다.
- 각 섹션은 실전 기능 완성보다 `UMG 컨트롤이 정상 렌더링/입력/갱신/포커스/드래그를 수행하는지`를 검증하는 데 집중한다.
- 통합 검증을 위한 테스트용 상호작용 액터 1종과 `BasicMap` 배치는 현재 문서 범위에 포함한다.

## 비범위
- 실제 게임 옵션 저장, 세이브 파일 반영, 사용자 설정 영속화
- 네트워크 동기화
- 다중 팝업 중첩
- 실제 인벤토리 시스템, 퀘스트 시스템, 상점 시스템 연동
- 리치 텍스트 마크업, 복잡한 애니메이션, 접근성 현지화 세부 조정

## 사용자 기능 요구사항

### 1. 팝업 공통 레이아웃
- 상단에는 타이틀, `전체 리셋`, `닫기` 버튼이 있는 타이틀바가 있다.
- UI Playground 팝업은 상단 세로 여백을 기존 메시지 팝업보다 조금 더 넉넉하게 두고, 하단 여백은 상태 바와 균형이 깨지지 않는 선에서만 보정한다.
- 본문은 좌측 섹션 내비게이션과 우측 현재 페이지 콘텐츠 영역으로 나뉜다.
- 좌측 내비게이션은 `Basic`, `Input`, `Collection`, `Advanced` 4개의 탭 버튼을 가진다.
- 좌측 탭 컬럼은 버튼 스택이 패널 상단에 바로 붙어 보이지 않도록 별도 top/bottom padding을 가진다.
- 우측 페이지 영역은 `WidgetSwitcher` 또는 동등한 구조로 한 번에 하나의 섹션만 표시한다.
- 하단에는 현재 상태나 마지막 이벤트를 보여주는 상태 바가 있다.
- 페이지별 콘텐츠가 길어질 수 있으므로 각 페이지는 독립 `ScrollBox`를 가져도 된다.

### 2. `Basic` 섹션
- 제목 텍스트, 본문 텍스트, 줄바꿈 텍스트, 짧은 보조 텍스트를 함께 배치한다.
- 버튼은 최소 `Primary`, `Secondary`, `Disabled`, `Close-like` 4종 상태를 보여준다.
- 버튼의 pressed 상태는 색과 음영만 달라지고, 내부 padding이나 전체 높이는 바뀌지 않아야 한다.
- 버튼 클릭 시 하단 상태 바 텍스트가 즉시 갱신되어 이벤트가 눈에 보이게 한다.
- 이미지 영역에는 기존 프로젝트의 원형 인디케이터 계열 텍스처, 스마일 아이콘, 또는 동일 톤의 샘플 이미지를 표시한다.
- 블러와 라운딩은 이 섹션에서 별도 설명 없이도 눈에 보이도록 패널 외형에 직접 반영한다.
- 필요 시 `ProgressBar`를 포함해 `Slider`와 시각적으로 연동되는 단순 진행률 표현을 추가할 수 있다.

### 3. `Input` 섹션
- `EditableTextBox` 1개와 `MultiLineEditableTextBox` 1개가 존재한다.
- `CheckBox`는 단일 토글 예시로 사용한다.
- `Slider`와 `SpinBox`는 같은 값 또는 관련 값을 표시하는 예시를 가진다.
- `ComboBoxString`은 최소 3개 이상의 선택지를 제공한다.
- 현재 입력 결과는 섹션 내부 요약 텍스트 또는 전역 상태 바에 즉시 반영한다.
- `Reset` 수행 시 모든 입력 값은 문서에 정의된 기본값으로 되돌아간다.
- 입력 필드에 포커스가 있을 때 `Tab`, `Shift+Tab`, 방향키, 게임패드 네비게이션이 어떻게 동작하는지 확인 가능해야 한다.

### 4. `Collection` 섹션
- `ScrollBox` 기반 샘플 목록이 존재한다.
- 재사용 가능한 엔트리를 가진 `ListView` 또는 `TileView`가 최소 하나 이상 존재한다.
- 각 항목은 텍스트, 보조 설명, 선택 상태, 비활성 상태 중 일부를 표현할 수 있어야 한다.
- `추가`, `삭제`, `선택 변경` 버튼을 통해 엔트리 재생성과 상태 갱신을 검증할 수 있어야 한다.
- 항목 수가 한 화면을 넘도록 샘플 데이터를 유지해 초기 상태에서도 스크롤을 검증할 수 있어야 한다.
- 1차 구현에서는 `ListView`와 `TileView`를 모두 넣어도 되고, 최소 `ListView` 1종에 필요한 보조 샘플만 추가해도 된다.

### 5. `Advanced` 섹션
- 드래그 소스와 드롭 타깃 영역이 분리되어 있어야 한다.
- 드래그 가능한 항목은 최소 4개 이상 제공한다.
- 빈 슬롯 드롭, 채워진 슬롯 드롭, 무효 위치 드롭을 모두 확인할 수 있어야 한다.
- 드롭 가능 대상 위에 포인터가 올라가면 시각적 강조가 즉시 보여야 한다.
- 현재 포커스된 위젯 이름 또는 논리 위치를 보여주는 디버그 텍스트를 둔다.
- 마우스만이 아니라 키보드/게임패드 기준의 포커스 이동 순서가 눈에 보이도록 설계한다.

### 6. 닫기와 리셋
- `닫기` 버튼으로 팝업을 종료할 수 있다.
- 필요하면 `Esc` 또는 별도 닫기 입력을 허용하되, 텍스트 입력 중에는 의도치 않게 닫히지 않도록 한다.
- `전체 리셋`은 현재 섹션만이 아니라 모든 섹션의 상태를 기본값으로 되돌린다.
- `전체 리셋`은 팝업을 닫지 않고 반복 테스트를 가능하게 해야 한다.

## 데이터 구조 권장안

### 1. 공통 팝업 요청과 전용 페이로드 분리
- 기존 `FCodexInteractionPopupRequest`에 `Name`, `ComboOptions`, `ListEntries`, `DragSources` 같은 전용 필드를 무분별하게 추가하지 않는 것을 권장한다.
- `UI Playground` 전용 초기값은 별도 페이로드 객체나 전용 구조체로 분리한다.
- 권장 예시
  - `UCodexInteractionUIPlaygroundPayload`
  - 또는 `FCodexInteractionUIPlaygroundRequest`

### 2. 전용 요청 데이터 권장 필드
- `FText Title`
- `ECodexUIPlaygroundSection InitialSection`
- `FString DefaultSingleLineText`
- `FString DefaultMultiLineText`
- `bool bDefaultChecked`
- `float DefaultSliderValue`
- `int32 DefaultSpinValue`
- `TArray<FString> ComboOptions`
- `int32 DefaultComboIndex`
- `TArray<FCodexUIPlaygroundListItemData> ListItems`
- `TArray<FCodexUIPlaygroundTileItemData> TileItems`
- `TArray<FCodexUIPlaygroundDragItemData> DragSourceItems`
- `TArray<FCodexUIPlaygroundDragItemData> DragTargetItems`

### 3. 전용 결과 데이터 권장 필드
- `FGuid RequestId`
- `ECodexUIPlaygroundSection LastActiveSection`
- `FString FinalSingleLineText`
- `FString FinalMultiLineText`
- `bool bFinalChecked`
- `float FinalSliderValue`
- `int32 FinalSpinValue`
- `int32 FinalComboIndex`
- `TArray<FString> FinalListItemIds`
- `TArray<FString> FinalDragTargetItemIds`
- `bool bWasClosed`

### 4. 리스트 항목 데이터 권장 필드
- `FName ItemId`
- `FText Label`
- `FText Description`
- `bool bIsEnabled`
- `bool bIsSelected`
- `FLinearColor AccentColor`

### 5. 드래그 항목 데이터 권장 필드
- `FName ItemId`
- `FText Label`
- `int32 SourceIndex`
- `bool bIsTargetSlot`

## WBP 애셋 구성 계획

### 1. 실제 WBP 애셋
- 팝업 본체
  - `/Game/UI/Interaction/WBP_InteractionUIPlaygroundPopup`
- 리스트 엔트리
  - `/Game/UI/Interaction/WBP_InteractionUIPlaygroundListEntry`
- 타일 또는 드래그 항목 엔트리
  - `/Game/UI/Interaction/WBP_InteractionUIPlaygroundTileEntry`

### 2. 실제 BP 애셋
- 테스트용 상호작용 액터 BP
  - `/Game/Blueprints/Interaction/BP_Interactable_WoodenSignUIPlaygroundPopup`

### 3. 스타일/텍스처 재사용 정책
- 가능하면 기존 상호작용 팝업의 버튼 스타일, 블러 기반 패널 외형, 둥근 브러시 구성을 재사용한다.
- 이미지 샘플은 기존 `T_InteractionSmileYellow`, `T_InteractionFilledCircle`, `T_InteractionOuterRing`, `T_InteractionTileRoundedVerticalGradient`를 우선 재사용한다.
- 새로운 이미지가 꼭 필요하지 않다면 외부 이미지 임포트는 추가하지 않는다.

## WBP 위젯 트리 권장 구조

### 1. 팝업 본체
- `RootCanvas`
- `Overlay_ScreenRoot`
- `SizeBox_PopupFrame`
- `BackgroundBlur_Panel`
- `Border_TintPanel`
- `VerticalBox_Content`
- `HorizontalBox_TitleBar`
  - `TXT_Title`
  - `BTN_ResetAll`
  - `BTN_Close`
- `HorizontalBox_Body`
  - `Border_LeftNav`
    - `VerticalBox_NavButtons`
      - `BTN_TabBasic`
      - `BTN_TabInput`
      - `BTN_TabCollection`
      - `BTN_TabAdvanced`
  - `WidgetSwitcher_Pages`
    - `ScrollBox_PageBasic`
    - `ScrollBox_PageInput`
    - `ScrollBox_PageCollection`
    - `ScrollBox_PageAdvanced`
- `Border_StatusBar`
  - `TXT_Status`

### 2. `Basic` 페이지 내부 예시
- `VerticalBox_PageBasicContent`
  - `TXT_BasicHeadline`
  - `TXT_BasicBody`
  - `HorizontalBox_ImagePreview`
    - `IMG_SmileIcon`
    - `IMG_FilledCircle`
    - `IMG_OuterRing`
  - `WrapBox_ButtonSamples`
    - `BTN_Primary`
    - `BTN_Secondary`
    - `BTN_DisabledSample`
    - `BTN_StatusPing`
  - 선택 사항 `ProgressBar_Sample`

### 3. `Input` 페이지 내부 예시
- `VerticalBox_PageInputContent`
  - `TXT_InputSummary`
  - `EditableTextBox_Name`
  - `MultiLineEditableTextBox_Notes`
  - `CheckBox_EnableOption`
  - `Slider_Value`
  - `SpinBox_Count`
  - `ComboBox_Mode`

### 4. `Collection` 페이지 내부 예시
- `VerticalBox_PageCollectionContent`
  - `HorizontalBox_CollectionActions`
    - `BTN_ListAdd`
    - `BTN_ListRemove`
    - `BTN_ListToggleSelection`
  - `ScrollBox_SampleList`
  - `ListView_Items`
  - 선택 사항 `TileView_Items`

### 5. `Advanced` 페이지 내부 예시
- `VerticalBox_PageAdvancedContent`
  - `TXT_FocusStatus`
  - `HorizontalBox_DragArea`
    - `WrapBox_DragSource`
    - `WrapBox_DragTarget`
  - `TXT_DragStatus`

### 6. 엔트리 WBP 공통 원칙
- 리스트/타일 엔트리는 실제 `UserWidget` 기반 WBP로 존재해야 한다.
- 항목 데이터 변경 시 어떤 필드가 재렌더 트리거를 담당하는지 명확히 해야 한다.
- 선택 강조, 비활성, 포커스 상태는 엔트리 내부에서 표현하되, 최종 상태 변경 책임은 상위 위젯이 가진다.

## 시각 스타일 계획

### 1. 전체 방향
- 기본 외형은 기존 메시지 팝업 계열과 유사한 중앙 모달, 배경 블러, 낮은 불투명도 오버레이, 둥근 패널을 유지한다.
- 기존 팝업보다 크기를 키우되, 화면을 꽉 채우는 전체 화면 UI는 피한다.
- 좌측 내비게이션과 우측 콘텐츠 영역의 시각 대비를 분리해 현재 섹션 구조가 한눈에 들어오게 한다.

### 2. 블러와 라운딩
- 패널 뒤는 `BackgroundBlur`로 흐리게 처리한다.
- 메인 패널과 섹션 카드에는 라운딩된 브러시를 사용한다.
- 버튼도 기존 상호작용 팝업과 같은 톤의 둥근 버튼 스타일을 공유한다.
- 버튼 피드백은 눌림 색 변화 중심으로 주고, pressed padding 차이로 높이가 흔들리는 연출은 금지한다.

### 3. 색상 방향
- 메인 팝업은 기존 하늘색 또는 중립 청록 계열 틴트를 기본으로 삼는다.
- 섹션 선택 강조에는 노란색 또는 민트 계열을 포인트 컬러로 사용한다.
- `Basic`, `Input`, `Collection`, `Advanced` 섹션은 완전히 다른 테마를 갖기보다 같은 디자인 시스템 안에서 포인트 색만 약하게 달리하는 편을 권장한다.

### 4. 디버그 정보 표시
- 상태 바와 포커스 상태 텍스트는 너무 눈에 거슬리지 않으면서도 즉시 읽을 수 있게 대비를 확보한다.
- 디버그 표시는 실서비스 UI보다 조금 더 노골적이어도 괜찮다.

## 런타임 구조 권장안

### 1. 팝업 본체 C++ 클래스
- 권장 이름
  - `UCodexInteractionUIPlaygroundPopupWidget`
- 책임
  - 초기 요청 또는 페이로드 적용
  - 섹션 전환
  - 버튼/입력/리스트/드래그 이벤트 수신
  - 전역 상태 바 갱신
  - 전체 리셋
  - 닫기 결과 전달
  - 포커스 이동 규칙 적용

### 2. 리스트 엔트리 C++ 클래스
- 권장 이름
  - `UCodexInteractionUIPlaygroundListEntryWidget`
- 책임
  - 항목 텍스트와 보조 설명 표시
  - 선택/비활성/강조 표현
  - 상위 팝업 또는 페이지에서 전달한 상태를 시각화

### 3. 타일 또는 드래그 엔트리 C++ 클래스
- 권장 이름
  - `UCodexInteractionUIPlaygroundTileEntryWidget`
- 책임
  - 드래그 가능한 항목 표시
  - 선택 및 드롭 가능 강조 표현
  - 실제 상태 변경은 상위 페이지에 위임

### 4. 드래그 앤 드롭 오퍼레이션
- 권장 이름
  - `UCodexUIPlaygroundDragDropOperation`
- 필드 예시
  - `FName ItemId`
  - `int32 SourceIndex`
  - `bool bFromSourcePanel`
- 목적
  - 드래그 원본과 드롭 적용 대상을 명확히 전달한다.

### 5. 상호작용 액터
- 권장 이름
  - `ACodexUIPlaygroundPopupInteractableActor`
- 권장 상속
  - `ACodexPopupInteractableActor`
- 책임
  - 통합 팝업용 기본 타이틀과 테스트 샘플 데이터를 보관한다.
  - 상호작용 시 `UI Playground` 전용 팝업 스타일과 페이로드를 구성한다.

### 6. 상호작용 서브시스템 확장 방향
- `ECodexInteractionPopupStyle`에 `UIPlayground` 값을 추가하는 방식을 권장한다.
- 서브시스템은 `PopupStyle`에 따라 `WBP_InteractionUIPlaygroundPopup`을 로드하고 적절한 위젯 클래스를 생성한다.
- `UI Playground` 전용 페이로드는 공통 팝업 요청과 분리된 경로로 해석한다.

## 테스트용 상호작용 액터와 맵 배치 계획
- 테스트용 BP 이름은 `BP_Interactable_WoodenSignUIPlaygroundPopup`을 권장한다.
- 기본 메시는 기존 `/Game/Vox/Meshes/Props/SM_Vox_WoodenSignpost`를 재사용한다.
- `InteractionType`은 `Use`를 사용한다.
- `PromptText`는 `테스트`, `열기`, 또는 `UI 테스트` 중 하나로 고정한다.
- `BasicMap`에는 기존 팝업 테스트 액터와 구분되는 별도 위치에 배치한다.
- 맵 배치 시 다른 팝업 테스트 액터와 너무 붙여 배치하지 않아 포커스 우선순위가 헷갈리지 않게 한다.

## 단계적 구현 권장 순서
1. 공통 팝업 껍데기와 좌측 내비게이션, 페이지 전환 구조를 먼저 만든다.
2. `Basic` 섹션을 구현해 버튼/텍스트/이미지/상태 바를 검증한다.
3. `Input` 섹션을 구현해 텍스트 입력과 값 변경 이벤트를 검증한다.
4. `Collection` 섹션을 구현해 스크롤과 재사용 엔트리 갱신을 검증한다.
5. `Advanced` 섹션을 구현해 드래그 앤 드롭과 포커스 표시를 붙인다.
6. 마지막으로 전용 상호작용 액터, 서브시스템 팝업 스타일 분기, `BasicMap` 배치를 연결한다.

## 완료 판정 기준
1. 전용 상호작용 액터 1종으로 통합 팝업을 열 수 있다.
2. 네 개의 섹션을 마우스와 키보드로 모두 전환할 수 있다.
3. 버튼 클릭, 입력값 변경, 리스트 갱신, 드래그 결과가 상태 텍스트에 즉시 반영된다.
4. 리셋 후 모든 섹션이 예측 가능한 기본 상태로 돌아간다.
5. 팝업 닫기 후 입력 모드와 마우스 커서 상태가 정상 복구된다.
6. 기존 메시지/스크롤/듀얼 타일 팝업 테스트와 충돌 없이 공존한다.
