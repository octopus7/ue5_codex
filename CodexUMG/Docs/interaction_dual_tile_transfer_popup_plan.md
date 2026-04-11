# 상호작용 좌우 타일 이동 팝업 구현 계획

## 문서 목적
- 이 문서는 좌우 2개의 스크롤 가능한 타일 뷰를 가진 숫자 이동 팝업의 구현 계획만 정의한다.
- 실제 C++ 수정, WBP 생성, 애셋 생성, 커맨드렛 실행, 레벨 배치는 이 문서에서 수행하지 않는다.
- WBP 생성/참조 규칙은 반드시 [wbp_asset_work_guidelines.md](./wbp_asset_work_guidelines.md)를 따른다.

## 고정 규칙
- 팝업의 실제 위젯 트리는 런타임 코드가 아니라 실제 `WBP_*` 애셋의 `WidgetTree`에 존재해야 한다.
- 다른 시스템이 이 팝업을 사용할 때도 `_C` 클래스 경로가 아니라 WBP 애셋 경로를 기준 참조로 삼는다.
- WBP 및 관련 UI 애셋 생성/갱신 로직은 에디터 모듈 코드에 둔다.
- 실행 진입은 `Commandlet`로만 수행한다.
- 현재 프로젝트의 `UnrealEditor.exe`가 실행 중이라 충돌이 나면 우회하지 말고 즉시 중단 후 사용자에게 알린다.

## 목표 상태
1. 화면 중앙에 모달 팝업이 열린다.
2. 팝업 본문에는 좌우로 나란히 배치된 2개의 스크롤 가능한 타일 뷰가 있다.
3. 각 타일 뷰는 한 화면 기준으로 `4열 x 10행`이 보이는 크기를 가진다.
4. 각 타일 항목은 정사각형이며 중앙에 `1~99` 숫자를 표시한다.
5. 타일 배경은 파스텔톤의 화사한 단색 계열이며, 세로 그라데이션과 약한 라운딩이 들어간 텍스처에 컬러를 입혀 사용한다.
6. 좌측 타일을 우측으로, 우측 타일을 좌측으로 드래그 앤 드롭으로 옮길 수 있다.
7. 팝업에는 `닫기`, `좌 타일 추가`, `좌 타일 지우기`, `우 타일 추가`, `우 타일 지우기` 버튼이 존재한다.

## 범위
- 이 문서는 팝업 UI와 팝업 내부 동작만 다룬다.
- 어떤 액터, 상호작용 대상, 퀘스트 시스템, 인벤토리 시스템이 이 팝업을 여는지는 후속 문서나 호출 주체에서 결정한다.
- 1차 구현 범위에서는 좌우 패널 간 이동과 선택/추가/삭제에 집중한다.
- 1차 구현 범위에서는 같은 패널 내부에서의 순서 재배치까지는 필수 요구사항으로 두지 않는다.

## 사용자 기능 요구사항

### 1. 팝업 레이아웃
- 상단에는 타이틀과 `닫기` 버튼이 있는 타이틀바가 있다.
- 본문에는 좌측 패널과 우측 패널이 동일한 비중으로 배치된다.
- 각 패널 상단에는 패널 제목과 `타일 추가`, `타일 지우기` 버튼이 있다.
- 각 패널 본문에는 개별 스크롤이 가능한 `TileView`가 있다.

### 2. 타일 표시
- 각 타일은 정사각형이다.
- 숫자 표시는 중앙 정렬된 큰 텍스트다.
- 숫자 범위는 `1~99`다.
- 같은 숫자는 양쪽을 합쳐 한 번만 존재하는 구조를 1차 권장안으로 둔다.
- 모든 숫자가 사용 중이면 `타일 추가`는 비활성화한다.

### 3. 추가/삭제 동작
- `좌 타일 추가`는 좌측 리스트에 새 타일을 추가한다.
- `우 타일 추가`는 우측 리스트에 새 타일을 추가한다.
- 새 타일 번호는 전체 풀에서 아직 사용되지 않은 가장 작은 숫자를 우선 사용한다.
- `좌 타일 지우기`는 현재 좌측에서 선택된 타일을 제거한다.
- `우 타일 지우기`는 현재 우측에서 선택된 타일을 제거한다.
- 선택된 타일이 없으면 해당 패널의 `타일 지우기` 버튼은 비활성화한다.

