# 상호작용 메시지 팝업 구현 계획

## 문서 목적
- 이 문서는 간단한 UMG 메시지 팝업 기능의 구현 계획만 정의한다.
- 실제 C++ 수정, 애셋 생성, 커맨드렛 실행, 맵 배치 작업은 이 문서에서 수행하지 않는다.
- 팝업은 월드 상호작용으로 열리고, 결과는 기존 상호작용 통합 서브시스템으로 다시 전달되는 구조를 목표로 한다.

## 선행 고정 규칙
- WBP 작업 방식은 반드시 [wbp_asset_work_guidelines.md](./wbp_asset_work_guidelines.md)를 따른다.
- 따라서 팝업 UI는 런타임 코드가 위젯 트리를 조립하는 방식이 아니라, 실제 `Widget Blueprint` 애셋으로 존재해야 한다.
- WBP 생성/갱신 로직은 `CodexUMGBootstrapEditor` 같은 에디터 모듈 코드에 두고, 실행 진입점은 `Commandlet`로 고정한다.
- 커맨드렛 실행 시 현재 프로젝트의 `UnrealEditor.exe`가 열려 있어 충돌이 나면 우회하지 않고 즉시 중단하고 사용자에게 알린다.

## 목표 상태
1. 유저가 나무 팻말 액터에 가까이 가면 기존 상호작용 표시 시스템을 통해 상호작용 이름 `보기`가 나타난다.
2. 유저가 상호작용 액션을 수행하면 화면 중앙의 일정 영역만 차지하는 단순 메시지 팝업이 열린다.
3. 팝업은 타이틀 바, 메시지 본문, 닫기 버튼을 갖는 보편적인 다이얼로그 형태다.
4. 패널 내부에는 배경 블러가 보이고, 그 위에 낮은 opacity의 하늘색 틴트가 겹쳐지며, 패널 모서리는 라운딩된다.
5. 팝업 버튼 레이아웃은 코드에서 `OK` 또는 `Yes/No` 두 가지 중 하나로 지정할 수 있다.
6. 닫기 버튼을 누르면 팝업이 닫힌다.
7. `Space` 키를 눌러도 팝업이 닫힌다.
8. `OK`, `Yes`, `No`, `Closed` 결과는 모두 `UCodexInteractionSubsystem`으로 먼저 전달된다.
9. 테스트용 나무 팻말 액터 BP는 기존 메시 `/Game/Vox/Meshes/Props/SM_Vox_WoodenSignpost`를 사용한다.
10. 팝업 WBP와 테스트용 나무 팻말 BP는 모두 에디터 모듈 코드 + 커맨드렛 경로로 생성 가능해야 한다.

