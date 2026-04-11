# 상호작용 통합 UI 플레이그라운드 팝업 구현 체크리스트

## 문서 목적
- 이 문서는 [interaction_ui_playground_popup_plan.md](./interaction_ui_playground_popup_plan.md)를 실제 구현 작업으로 옮길 때 필요한 체크리스트와 권장 작업 순서를 정의한다.
- 목표는 설계 검토 문서를 실제 C++/WBP/애셋 빌더/맵 배치 작업 단위로 분해하는 것이다.
- 이 문서 자체는 구현을 수행하지 않는다.

## 전제
- 기존 메시지 팝업, 스크롤 메시지 팝업, 듀얼 타일 이동 팝업은 유지한다.
- 이번 작업은 기존 상호작용 팝업 체계 위에 `UI Playground` 팝업을 `추가`하는 방식으로 진행한다.
- WBP 생성 및 갱신은 기존 공용 interaction asset builder와 commandlet 경로를 재사용한다.
- `_C` 클래스 경로가 아니라 실제 WBP 애셋 경로를 기준 자산으로 다룬다.

## 최종 산출물 목표
- 런타임
  - `UIPlayground` 팝업 스타일 분기
  - `UI Playground` 팝업 위젯 클래스
  - 리스트 엔트리 위젯 클래스
  - 드래그용 타일 엔트리 위젯 클래스
  - 필요 시 전용 payload / item 데이터 클래스
  - 전용 상호작용 액터 클래스
- 에디터/애셋
  - `WBP_InteractionUIPlaygroundPopup`
  - `WBP_InteractionUIPlaygroundListEntry`
  - `WBP_InteractionUIPlaygroundTileEntry`
  - `BP_Interactable_WoodenSignUIPlaygroundPopup`
- 맵
  - `BasicMap` 테스트 배치

## 구현 원칙
- 먼저 `공통 분기와 빈 뼈대`를 만들고, 그 위에 섹션 기능을 순서대로 쌓는다.
- `Basic` 페이지를 먼저 끝내고, 그 다음 `Input`, `Collection`, `Advanced` 순으로 확장한다.
- 드래그 앤 드롭과 포커스 네비게이션은 마지막 단계에 붙인다.
- 한 번에 다 구현하지 말고, 팝업이 열리는 최소 경로를 먼저 만든 다음 섹션별로 확장한다.

## 권장 작업 순서

### 1단계. 타입과 경로 상수 추가
- [ ] `ECodexInteractionPopupStyle`에 `UIPlayground` 값을 추가한다.
- [ ] 필요 시 `ECodexUIPlaygroundSection` enum을 새로 정의한다.
- [ ] `CodexInteractionAssetPaths`에 아래 항목을 추가한다.
  - [ ] `UIPlaygroundPopupWidgetName`
  - [ ] `UIPlaygroundListEntryWidgetName`
  - [ ] `UIPlaygroundTileEntryWidgetName`
  - [ ] `InteractableWoodenSignUIPlaygroundPopupName`
  - [ ] 대응하는 `ObjectPath`
- [ ] 기존 코드와 이름 충돌이 없는지 확인한다.

### 2단계. 런타임 최소 팝업 경로 연결
- [ ] `UCodexInteractionSubsystem`에서 `PopupStyle == UIPlayground` 분기를 추가한다.
- [ ] `ResolvePopupWidgetClass()`가 `WBP_InteractionUIPlaygroundPopup`을 찾을 수 있게 한다.
- [ ] 전용 팝업 위젯 클래스 `UCodexInteractionUIPlaygroundPopupWidget`의 헤더/CPP를 추가한다.
- [ ] 이 시점의 목표는 섹션 구현이 없어도 팝업이 열리고 닫히는 최소 경로를 만드는 것이다.
- [ ] `ApplyPopupInputMode()`와 닫기 후 입력 복구가 기존 팝업과 동일하게 동작하는지 확인한다.