### 4. 드래그 앤 드롭
- 좌측 타일은 우측 타일 뷰로 드롭할 수 있다.
- 우측 타일은 좌측 타일 뷰로 드롭할 수 있다.
- 유효한 반대편 패널에 드롭되면 원본 배열에서 제거되고 대상 배열에 추가된다.
- 유효하지 않은 위치에 드롭하면 이동은 취소되고 원래 패널에 유지된다.
- 1차 구현에서는 같은 패널 내부 재정렬은 생략 가능하다.

### 5. 닫기
- `닫기` 버튼으로 팝업을 닫을 수 있다.
- `Esc` 또는 별도 단축키 닫기 지원 여부는 후속 요구사항이 없으면 1차 범위에 포함하지 않는다.

## 데이터 구조 권장안

### 1. 팝업 요청 데이터
- 권장 구조체 예시
  - `FGuid RequestId`
  - `FText Title`
  - `TArray<int32> LeftNumbers`
  - `TArray<int32> RightNumbers`
  - `bool bAllowDuplicateNumbers = false`
- 목적
  - 호출 주체가 초기 좌우 숫자 상태를 넘길 수 있게 한다.

### 2. 팝업 결과 데이터
- 권장 구조체 예시
  - `FGuid RequestId`
  - `TArray<int32> LeftNumbers`
  - `TArray<int32> RightNumbers`
  - `bool bWasClosed`
- 목적
  - 닫는 시점의 좌우 최종 상태를 호출 주체가 받을 수 있게 한다.

### 3. 타일 항목 데이터
- 권장 `UObject` 또는 `UStruct` 필드
  - `int32 Number`
  - `FLinearColor TintColor`
  - `bool bIsSelected`
- 목적
  - `TileView` 항목 데이터와 시각 상태를 분리한다.

## WBP 애셋 구성 계획

### 1. 실제 WBP 애셋
- 팝업 본체
  - `/Game/UI/Interaction/WBP_InteractionDualTileTransferPopup`
- 타일 항목 엔트리
  - `/Game/UI/Interaction/WBP_InteractionDualTileTransferTileEntry`

### 2. 실제 텍스처 애셋
- 타일 배경용 베이스 텍스처
  - `/Game/UI/Interaction/T_InteractionTileRoundedVerticalGradient`
- 이 텍스처는 외부 이미지 임포트가 아니라 에디터 모듈 코드로 생성한다.
- 배경 모서리는 투명 처리되어 라운딩된 사각형만 남게 한다.
- 내부는 위아래 밝기 차가 있는 세로 그라데이션으로 만든다.
- 개별 타일은 같은 텍스처를 공유하고 `Brush Tint` 또는 동등한 방식으로 파스텔 컬러를 입힌다.

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
  - `BTN_Close`
- `HorizontalBox_Body`
  - `VerticalBox_LeftPanel`
    - `HorizontalBox_LeftHeader`
      - `TXT_LeftTitle`
      - `BTN_LeftAdd`
      - `BTN_LeftRemove`
    - `TileView_Left`
  - `VerticalBox_RightPanel`
    - `HorizontalBox_RightHeader`
      - `TXT_RightTitle`
      - `BTN_RightAdd`
      - `BTN_RightRemove`
    - `TileView_Right`

### 2. 타일 엔트리
- `Border_TileRoot`
- `Overlay_TileContent`
  - `IMG_TileBackground`
  - `TXT_Number`
  - 필요 시 선택 상태 표시용 `Border_SelectedOutline`

### 3. BindWidget 대상 권장안
- 팝업 본체
  - `TXT_Title`
  - `BTN_Close`
  - `BTN_LeftAdd`
  - `BTN_LeftRemove`
  - `BTN_RightAdd`
  - `BTN_RightRemove`
  - `TileView_Left`
  - `TileView_Right`