## 현재 프로젝트 기준 접점
- 상호작용 중심 진입점은 이미 [Source/CodexUMG/Public/Interaction/CodexInteractionSubsystem.h](../Source/CodexUMG/Public/Interaction/CodexInteractionSubsystem.h)에 `UCodexInteractionSubsystem`으로 존재한다.
- 월드 대상 상호작용 데이터는 [Source/CodexUMG/Public/Interaction/CodexInteractionComponent.h](../Source/CodexUMG/Public/Interaction/CodexInteractionComponent.h)의 `UCodexInteractionComponent`가 담당한다.
- 현재 상호작용 요청 구조체는 [Source/CodexUMG/Public/Interaction/CodexInteractionTypes.h](../Source/CodexUMG/Public/Interaction/CodexInteractionTypes.h)의 `FCodexInteractionRequest`를 사용한다.
- 현재 popup 자산 경로 상수는 [Source/CodexUMG/Public/Interaction/CodexInteractionAssetPaths.h](../Source/CodexUMG/Public/Interaction/CodexInteractionAssetPaths.h)를 기준으로 맞춘다.
- 테스트용 상호작용 액터 기반 클래스는 [Source/CodexUMG/Public/Interaction/CodexInteractableActor.h](../Source/CodexUMG/Public/Interaction/CodexInteractableActor.h)의 `ACodexInteractableActor`를 재사용할 수 있다.
- popup 위젯 C++ 베이스 클래스는 [Source/CodexUMG/Public/Interaction/CodexInteractionMessagePopupWidget.h](../Source/CodexUMG/Public/Interaction/CodexInteractionMessagePopupWidget.h)와 [Source/CodexUMG/Private/Interaction/CodexInteractionMessagePopupWidget.cpp](../Source/CodexUMG/Private/Interaction/CodexInteractionMessagePopupWidget.cpp)를 기준으로 확장한다.
- popup형 상호작용 액터 기반 클래스는 [Source/CodexUMG/Public/Interaction/CodexPopupInteractableActor.h](../Source/CodexUMG/Public/Interaction/CodexPopupInteractableActor.h)와 [Source/CodexUMG/Private/Interaction/CodexPopupInteractableActor.cpp](../Source/CodexUMG/Private/Interaction/CodexPopupInteractableActor.cpp)를 기준으로 확인한다.
- 기존 `Widget Blueprint` 생성 패턴은 [Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp](../Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp)의 `UWidgetBlueprintFactory` 사용 흐름을 기준으로 맞추면 된다.
- 기존 입력 바인딩 확장 패턴은 [Source/CodexUMG/Private/CodexTopDownPlayerController.cpp](../Source/CodexUMG/Private/CodexTopDownPlayerController.cpp)와 [Source/CodexUMG/Public/CodexTopDownInputConfigDataAsset.h](../Source/CodexUMG/Public/CodexTopDownInputConfigDataAsset.h)를 기준으로 따른다.
- 런타임/에디터 모듈 모두 이미 `UMG` 의존성을 가지고 있으므로, 1차 구현의 핵심은 새 UI 구조와 상호작용 흐름 추가다.
- 현재 상호작용 위젯 참조 일부는 `_C` 클래스 경로 패턴을 쓰고 있지만, 이번 팝업 계획은 [wbp_asset_work_guidelines.md](./wbp_asset_work_guidelines.md)에 따라 WBP 애셋 경로를 기준값으로 잡아야 한다.

## 핵심 원칙
- 팝업은 월드에 붙는 `UWidgetComponent`가 아니라, 플레이어 화면에 `AddToViewport`되는 viewport UMG다.
- 팝업 위젯 트리는 실제 `WBP_InteractionMessagePopup` 애셋 안에 저장되어 있어야 한다.
- 코드의 책임은 데이터 주입, 표시/숨김, 버튼 레이아웃 전환, 결과 전달에 한정한다.
- `OK`/`Yes`/`No`/`Closed` 어느 결과든 먼저 `UCodexInteractionSubsystem`이 수신하고, 그 다음에 대상 액터나 후속 처리 지점으로 라우팅한다.
- 1차 구현에서는 팝업을 한 번에 하나만 띄운다. 큐, 중첩, 다중 모달은 다루지 않는다.
- 팝업이 열려 있는 동안 새 상호작용 요청은 무시하거나 현재 팝업 종료 전까지 보류하는 단일 활성 상태를 유지한다.
- 나무 팻말의 상호작용 타입은 1차 구현에서 새 enum을 추가하지 않고 `Use`를 유지해도 된다. 사용자에게 보이는 이름은 `PromptText = "보기"`로 해결한다.
- 마우스 클릭이 가능한 버튼 UI이므로, 팝업이 열릴 때 플레이어 컨트롤러는 `Game and UI` 성격의 입력 모드와 마우스 커서 표시를 함께 관리해야 한다.

## 산출물

### 런타임 C++ 산출물
- `ECodexPopupButtonLayout`
  - `Ok`
  - `YesNo`
- `ECodexPopupResult`
  - `Ok`
  - `Yes`
  - `No`
  - `Closed`
- `FCodexInteractionPopupRequest`
  - 타이틀, 메시지, 버튼 레이아웃, 요청 주체, 대상 액터, 대상 상호작용 컴포넌트를 묶는 구조체
- `FCodexInteractionPopupResponse`
  - 요청 ID, 결과 enum, 요청 주체, 대상 액터를 묶는 구조체
- `UCodexInteractionMessagePopupWidget`
  - 실제 `WBP_InteractionMessagePopup`의 C++ 베이스 클래스
- `UCodexInteractionSubsystem` 확장
  - 팝업 열기, 활성 팝업 유지, 팝업 닫기 요청, 결과 수신, 결과 라우팅
- 팝업 데이터 제공용 상호작용 액터 확장
  - 권장안: `ACodexPopupInteractableActor : public ACodexInteractableActor`
  - 나무 팻말 BP는 이 클래스를 부모로 사용
