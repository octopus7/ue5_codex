# 상호작용 스크롤 메시지 팝업 구현 계획

## 문서 목적
- 이 문서는 스크롤 가능한 긴 텍스트를 보여주는 UMG 팝업 기능의 구현 계획만 정의한다.
- 실제 C++ 수정, 애셋 생성, 커맨드렛 실행, 맵 배치 작업은 이 문서에서 수행하지 않는다.
- 이미 구현된 단순 메시지 팝업 흐름을 최대한 재사용하면서, 긴 메시지 본문과 스크롤 영역이 필요한 변형 팝업을 목표로 한다.

## 선행 고정 규칙
- WBP 작업 방식은 반드시 [wbp_asset_work_guidelines.md](./wbp_asset_work_guidelines.md)를 따른다.
- 팝업 UI는 코드에서 위젯 트리를 조립하지 않고 실제 `Widget Blueprint` 애셋으로 존재해야 한다.
- WBP 생성/갱신 로직은 `CodexUMGBootstrapEditor` 같은 에디터 모듈 코드에 두고, 실행 진입점은 `Commandlet`로 고정한다.
- 커맨드렛 실행 중 현재 프로젝트의 `UnrealEditor.exe`가 열려 있어 충돌하면 우회하지 말고 즉시 중단 후 사용자에게 알린다.

## 기존 단순 팝업과의 관계
- 이 팝업은 기존 [interaction_message_popup_plan.md](./interaction_message_popup_plan.md)의 구조를 기반으로 하는 파생형으로 본다.
- 표시 위치, 중앙 패널 개념, blur 기반 패널, viewport UMG 사용, 상호작용 서브시스템을 통한 요청/결과 흐름은 기존 단순 팝업과 동일 계열로 유지한다.
- 이번 변형의 핵심 차이는 아래 네 가지다.
  - 본문이 길어질 수 있으므로 `ScrollBox` 기반 스크롤 영역이 필요하다.
  - 본문 상단에 노란 스마일 아이콘이 추가된다.
  - 하단 버튼은 `OK` 단일 버튼으로 고정한다.
  - 기존 하늘색 저투명 틴트 대신 노란색 계열 저투명 틴트를 사용한다.
  - 스마일 아이콘은 외부 이미지 임포트가 아니라 에디터 모듈 코드로 생성한 투명 배경 텍스처 애셋을 사용한다.

## 목표 상태
1. 플레이어가 특정 상호작용 대상에 접근하면 상호작용 이름은 `읽기`로 표시되고, 이를 사용하면 화면 중앙의 제한된 영역에 스크롤 메시지 팝업이 열린다.
2. 팝업은 타이틀바, 노란 스마일 아이콘, 스크롤되는 긴 메시지, `OK` 버튼으로 구성된다.
3. 패널 외양은 기존 단순 팝업과 거의 동일하되, 낮은 opacity의 중첩 색상만 하늘색 계열에서 노란색 계열로 바뀐다.
4. 본문이 짧아도 동일한 레이아웃을 사용하고, 본문이 길면 스크롤 영역 내부에서만 스크롤된다.
5. 사용자가 `OK` 버튼을 누르면 팝업이 닫히고 결과가 기존 상호작용 통합 처리 흐름으로 전달된다.
6. 별도 요구가 없으므로 1차 구현에서는 `Yes/No` 분기, 추가 버튼, 별도 장식 레이아웃은 도입하지 않는다.

## 현재 프로젝트 기준 접점
- 상호작용 진입점은 [Source/CodexUMG/Public/Interaction/CodexInteractionSubsystem.h](../Source/CodexUMG/Public/Interaction/CodexInteractionSubsystem.h)의 `UCodexInteractionSubsystem`을 기준으로 삼는다.
- popup 자산 경로 상수는 [Source/CodexUMG/Public/Interaction/CodexInteractionAssetPaths.h](../Source/CodexUMG/Public/Interaction/CodexInteractionAssetPaths.h)를 기준으로 맞춘다.
- popup 요청/응답 타입은 [Source/CodexUMG/Public/Interaction/CodexInteractionTypes.h](../Source/CodexUMG/Public/Interaction/CodexInteractionTypes.h)를 기준으로 확장한다.
- 스크롤 팝업 위젯 클래스는 [Source/CodexUMG/Public/Interaction/CodexInteractionScrollMessagePopupWidget.h](../Source/CodexUMG/Public/Interaction/CodexInteractionScrollMessagePopupWidget.h)와 [Source/CodexUMG/Private/Interaction/CodexInteractionScrollMessagePopupWidget.cpp](../Source/CodexUMG/Private/Interaction/CodexInteractionScrollMessagePopupWidget.cpp)를 기준으로 확인한다.
- 스크롤 팝업용 상호작용 액터는 [Source/CodexUMG/Public/Interaction/CodexScrollMessagePopupInteractableActor.h](../Source/CodexUMG/Public/Interaction/CodexScrollMessagePopupInteractableActor.h)와 [Source/CodexUMG/Private/Interaction/CodexScrollMessagePopupInteractableActor.cpp](../Source/CodexUMG/Private/Interaction/CodexScrollMessagePopupInteractableActor.cpp)를 기준으로 확인한다.
- 기존 팝업형 상호작용 흐름과 입력 처리 확장 방식은 [interaction_message_popup_plan.md](./interaction_message_popup_plan.md)의 방향을 따른다.
- 기존 `Widget Blueprint` 생성 패턴은 [Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp](../Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp)의 흐름을 기준으로 본다.
- 플레이어 컨트롤러의 popup 입력 모드 전환, 마우스 커서 표시, 서브시스템 연결 방식은 기존 단순 팝업 구현을 재사용하는 것이 우선이다.