- 타일 엔트리
  - `IMG_TileBackground`
  - `TXT_Number`
  - `Border_SelectedOutline`

## 시각 스타일 계획

### 1. 팝업 외형
- 기본 패널 외형은 기존 메시지 팝업 계열과 최대한 유사하게 가져간다.
- 중앙 정렬 모달, 배경 블러, 낮은 불투명도의 색 오버레이, 라운딩된 패널 구조를 유지한다.
- 이 문서의 핵심 차별점은 내부 콘텐츠가 단일 메시지가 아니라 좌우 2개의 타일 패널이라는 점이다.

### 2. 타일 배경
- 타일 배경은 단색 평면이 아니라 세로 그라데이션이 들어간 라운드 텍스처를 사용한다.
- 텍스처 자체는 무채색 또는 중립 밝기 기반으로 생성한다.
- 실제 색감은 항목별 `Tint`로 입혀서 파스텔 톤을 만든다.
- 1차 권장 팔레트는 8~12개의 밝은 파스텔 색을 순환 사용한다.
- 예시 계열
  - 연하늘
  - 연민트
  - 연노랑
  - 연핑크
  - 연살구
  - 연라임

### 3. 숫자 텍스트
- 숫자는 중앙 정렬한다.
- 가독성을 위해 흰색 또는 어두운 회색 계열을 쓰되 배경과 충분한 대비를 확보한다.
- `1~99` 두 자리 수까지 안정적으로 들어가는 크기로 맞춘다.

### 4. 타일 뷰 크기
- 각 타일 뷰는 한 번에 `4열 x 10행`이 보이는 높이를 목표로 잡는다.
- 40개를 초과하면 각 패널이 독립적으로 스크롤된다.
- 타일 크기는 정사각형 유지가 우선이며, 패널 폭에 맞춰 4열이 균등 배치되도록 계산한다.

## 런타임 구조 권장안

### 1. 팝업 본체 C++ 클래스
- 권장 이름
  - `UCodexInteractionDualTileTransferPopupWidget`
- 책임
  - 요청 데이터 적용
  - 좌우 `TileView` 데이터 공급
  - 선택 상태 갱신
  - 추가/삭제 버튼 처리
  - 드래그 앤 드롭 수락 처리
  - 닫기 결과 전달

### 2. 타일 엔트리 C++ 클래스
- 권장 이름
  - `UCodexInteractionDualTileTransferTileEntryWidget`
- 책임
  - 숫자 표시
  - 배경 틴트 반영
  - 선택 상태 표시
  - 드래그 시작
  - 드롭 비주얼 제공

### 3. 드래그 앤 드롭 오퍼레이션
- 권장 이름
  - `UCodexTileTransferDragDropOperation`
- 필드 예시
  - `int32 Number`
  - `bool bSourceIsLeft`
  - `int32 SourceIndex`
- 목적
  - 어느 패널의 어떤 숫자가 드래그 중인지 명확히 전달한다.

## 동작 흐름 권장안
1. 호출 주체가 팝업 요청 데이터를 만든다.
2. 서브시스템 또는 팝업 호스트가 WBP 애셋 경로를 기준으로 팝업을 연다.
3. 팝업 본체는 좌우 숫자 배열을 내부 데이터 객체 배열로 변환한다.
4. 각 `TileView`는 해당 배열을 표시한다.
5. 사용자가 타일을 선택하면 해당 패널의 삭제 버튼이 활성화된다.
6. 사용자가 `타일 추가`를 누르면 사용되지 않은 다음 숫자를 생성해 해당 패널에 넣는다.
7. 사용자가 타일을 반대편 패널로 드래그 앤 드롭하면 대상 배열로 이동한다.
8. 사용자가 `닫기`를 누르면 좌우 최종 배열이 결과 데이터로 묶여 호출 주체에 전달된다.

## 에디터 모듈 / 커맨드렛 계획

### 1. 생성 대상
- `WBP_InteractionDualTileTransferPopup`
- `WBP_InteractionDualTileTransferTileEntry`
- `T_InteractionTileRoundedVerticalGradient`