### 3단계. 전용 상호작용 액터 연결
- [ ] `ACodexUIPlaygroundPopupInteractableActor`를 추가한다.
- [ ] 부모는 `ACodexPopupInteractableActor`를 기본 권장안으로 둔다.
- [ ] `GetPopupStyle()`는 `UIPlayground`를 반환하게 한다.
- [ ] 기본 타이틀과 prompt text를 설정한다.
- [ ] 필요 시 playground 초기 상태를 채울 전용 payload 또는 샘플 데이터 주입 함수를 만든다.

### 4단계. 전용 데이터 구조 추가
- [ ] playground 전용 요청 데이터 구조를 정의한다.
- [ ] playground 전용 결과 데이터 구조를 정의한다.
- [ ] 리스트 항목용 데이터 구조를 정의한다.
- [ ] 드래그 항목용 데이터 구조를 정의한다.
- [ ] 공통 `FCodexInteractionPopupRequest`를 과도하게 비대하게 만들지 않는지 확인한다.
- [ ] payload가 `UObject`일지 `UStruct`일지 결정한다.
  - 권장: 런타임 바인딩과 확장을 생각하면 `UObject` payload + `UObject` item이 다루기 쉽다.

### 5단계. 팝업 WBP 최소 뼈대 생성
- [ ] `WBP_InteractionUIPlaygroundPopup` 생성 로직을 asset builder에 추가한다.
- [ ] 공통 패널 구조를 먼저 만든다.
  - [ ] `RootCanvas`
  - [ ] `Overlay_ScreenRoot`
  - [ ] `SizeBox_PopupFrame`
  - [ ] `BackgroundBlur_Panel`
  - [ ] `Border_TintPanel`
  - [ ] `VerticalBox_Content`
  - [ ] `HorizontalBox_TitleBar`
  - [ ] `HorizontalBox_Body`
  - [ ] `Border_StatusBar`
- [ ] 좌측 탭 버튼과 우측 `WidgetSwitcher`를 배치한다.
- [ ] `BTN_ResetAll`, `BTN_Close`, `TXT_Status` 바인딩 대상이 실제 위젯에 존재하는지 확인한다.
- [ ] 이 단계에서는 페이지 내부가 비어 있어도 괜찮다.

### 6단계. `Basic` 페이지 구현
- [ ] 버튼 샘플 위젯을 배치한다.
- [ ] 제목/본문/보조 텍스트를 배치한다.
- [ ] 이미지 샘플을 배치한다.
- [ ] 상태 바 텍스트 갱신 함수를 붙인다.
- [ ] 버튼 클릭만으로 상태 바가 바뀌는지 확인한다.
- [ ] `Disabled` 버튼이 실제로 비활성 상태로 보이는지 확인한다.

### 7단계. `Input` 페이지 구현
- [ ] `EditableTextBox`
- [ ] `MultiLineEditableTextBox`
- [ ] `CheckBox`
- [ ] `Slider`
- [ ] `SpinBox`
- [ ] `ComboBoxString`
- [ ] 각 위젯 변경 시 요약 텍스트나 상태 바가 즉시 갱신되도록 한다.
- [ ] 기본값과 리셋값을 문서 기준으로 고정한다.
- [ ] 텍스트 입력 중 포커스 이동과 닫기 입력 충돌이 없는지 확인한다.

### 8단계. `Collection` 페이지 구현
- [ ] `ScrollBox` 기반 샘플 블록을 만든다.
- [ ] `ListView` 또는 `TileView` 중 최소 1개를 구현한다.
- [ ] 전용 엔트리 WBP `WBP_InteractionUIPlaygroundListEntry` 생성 로직을 추가한다.
- [ ] 전용 엔트리 C++ 클래스 `UCodexInteractionUIPlaygroundListEntryWidget`를 추가한다.
- [ ] `추가`, `삭제`, `선택 변경` 버튼을 연결한다.
- [ ] 항목 상태 변경 뒤 리스트가 실제로 새로 그려지는지 확인한다.
- [ ] 초기 항목 수가 한 화면을 넘도록 기본 샘플 데이터를 넣는다.