## 핵심 설계 원칙
- 팝업은 월드 부착형 `UWidgetComponent`가 아니라 `AddToViewport` 기반의 화면 UMG다.
- 실제 레이아웃은 `WBP_InteractionScrollMessagePopup` 같은 별도 WBP 애셋 안의 `WidgetTree`에 존재해야 한다.
- 런타임 코드는 텍스트 주입, 스크롤 초기 위치 설정, 버튼 이벤트 전달, 표시/닫기 제어만 담당한다.
- 시각 스타일은 기존 단순 팝업과 최대한 통일하고, 새로운 차이는 노란 틴트와 스마일 아이콘 정도로 제한한다.
- 버튼 구성은 `OK` 하나로 고정한다.
- 사용자에게 보이는 상호작용 이름은 `읽기`로 고정한다.
- 1차 구현에서 내부 enum이 기존 `Use`를 재사용하더라도, 노출 텍스트 또는 prompt는 반드시 `읽기`여야 한다.
- 별도 요구가 없으므로 이 문서에서는 `Space` 닫기나 타이틀바 닫기 버튼을 기본 요구사항으로 잡지 않는다.
- 기존 전역 팝업 닫기 입력이 이미 존재한다면 이 변형 팝업에서 허용할지 차단할지 명시적으로 결정해야 하며, 1차 권장안은 `OK` 버튼만 사용자 노출 종료 수단으로 두는 것이다.

## 결과 구조 권장안

### 1. 결과 타입
- 사용자 노출 결과는 기본적으로 `Ok` 하나면 충분하다.
- 다만 기존 popup 처리 흐름 호환을 위해 내부적으로 `Closed` 보조 결과를 유지하는 것은 허용할 수 있다.
- 권장 방향은 아래 둘 중 하나다.
  - 기존 popup 결과 enum을 재사용하고 이 팝업은 `Ok`만 실제로 발생시킨다.
  - 스크롤 메시지 팝업 전용 결과 enum을 두되 `Ok` 중심의 단순 구조로 유지한다.

### 2. 요청 데이터
- 권장 필드
  - `FGuid RequestId`
  - `FText Title`
  - `FText Message`
  - `TWeakObjectPtr<APlayerController> RequestingController`
  - `TWeakObjectPtr<AActor> SourceActor`
  - `TWeakObjectPtr<UCodexInteractionComponent> SourceInteractionComponent`
- 목적
  - 긴 메시지 표시와 결과 라우팅에 필요한 문맥을 하나의 요청 구조체에 묶는다.

### 3. 응답 데이터
- 권장 필드
  - `FGuid RequestId`
  - `ECodexPopupResult` 또는 전용 `ECodexScrollPopupResult`
  - `TWeakObjectPtr<APlayerController> RequestingController`
  - `TWeakObjectPtr<AActor> SourceActor`
  - `TWeakObjectPtr<UCodexInteractionComponent> SourceInteractionComponent`
- 목적
  - `UCodexInteractionSubsystem`이 먼저 결과를 받고 후속 처리를 이어갈 수 있게 한다.

## 권장 런타임 구조

### 1. `UCodexInteractionSubsystem`
- 책임
  - 상호작용 요청 수신
  - 현재 대상이 스크롤 메시지 팝업 대상인지 판별
  - 팝업 요청 생성
  - WBP 기반 팝업 생성 및 표시
  - `OK` 결과 수신
  - 팝업 종료 후 후속 처리 라우팅