- `UCodexTopDownInputConfigDataAsset` 확장
  - `PopupCloseAction` 참조 추가
- `ACodexTopDownPlayerController` 확장
  - `PopupCloseAction` 바인딩과 활성 팝업 닫기 호출 추가

### 에디터 / 커맨드렛 산출물
- `CodexUMGBootstrapEditor` 내부의 팝업 WBP 생성/갱신 코드
- `CodexUMGBootstrapEditor` 내부의 나무 팻말 BP 생성/갱신 코드
- 기존 [Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexInteractionAssetBuildCommandlet.cpp](../Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexInteractionAssetBuildCommandlet.cpp) 확장
  - 1차 구현에서는 별도 커맨드렛보다 기존 상호작용 애셋 빌드 커맨드렛 확장을 우선 권장한다.

### 실제 콘텐츠 산출물
- `/Game/UI/Interaction/WBP_InteractionMessagePopup`
- `/Game/Blueprints/Interaction/BP_Interactable_WoodenSignPopup`
- `/Game/Input/Actions/IA_PopupClose`
- `/Game/Input/Contexts/IMC_TopDown` 갱신
- `/Game/Data/Input/DA_TopDownInputConfig` 갱신
- 필요 시 `/Game/Maps/BasicMap` 내 테스트용 나무 팻말 배치 갱신

## BP 명시
- 단순 메시지 팝업용 최종 상호작용 BP는 `BP_Interactable_WoodenSignPopup`으로 명시한다.
- 이 BP는 레벨에 실제로 배치되는 최종 애셋이다.
- 공용 부모 클래스나 공용 C++ 기반은 재사용할 수 있지만, 레벨에 올리는 최종 BP는 이 애셋으로 본다.
- 스크롤 메시지 팝업용 `BP_Interactable_WoodenSignScrollPopup`과는 별도 애셋으로 구분한다.
- 따라서 레벨에서는 단순 메시지 팝업용 액터와 스크롤 메시지 팝업용 액터가 서로 다른 인스턴스로 별도 배치될 수 있다.

## 권장 데이터 구조

### 1. `ECodexPopupButtonLayout`
- `Ok`
  - 하단 버튼 행에 `OK` 버튼만 노출한다.
- `YesNo`
  - 하단 버튼 행에 `Yes`, `No` 버튼을 노출한다.

### 2. `ECodexPopupResult`
- `Ok`
  - `OK` 버튼 클릭
- `Yes`
  - `Yes` 버튼 클릭
- `No`
  - `No` 버튼 클릭
- `Closed`
  - 타이틀 바 닫기 버튼 클릭 또는 `Space` 키 닫기

### 3. `FCodexInteractionPopupRequest`
- 권장 필드
  - `FGuid RequestId`
  - `FText Title`
  - `FText Message`
  - `ECodexPopupButtonLayout ButtonLayout`
  - `TWeakObjectPtr<APlayerController> RequestingController`
  - `TWeakObjectPtr<AActor> SourceActor`
  - `TWeakObjectPtr<UCodexInteractionComponent> SourceInteractionComponent`
- 목적
  - 팝업 표시와 결과 라우팅에 필요한 문맥을 한 번에 전달한다.

### 4. `FCodexInteractionPopupResponse`
- 권장 필드
  - `FGuid RequestId`
  - `ECodexPopupResult Result`
  - `TWeakObjectPtr<APlayerController> RequestingController`
  - `TWeakObjectPtr<AActor> SourceActor`
  - `TWeakObjectPtr<UCodexInteractionComponent> SourceInteractionComponent`
- 목적
  - 결과를 `UCodexInteractionSubsystem`이 해석하고 후속 처리 대상으로 전달할 수 있게 한다.

## 권장 런타임 구조

### 1. `UCodexInteractionSubsystem`
- 책임
  - 기존 상호작용 요청 수신
  - 현재 대상이 팝업형 상호작용인지 판별
  - 팝업 요청 구성
  - 활성 팝업 인스턴스 생성 및 표시
  - `Space` 닫기 요청 처리
  - 버튼 클릭 결과 수신
  - 결과를 대상 액터 또는 후속 처리 지점으로 전달