### 9단계. `Advanced` 페이지 구현
- [ ] 드래그 소스용 샘플 항목을 만든다.
- [ ] 드롭 타깃 슬롯 또는 영역을 만든다.
- [ ] 전용 엔트리 WBP `WBP_InteractionUIPlaygroundTileEntry` 생성 로직을 추가한다.
- [ ] 전용 엔트리 C++ 클래스 `UCodexInteractionUIPlaygroundTileEntryWidget`를 추가한다.
- [ ] `UCodexUIPlaygroundDragDropOperation`을 추가한다.
- [ ] 드래그 상태, 드롭 성공/실패, 현재 포커스를 보여주는 디버그 텍스트를 연결한다.
- [ ] 드롭 가능 강조 표시가 남지 않도록 초기화 경로를 넣는다.

### 10단계. 탭 전환과 포커스 규칙 마무리
- [ ] 탭 버튼 클릭 시 `WidgetSwitcher`가 즉시 바뀌는지 확인한다.
- [ ] 키보드만으로 탭 버튼 사이 이동이 가능한지 확인한다.
- [ ] 각 페이지 진입 시 기본 포커스 대상이 명확한지 확인한다.
- [ ] `Tab`, `Shift+Tab`, 방향키, 게임패드 D-pad 이동 규칙을 정리한다.
- [ ] 텍스트 입력 중과 아닌 경우 입력 우선순위가 분리되는지 확인한다.

### 11단계. 전체 리셋과 결과 회수
- [ ] `BTN_ResetAll`이 모든 페이지 상태를 기본값으로 되돌리는지 확인한다.
- [ ] 현재 활성 섹션도 적절히 초기화되는지 확인한다.
- [ ] 팝업 종료 시 최소 결과가 회수되는지 확인한다.
  - [ ] 마지막 활성 섹션
  - [ ] 입력값
  - [ ] 체크 상태
  - [ ] 슬라이더/스핀 값
  - [ ] 콤보 선택
  - [ ] 리스트/드래그 최종 상태

### 12단계. 에디터 빌더와 commandlet 연결
- [ ] asset builder에 새 WBP 3종 생성 로직을 추가한다.
- [ ] 테스트용 BP 생성 로직을 추가한다.
- [ ] 기존 commandlet이 새 자산도 함께 생성/갱신하도록 연결한다.
- [ ] 버전 문자열을 새로 잡는다.
- [ ] 동일 레시피일 때 재저장하지 않는 정책이 유지되는지 확인한다.

### 13단계. `BasicMap` 배치
- [ ] `BP_Interactable_WoodenSignUIPlaygroundPopup` 배치를 추가한다.
- [ ] 기존 다른 팝업 테스트 액터와 구분되는 위치에 둔다.
- [ ] 플레이어 접근 시 포커스 우선순위가 엉키지 않는 거리인지 확인한다.

### 14단계. 최종 검증
- [ ] 팝업이 열린다.
- [ ] 탭 전환이 된다.
- [ ] 버튼 클릭 이벤트가 보인다.
- [ ] 입력 컨트롤이 동작한다.
- [ ] 리스트가 스크롤되고 갱신된다.
- [ ] 드래그 앤 드롭이 된다.
- [ ] 포커스 이동이 된다.
- [ ] 리셋이 된다.
- [ ] 닫기 후 입력 모드가 정상 복구된다.

## 실제 수정 파일 권장 목록

### 1. 공통 타입/경로
- `Source/CodexUMG/Public/Interaction/CodexInteractionTypes.h`
- `Source/CodexUMG/Public/Interaction/CodexInteractionAssetPaths.h`

### 2. 런타임 서브시스템
- `Source/CodexUMG/Public/Interaction/CodexInteractionSubsystem.h`
- `Source/CodexUMG/Private/Interaction/CodexInteractionSubsystem.cpp`