- 권장 방향
  - 기존 단순 팝업 오픈/닫기 경로를 최대한 재사용한다.
  - 동일 시점에 하나의 modal popup만 유지하는 규칙을 그대로 따른다.

### 2. `UCodexInteractionScrollMessagePopupWidget`
- 역할
  - 실제 `WBP_InteractionScrollMessagePopup`의 C++ 베이스 클래스
- 책임
  - 타이틀 텍스트 반영
  - 긴 메시지 텍스트 반영
  - 팝업 표시 직후 스크롤 위치를 맨 위로 초기화
  - `OK` 버튼 클릭 결과를 서브시스템에 전달
- 비책임
  - 위젯 트리 생성
  - 상호작용 대상 판별
  - 결과 직접 소비

### 3. 상호작용 대상 액터
- 기존 popup interactable 계열을 재사용하는 것이 우선이다.
- 다만 1차 구현 기준으로는 단순 메시지 팝업용 BP와 동일한 BP를 공유하지 않고, 스크롤 메시지 팝업 전용 상호작용 BP를 별도로 만든다.
- 즉, 공용 부모 클래스나 공용 C++ 기반은 재사용할 수 있지만, 레벨에 배치하는 최종 BP 애셋은 별도여야 한다.
- 필요한 경우 전용 데이터만 추가한다.
  - `PopupTitle`
  - `PopupLongMessage`
- 권장 BP 이름 예시는 `BP_Interactable_WoodenSignScrollPopup`이다.
- 이 BP는 단순 메시지 팝업용 `BP_Interactable_WoodenSignPopup`과는 별개 애셋으로 본다.
- 레벨에는 단순 메시지 팝업용 액터와 별도의 인스턴스로 배치한다.
- 팝업을 여는 상호작용 prompt 기본값은 `읽기`로 둔다.

## 산출물

### 런타임 C++ 산출물
- `UCodexInteractionScrollMessagePopupWidget`
- 스크롤 메시지 팝업 요청/응답 구조체
- `UCodexInteractionSubsystem` 확장
  - 스크롤 메시지 팝업 열기
  - 결과 수신
  - 기존 modal popup 상태와 충돌하지 않도록 관리

### 에디터 / 커맨드렛 산출물
- `CodexUMGBootstrapEditor` 내부 스크롤 팝업 WBP 생성/갱신 코드
- 필요 시 기존 상호작용 애셋 빌드 커맨드렛 확장

### 실제 콘텐츠 산출물
- `/Game/UI/Interaction/WBP_InteractionScrollMessagePopup`
- `/Game/Blueprints/Interaction/BP_Interactable_WoodenSignScrollPopup`
  - 단순 메시지 팝업용 `BP_Interactable_WoodenSignPopup`과 별도 애셋
- 팝업에 사용되는 노란 스마일 아이콘 텍스처 애셋
  - 외부 파일 임포트가 아니라 에디터 모듈 코드 + commandlet로 생성
  - canonical asset path는 `/Game/UI/Interaction/T_InteractionSmileYellow`로 고정한다.
  - legacy alias `T_InteractionSmileIcon`이 문서에 남아 있더라도 비교 시에는 `T_InteractionSmileYellow`로 정규화한다.
- 필요 시 `/Game/Maps/BasicMap` 또는 테스트 맵에 스크롤 팝업용 BP를 별도 배치

## WBP 구조 명세

### 실제 애셋 이름
- `/Game/UI/Interaction/WBP_InteractionScrollMessagePopup`

### 구조 규칙
- 아래 위젯 구조는 실제 WBP 애셋 `WidgetTree` 안에 존재해야 한다.
- 런타임 C++은 `BindWidget` 또는 동등한 방식으로 이미 존재하는 위젯을 제어만 한다.

### 권장 위젯 트리
- `RootCanvas`
  - 전체 화면 루트
- `Overlay_ScreenRoot`
  - 중앙 정렬 레이어
- `SizeBox_PopupFrame`
  - 최대 폭/높이 제한
- `Overlay_PanelRoot`
  - blur, 틴트, 콘텐츠 중첩
- `BackgroundBlur_Panel`
  - 패널 영역 blur
- `Border_YellowTintPanel`
  - 낮은 opacity 노란 틴트
- `VerticalBox_Content`
  - 타이틀바 / 아이콘 / 스크롤 영역 / 버튼 배치
- `HorizontalBox_TitleBar`
  - `TXT_Title`
- `Image_SmileIcon`
  - 노란 스마일 아이콘
- `ScrollBox_Message`
  - 긴 본문 스크롤 영역
- `TXT_Message`
  - 실제 긴 메시지 텍스트