- 권장 신규 함수
  - `bool OpenInteractionPopup(const FCodexInteractionPopupRequest& Request)`
  - `void SubmitInteractionPopupResult(const FCodexInteractionPopupResponse& Response)`
  - `void RequestCloseActivePopup(APlayerController* RequestingController)`
  - `bool HasActivePopup() const`
- 상태 관리 원칙
  - 활성 팝업은 한 개만 유지한다.
  - 활성 팝업이 있으면 `RequestInteraction()`은 새 팝업을 열지 않는다.
  - 팝업이 닫히면 내부 상태를 정리하고 입력 모드를 원복한다.

### 2. `UCodexInteractionMessagePopupWidget`
- 역할
  - `WBP_InteractionMessagePopup`의 C++ 베이스 클래스
  - 위젯 생성이 아니라 위젯 상태 반영 담당
- 책임
  - 타이틀/본문 텍스트 반영
  - 버튼 레이아웃 전환
  - 닫기 버튼, `OK`, `Yes`, `No` 버튼 클릭 이벤트를 서브시스템에 전달
  - 최초 표시 시 간단한 페이드 인 정도만 담당 가능
- 비책임
  - 위젯 트리 생성
  - 상호작용 대상 판정
  - 결과 직접 소비
- 구현 원칙
  - 버튼 세 종류는 모두 WBP 안에 실제로 존재시키고, 코드에서 `Visibility`만 전환한다.
  - WBP 참조의 기준값은 애셋 경로다.
  - `UClass` 해석은 실제 `CreateWidget` 직전에만 수행한다.

### 3. `ACodexPopupInteractableActor`
- 목적
  - 팝업 메시지를 띄우는 상호작용 대상의 공통 베이스
- 권장 소유 데이터
  - `PopupTitle`
  - `PopupMessage`
  - `ButtonLayout`
- 권장 동작
  - `HandleInteractionRequested`에서 즉시 파괴/소비하지 않는다.
  - 대신 `UCodexInteractionSubsystem`이 사용할 `FCodexInteractionPopupRequest` 문맥을 제공한다.
  - 팝업 결과가 돌아오면 `BlueprintImplementableEvent` 또는 `BlueprintNativeEvent`로 후속 반응을 받는다.

### 4. 나무 팻말 BP
- 이름 권장
  - `BP_Interactable_WoodenSignPopup`
- 부모 클래스 권장
  - `ACodexPopupInteractableActor`
- 문서 기준 역할
  - 단순 메시지 팝업 전용 최종 상호작용 BP
  - 레벨에 실제로 배치되는 최종 애셋
  - 스크롤 메시지 팝업용 `BP_Interactable_WoodenSignScrollPopup`과는 별도 애셋
- 레벨 배치 기준
  - 단순 메시지 팝업이 필요하면 `BP_Interactable_WoodenSignPopup` 인스턴스를 별도 액터로 배치한다.
- 기본값
  - Static Mesh: `/Game/Vox/Meshes/Props/SM_Vox_WoodenSignpost`
  - InteractionType: `Use`
  - PromptText: `보기`
  - PopupTitle: 예) `안내`
  - PopupMessage: 예) 팻말에 적힌 테스트 문구
  - ButtonLayout: `Ok` 또는 `YesNo`

## WBP 구조 명세

### 실제 애셋 이름
- `/Game/UI/Interaction/WBP_InteractionMessagePopup`

### 구조 원칙
- 아래 위젯 트리는 실제 WBP 애셋의 `WidgetTree`에 존재해야 한다.
- 런타임 C++는 이 트리를 만들지 않고, 이미 존재하는 위젯을 `BindWidget`으로 받아 제어만 한다.

### 권장 위젯 트리
- `RootCanvas`
  - 전체 화면 루트
- `Overlay_ScreenRoot`
  - 화면 중앙 정렬용 레이어
- `SizeBox_PopupFrame`
  - 팝업 최대 폭 제한
- `Overlay_PanelRoot`
  - 패널 시각 요소와 컨텐츠 중첩
- `BackgroundBlur_Panel`
  - 패널 영역에만 블러 적용
- `Border_SkyTintPanel`
  - 낮은 opacity 하늘색 틴트
- `VerticalBox_Content`
  - 타이틀 바 / 본문 / 버튼 행 배치