### 2. 생성 책임
- 에디터 모듈 코드가 실제 WBP와 텍스처 애셋을 생성/갱신한다.
- 커맨드렛이 그 생성 로직의 유일한 실행 진입점이 된다.

### 3. 텍스처 생성 방식
- `UTexture2D`를 코드로 직접 생성한다.
- 투명 배경 위에 라운드 사각형 마스크를 만든다.
- 상단이 조금 더 밝고 하단이 조금 더 진한 세로 그라데이션을 준다.
- UI 용도에 맞게 `TextureGroup=UI` 계열 설정과 mip 최소화 설정을 적용한다.

### 4. 충돌 처리
- 프로젝트 에디터가 실행 중이면 작업을 즉시 중단한다.
- 띄워진 에디터를 유지한 채 다른 경로로 우회 실행하지 않는다.
- 실패 사실과 원인을 사용자에게 그대로 알린다.

## 산출물

### 1. 문서 기준 런타임 산출물
- `UCodexInteractionDualTileTransferPopupWidget`
- `UCodexInteractionDualTileTransferTileEntryWidget`
- `UCodexTileTransferDragDropOperation`
- 팝업 요청/응답 구조체

### 2. 문서 기준 콘텐츠 산출물
- `/Game/UI/Interaction/WBP_InteractionDualTileTransferPopup`
- `/Game/UI/Interaction/WBP_InteractionDualTileTransferTileEntry`
- `/Game/UI/Interaction/T_InteractionTileRoundedVerticalGradient`

## 비범위
- 같은 패널 내부 순서 재정렬
- 게임패드 전용 포커스 네비게이션 세부 튜닝
- 사운드, 이펙트, 애니메이션 세부 연출
- 외부 상호작용 대상이나 레벨 배치 규칙
- 네트워크 동기화

## 완료 기준
- 팝업 본체와 타일 엔트리 모두 실제 WBP 애셋으로 존재한다.
- 타일 배경 텍스처도 실제 애셋으로 존재하며 외부 파일 임포트가 아니다.
- 좌우 2개의 타일 뷰가 한 화면 기준 `4열 x 10행`을 표시한다.
- 타일은 정사각형이며 숫자 `1~99`를 보여준다.
- 좌우 패널 모두 `타일 추가`, `타일 지우기` 버튼을 가진다.
- `닫기` 버튼이 존재한다.
- 좌우 패널 간 드래그 앤 드롭 이동이 가능하다.
- WBP 생성/갱신은 에디터 모듈 + 커맨드렛 경로로만 수행된다.
- 에디터 실행 충돌 시 우회 없이 중단한다.

## 검증 체크리스트
1. `WBP_InteractionDualTileTransferPopup`가 실제 애셋으로 존재한다.
2. `WBP_InteractionDualTileTransferTileEntry`가 실제 애셋으로 존재한다.
3. `T_InteractionTileRoundedVerticalGradient`가 실제 애셋으로 존재한다.
4. 팝업 위젯 트리가 런타임 코드 조립이 아니라 WBP 내부에 존재한다.
5. 각 패널에 `TileView`가 실제로 존재한다.
6. 좌측과 우측 패널이 동시에 보인다.
7. 각 패널은 한 화면 기준 `4열 x 10행`을 표시한다.
8. 40개를 넘는 항목은 각 패널에서 독립 스크롤된다.
9. 타일이 정사각형으로 보인다.
10. 숫자 표시가 `1~99` 범위로 동작한다.
11. 타일 배경이 파스텔 계열 틴트 + 세로 그라데이션 + 약한 라운딩으로 보인다.
12. 좌우 패널의 `타일 추가`, `타일 지우기`, 상단 `닫기` 버튼이 모두 존재한다.
13. 좌우 패널 간 드래그 앤 드롭 이동이 된다.
14. WBP 참조가 `_C` 클래스 기준이 아니라 WBP 애셋 기준으로 설계되어 있다.
15. 에디터가 실행 중이면 커맨드렛이 우회 없이 중단된다.