- `HorizontalBox_ButtonRow`
  - `BTN_Ok`

### 권장 BindWidget 대상
- `TXT_Title`
- `Image_SmileIcon`
- `ScrollBox_Message`
- `TXT_Message`
- `BTN_Ok`

## 시각 디자인 명세

### 공통 방향
- 기본 패널 외양은 기존 단순 팝업과 동일 계열을 유지한다.
- 특별한 추가 지시가 없으면 크기, blur 강도, 라운딩 감각, 전체 비례는 기존 팝업을 따른다.
- 시각적으로 바뀌는 핵심 포인트는 노란색 틴트와 스마일 아이콘이다.

### 패널 배치
- 팝업은 화면 중앙 고정이다.
- 전체 화면을 검게 덮는 강한 modal 배경은 쓰지 않는다.
- 사용자의 시선은 중앙 패널에 모이고, 주변 화면은 약하게 비쳐 보이는 정도로 유지한다.

### 패널 스타일
- 패널 뒤는 `BackgroundBlur`로 흐리게 처리한다.
- 패널 위에는 노란색 계열의 저투명 틴트를 얹는다.
  - 예시: 따뜻한 노란색 RGB + `Alpha 0.10 ~ 0.18`
- 모서리는 기존 단순 팝업 수준의 라운딩을 유지한다.
- 패널 그림자, 테두리, 버튼 스타일은 기존 단순 팝업 디자인 패밀리를 그대로 따르는 것이 우선이다.

### 레이아웃 비율
- 권장 폭 `520 ~ 620`
- 권장 높이 `340 ~ 460`
- 스크롤 영역은 패널 내부에서 가장 큰 세로 영역을 차지해야 한다.
- 메시지가 짧아도 전체 패널이 과도하게 줄어들지 않도록 최소 높이를 유지한다.

### 아이콘 스타일
- 스마일 아이콘은 따뜻한 노란색 계열이며 본문 상단 중앙 또는 좌우 균형이 맞는 위치에 둔다.
- 지나치게 사실적인 아이콘보다 단순하고 읽기 쉬운 UI 아이콘이 낫다.
- 아이콘 크기는 타이틀보다 약간 크거나 비슷한 수준에서 시선 유도 역할만 하게 유지한다.
- 아이콘 원본 텍스처의 배경은 완전 투명해야 한다.
- WBP의 `Image_SmileIcon`은 투명 배경 텍스처 브러시를 사용한다.

### 버튼 스타일
- 하단 버튼은 `OK` 하나만 둔다.
- 기존 단순 팝업 버튼 스타일을 최대한 재사용한다.
- hover 상태는 최소한의 시각 차이를 가져야 한다.

## 입력 및 상호작용 흐름
1. 플레이어가 레벨에 별도로 배치된 스크롤 메시지 팝업용 BP를 향해 상호작용한다.
2. 플레이어 컨트롤러는 기존처럼 `UCodexInteractionSubsystem::RequestInteraction()` 경로를 사용한다.
3. `UCodexInteractionSubsystem`은 해당 대상이 스크롤 메시지 팝업을 요구하는지 판별한다.
4. 조건이 맞으면 스크롤 메시지 팝업 요청 데이터를 구성한다.
5. 서브시스템은 WBP 애셋 경로를 통해 팝업을 로드하고 `CreateWidget` 후 `AddToViewport` 한다.
6. 팝업이 열리면 마우스 클릭이 가능하도록 입력 모드와 커서를 적절히 전환한다.
7. 위젯은 타이틀, 스마일 아이콘, 긴 메시지를 반영하고 스크롤 위치를 맨 위로 맞춘다.
8. 사용자가 `OK` 버튼을 누르면 결과가 먼저 `UCodexInteractionSubsystem`으로 들어간다.
9. 서브시스템은 필요하면 원래의 상호작용 대상에 결과를 전달하고 팝업을 닫는다.
10. 팝업 종료 후 입력 모드와 커서 상태를 원복한다.

## 에디터 모듈 / 커맨드렛 계획

### 1. WBP 생성 규칙
- `WBP_InteractionScrollMessagePopup`은 에디터 모듈 코드가 실제 애셋으로 생성/갱신한다.
- `WidgetTree` 안에는 타이틀바, 노란 스마일 아이콘, 스크롤 영역, `OK` 버튼이 실제로 존재해야 한다.
- 런타임 fallback으로 위젯 트리를 코드 조립하는 경로는 두지 않는다.