- `HorizontalBox_TitleBar`
  - `TXT_Title`
  - `BTN_Close`
- `TXT_Message`
  - 메시지 본문
- `HorizontalBox_ButtonRow`
  - `BTN_Ok`
  - `BTN_Yes`
  - `BTN_No`

### 권장 BindWidget 대상
- `TXT_Title`
- `TXT_Message`
- `BTN_Close`
- `BTN_Ok`
- `BTN_Yes`
- `BTN_No`
- `HorizontalBox_ButtonRow`

## 시각 디자인 명세

### 배치
- 팝업은 화면 중앙에 고정한다.
- 전체 화면을 덮는 검은 배경 모달은 두지 않는다.
- 중앙 패널만 시선을 끄는 구조를 사용한다.

### 패널 크기
- 기준 폭 `480 ~ 560`
- 기준 높이 `220 ~ 320`
- 너무 넓은 전체 화면 박스는 금지한다.

### 패널 스타일
- 패널 뒤는 `BackgroundBlur`로 흐리게 보여야 한다.
- 블러는 전체 화면이 아니라 패널 중심 영역에 한정하는 것을 우선 권장한다.
- 그 위에 낮은 opacity의 하늘색 틴트를 얹는다.
  - 예: 하늘색 계열 RGB + `Alpha 0.12 ~ 0.20`
- 모서리는 명확히 둥글어야 한다.
  - 권장 반경 `16 ~ 24`
- 타이틀 바와 본문은 시각적으로 분리하되, 과한 장식은 넣지 않는다.

### 버튼 스타일
- `OK`, `Yes`, `No`, `닫기` 버튼은 동일한 디자인 계열을 유지한다.
- 1차 구현에서는 hover/pressed 상태가 단순해도 되지만, 최소한 일반/hover 차이는 있어야 한다.
- `YesNo` 레이아웃일 때 `OK` 버튼은 숨기고, `Ok` 레이아웃일 때 `Yes/No` 버튼은 숨긴다.

## 입력 및 상호작용 흐름
1. 플레이어가 레벨에 별도로 배치된 `BP_Interactable_WoodenSignPopup` 나무 팻말에 가까워지면 기존 상호작용 표시 위젯에서 `보기`가 보인다.
2. 플레이어가 상호작용 입력을 누르면 `ACodexTopDownPlayerController`는 기존처럼 `UCodexInteractionSubsystem::RequestInteraction()`만 호출한다.
3. `UCodexInteractionSubsystem`은 현재 포커스된 상호작용 대상이 팝업형 액터인지 판별한다.
4. 팝업형 액터이면 `FCodexInteractionPopupRequest`를 구성해 팝업을 연다.
5. 서브시스템은 WBP 애셋 기준 경로에서 팝업 애셋을 로드하고, 최종 사용 시점에서만 위젯 클래스를 해석해 `CreateWidget` 후 `AddToViewport`한다.
6. 팝업이 열리면 플레이어 컨트롤러는 마우스 커서를 표시하고 입력 모드를 팝업 클릭 가능 상태로 바꾼다.
7. 유저가 `BTN_Close` 또는 `Space`를 사용하면 `Closed` 결과가 발생한다.
8. 유저가 `OK`, `Yes`, `No` 중 하나를 누르면 해당 결과 enum이 발생한다.
9. 결과는 항상 `UCodexInteractionSubsystem::SubmitInteractionPopupResult()`로 먼저 들어간다.
10. 서브시스템은 필요 시 대상 액터에 결과를 전달하고, 팝업을 닫고, 입력 모드와 커서를 원복한다.

## 입력 자산 계획

### 1. 신규 입력 액션
- `/Game/Input/Actions/IA_PopupClose`
- 값 타입은 `Boolean`
- 기본 키는 `Space`

### 2. 입력 매핑
- 기존 `/Game/Input/Contexts/IMC_TopDown`에 `IA_PopupClose -> Space` 매핑을 추가한다.
- 기존 `/Game/Data/Input/DA_TopDownInputConfig`에 `PopupCloseAction` 참조를 추가한다.