### 3. 새 런타임 클래스
- `Source/CodexUMG/Public/Interaction/CodexInteractionUIPlaygroundPopupWidget.h`
- `Source/CodexUMG/Private/Interaction/CodexInteractionUIPlaygroundPopupWidget.cpp`
- `Source/CodexUMG/Public/Interaction/CodexInteractionUIPlaygroundListEntryWidget.h`
- `Source/CodexUMG/Private/Interaction/CodexInteractionUIPlaygroundListEntryWidget.cpp`
- `Source/CodexUMG/Public/Interaction/CodexInteractionUIPlaygroundTileEntryWidget.h`
- `Source/CodexUMG/Private/Interaction/CodexInteractionUIPlaygroundTileEntryWidget.cpp`
- `Source/CodexUMG/Public/Interaction/CodexUIPlaygroundPopupInteractableActor.h`
- `Source/CodexUMG/Private/Interaction/CodexUIPlaygroundPopupInteractableActor.cpp`
- 필요 시 payload / item 데이터 클래스 헤더/CPP

### 4. 에디터 애셋 빌더
- `Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp`

### 5. commandlet
- 기존 공용 commandlet 사용 시 직접 수정이 필요 없는지 먼저 확인한다.
- 필요 시
  - `Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexInteractionAssetBuildCommandlet.cpp`

## 실제 구현 순서에서 중요한 의사결정

### 1. payload 표현 방식
- 선택지 A: 공통 popup request에 필드 추가
  - 장점: 분기 수가 적다.
  - 단점: 구조체가 빠르게 비대해진다.
- 선택지 B: 전용 payload 객체 분리
  - 장점: playground 전용 상태를 격리할 수 있다.
  - 단점: 런타임 캐스팅과 소유 관계를 신경 써야 한다.
- 권장: `전용 payload 객체 분리`

### 2. `Collection` 페이지의 리스트 종류
- 선택지 A: `ListView`만 먼저 구현
- 선택지 B: `ListView`와 `TileView`를 같이 구현
- 권장: `ListView` 먼저 구현 후 여유가 있으면 `TileView` 추가

### 3. `Advanced` 페이지의 드래그 구조
- 선택지 A: 단순한 소스-타깃 박스 기반 드롭
- 선택지 B: 슬롯 기반 다중 드롭
- 권장: 먼저 `단순한 소스-타깃 박스 기반 드롭`, 이후 필요 시 슬롯형으로 확장

## 빠른 1차 완성 기준
- `UIPlayground` 팝업 스타일 분기가 추가되어 있다.
- 전용 액터로 팝업이 열린다.
- `Basic`와 `Input` 페이지가 구현되어 있다.
- `Collection`은 최소 `ListView` 1종이 동작한다.
- `Advanced`는 최소 드래그 샘플 1종이 동작한다.
- 리셋과 닫기가 정상 동작한다.

## 2차 확장 기준
- `Collection`에 `TileView` 추가
- `Advanced`에 슬롯형 드롭 확장
- 포커스 디버그 가시성 강화
- 결과 데이터 정교화
- 학습 가이드 문서 추가

## 작업 시작 추천 포인트
1. 타입/경로 상수 추가
2. 런타임 팝업 분기와 빈 위젯 클래스 추가
3. WBP 껍데기 생성
4. `Basic` 페이지 연결
5. 이후 섹션 확장

## 완료 후 확인할 질문
- 이 팝업이 기존 개별 팝업의 회귀 테스트 허브 역할을 실제로 해주는가
- `Input` 페이지에서 포커스와 닫기 입력 충돌이 없는가
- `Collection` 페이지에서 엔트리 갱신 누락이 없는가
- `Advanced` 페이지에서 드롭 강조 잔상이 남지 않는가
- 기존 메시지/스크롤/듀얼 타일 팝업과 공존 시 충돌이 없는가