### 2. 스타일 재사용 규칙
- 가능한 한 기존 단순 팝업의 스타일 자산, 버튼 스타일, 패널 구조를 재사용한다.
- 단, `Border_SkyTintPanel` 계열 색상만 노란색 계열로 바꾼 별도 패널 스타일 또는 브러시를 사용한다.
- 기존 팝업과 동일한 구조를 유지해야 하므로 별도의 전혀 다른 시각 언어는 도입하지 않는다.

### 3. 스마일 아이콘 텍스처 생성 규칙
- 노란 스마일 아이콘은 외부 `PNG`를 임포트하지 않는다.
- 에디터 모듈 코드가 투명 배경 `UTexture2D` 애셋을 직접 생성한다.
- 배경 픽셀은 알파 `0`으로 채워 완전 투명해야 한다.
- 얼굴, 눈, 입만 노란색/짙은색 불투명 픽셀로 그린 단순 UI 아이콘으로 만든다.
- 권장 해상도는 `64x64` 또는 `128x128`이다.
- UI 용도에 맞게 `TextureGroup=UI` 계열 설정과 mip 최소화 방향을 적용한다.
- 생성과 갱신은 다른 WBP 애셋과 마찬가지로 에디터 모듈 코드 + commandlet 경로로만 수행한다.

### 4. 커맨드렛 진입
- 1차 구현에서는 기존 상호작용 애셋 빌드 커맨드렛에 스크롤 팝업 WBP 생성 단계를 추가하는 방향이 우선이다.
- 기존 팝업과 같은 애셋 빌드 흐름 안에서 관리하는 편이 충돌 지점과 유지보수 비용을 줄인다.

### 5. 에디터 충돌 처리
- 커맨드렛 실행 중 현재 프로젝트 에디터가 실행 중이면 즉시 실패한다.
- 실패 후 우회 경로는 시도하지 않는다.
- 사용자에게 에디터 종료 후 다시 실행해야 함을 그대로 알린다.

## 완료 기준
- `WBP_InteractionScrollMessagePopup`이 실제 애셋으로 존재한다.
- `BP_Interactable_WoodenSignScrollPopup`이 단순 메시지 팝업용 BP와 별도의 실제 애셋으로 존재한다.
- 노란 스마일 아이콘 텍스처 애셋이 외부 임포트 없이 자동 생성되어 존재한다.
- 팝업을 여는 상호작용 이름 또는 prompt가 `읽기`로 표시된다.
- WBP 안에서 타이틀바, 노란 스마일 아이콘, `ScrollBox`, 긴 메시지 텍스트, `OK` 버튼 구조를 확인할 수 있다.
- 패널 외양은 기존 단순 팝업과 같은 계열이며, 저투명 틴트만 노란색 계열로 바뀌어 있다.
- 긴 메시지는 패널 전체가 아니라 내부 스크롤 영역에서 스크롤된다.
- `OK` 버튼 결과는 먼저 `UCodexInteractionSubsystem`으로 전달된다.
- 생성/갱신은 에디터 모듈 + 커맨드렛 경로로만 수행된다.
- 레벨에는 단순 메시지 팝업용 BP와 스크롤 메시지 팝업용 BP가 서로 다른 액터 인스턴스로 구분되어 배치될 수 있다.

## 검증 체크리스트
1. `WBP_InteractionScrollMessagePopup`을 UMG 디자이너에서 열었을 때 실제 위젯 트리가 보인다.
2. 타이틀바, 스마일 아이콘, 스크롤 본문, `OK` 버튼이 실제 WBP에 존재한다.
3. `BP_Interactable_WoodenSignScrollPopup`이 단순 메시지 팝업용 BP와 별도 애셋으로 존재한다.
4. 스크롤 메시지 팝업용 BP가 레벨에서 단순 메시지 팝업용 BP와 별도 인스턴스로 구분 배치될 수 있다.
5. WBP 참조 기준값에 `_C` 클래스 경로를 저장하지 않는다.
6. 스마일 아이콘 텍스처가 외부 임포트가 아니라 자동 생성된 애셋이다.
7. 스마일 아이콘 텍스처 배경이 투명하다.
8. 패널 뒤 배경이 blur로 보인다.
9. 패널 위 저투명 중첩 색이 기존 하늘색이 아니라 노란색 계열로 보인다.
10. 긴 메시지가 패널 전체를 늘리지 않고 `ScrollBox` 내부에서만 스크롤된다.
11. 상호작용 가능 상태에서 사용자에게 보이는 텍스트가 `읽기`다.
12. `OK` 버튼 클릭 시 결과가 먼저 `UCodexInteractionSubsystem`에 전달된다.
13. 커맨드렛 실행 중 에디터 충돌이 발생하면 우회 없이 즉시 중단한다.