### 3. 플레이어 컨트롤러 바인딩
- [Source/CodexUMG/Private/CodexTopDownPlayerController.cpp](../Source/CodexUMG/Private/CodexTopDownPlayerController.cpp)에 기존 패턴과 동일하게 `BindTopDownPopupCloseAction()`을 추가한다.
- `HandlePopupCloseInput()`는 활성 팝업이 있을 때만 `UCodexInteractionSubsystem::RequestCloseActivePopup(this)`를 호출한다.
- 활성 팝업이 없으면 아무 동작도 하지 않는다.

## 에디터 모듈 / 커맨드렛 계획

### 1. WBP 생성 원칙
- `WBP_InteractionMessagePopup`는 에디터 모듈 코드가 실제 애셋으로 생성/갱신한다.
- `WidgetTree`에는 위에서 정의한 루트, 블러, 타이틀, 메시지, 버튼 위젯이 실제로 들어 있어야 한다.
- 런타임에 위젯 트리를 만들기 위한 보조 코드 경로는 두지 않는다.

### 2. BP 생성 원칙
- `BP_Interactable_WoodenSignPopup`는 에디터 모듈 코드로 생성/갱신한다.
- 부모는 `ACodexPopupInteractableActor`를 사용한다.
- 메시는 `/Game/Vox/Meshes/Props/SM_Vox_WoodenSignpost`를 지정한다.
- `PromptText`는 `보기`로 기본 설정한다.

### 3. 커맨드렛 진입
- 1차 구현에서는 기존 `UCodexInteractionAssetBuildCommandlet`에 아래 단계를 추가하는 방식을 권장한다.
  1. 팝업 WBP 생성/갱신
  2. `IA_PopupClose` 생성/갱신
  3. `IMC_TopDown`, `DA_TopDownInputConfig` 갱신
  4. 나무 팻말 BP 생성/갱신
  5. 필요 시 `BasicMap` 테스트 배치 갱신

### 4. 에디터 충돌 처리
- 커맨드렛 실행 중 현재 프로젝트 에디터가 실행 중이면 즉시 실패한다.
- 실패 시 우회 경로를 시도하지 않는다.
- 사용자에게 에디터 종료 후 다시 실행해야 한다는 사실만 명확히 알린다.

## 완료 기준
- `WBP_InteractionMessagePopup`가 실제 애셋으로 존재한다.
- 위젯 트리는 해당 WBP 애셋 안에서 확인 가능하다.
- 나무 팻말 BP가 실제 애셋으로 존재하고 `SM_Vox_WoodenSignpost`를 사용한다.
- 나무 팻말 상호작용 프롬프트는 `보기`다.
- 상호작용 입력 시 중앙 팝업이 열린다.
- 닫기 버튼과 `Space` 키로 팝업을 닫을 수 있다.
- 버튼 레이아웃은 코드에서 `Ok` 또는 `YesNo`로 지정할 수 있다.
- `OK`, `Yes`, `No`, `Closed` 결과는 모두 `UCodexInteractionSubsystem`이 먼저 수신한다.
- 생성/갱신은 에디터 모듈 + 커맨드렛 경로로만 수행된다.

## 검증 체크리스트
1. `WBP_InteractionMessagePopup`를 UMG 디자이너에서 열었을 때 타이틀 바, 메시지, 닫기 버튼, 버튼 행, 블러 패널 구조가 실제로 보인다.
2. WBP 참조 기준값에 `_C` 클래스 경로를 저장하지 않는다.
3. `BP_Interactable_WoodenSignPopup`의 메시가 `SM_Vox_WoodenSignpost`로 설정되어 있다.
4. 나무 팻말 근처에서 기존 상호작용 표시 텍스트가 `보기`로 보인다.
5. 상호작용 입력 시 팝업이 화면 중앙의 제한된 영역만 차지한다.
6. 패널 뒤 배경이 블러되어 보인다.
7. 패널 위에 낮은 opacity의 하늘색 틴트가 겹쳐 보인다.
8. 패널 모서리가 둥글게 보인다.
9. `Ok` 레이아웃에서는 `OK` 버튼만 보인다.
10. `YesNo` 레이아웃에서는 `Yes`와 `No` 버튼만 보인다.
11. `Space`를 누르면 팝업이 닫히고 결과는 `Closed`로 전달된다.
12. 버튼 클릭 결과가 액터보다 먼저 `UCodexInteractionSubsystem`에 도달한다.
13. 커맨드렛 실행 중 에디터 충돌이 발생하면 우회 없이 즉시 중단한다.
