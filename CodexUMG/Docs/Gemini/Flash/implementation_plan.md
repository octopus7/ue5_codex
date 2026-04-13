# [GeminiFlash] Simplified UMG Drag & Drop Implementation Plan

기존 `Codex` 시스템의 복잡도를 제거하고 핵심 동작만으로 구성된 최소 기능 드래그 앤 드롭 시스템을 구축합니다. 이 플랜은 사용자가 버튼 하나(Commandlet)로 블루프린트 자산까지 자동 생성할 수 있도록 설계되었습니다.

## User Review Required

> [!IMPORTANT]
> **Commandlet 테스트 방법**:
> 1. 프로젝트 빌드 후 명령행(CMD/PowerShell)에서 다음 명령을 실행합니다:
>    `UnrealEditor-Cmd.exe "프로젝트경로" -run=GeminiFlashSimpleBuild`
> 2. 실행 후 `/Game/GeminiFlash/` 폴더에 `WBP_SimpleSlot`, `WBP_SimplePopup` 자산이 생성되었는지 확인합니다.
> 3. 해당 위젯은 인터랙션 시스템 없이도 테스트 액터 혹은 콘솔 명령을 통해 직접 띄워볼 수 있습니다.

## Proposed Changes

### 1. Core C++ Foundation (Runtime Module)
드래그 앤 드롭의 표준 규약을 정의합니다.

#### [NEW] [GeminiFlashSimpleDragDropOperation.h](file:///d:/github/ue5_codex/CodexUMG/Source/CodexUMG/Public/Simple/GeminiFlashSimpleDragDropOperation.h)
- 최소한의 페이로드(`int32 Value`)와 시작점 참조(`SourceWidget`)를 포함합니다.

#### [NEW] [GeminiFlashSimpleSlotWidget.h](file:///d:/github/ue5_codex/CodexUMG/Source/CodexUMG/Public/Simple/GeminiFlashSimpleSlotWidget.h) / [.cpp](file:///d:/github/ue5_codex/CodexUMG/Source/CodexUMG/Private/Simple/GeminiFlashSimpleSlotWidget.cpp)
- `TXT_Value` (텍스트), `Border_Highlight` (호버 효과)를 포함하는 슬롯.
- 드래그 탐지 및 드롭 수신 로직 구현.

#### [NEW] [GeminiFlashSimplePopupWidget.h](file:///d:/github/ue5_codex/CodexUMG/Source/CodexUMG/Public/Simple/GeminiFlashSimplePopupWidget.h) / [.cpp](file:///d:/github/ue5_codex/CodexUMG/Source/CodexUMG/Private/Simple/GeminiFlashSimplePopupWidget.cpp)
- 슬롯들을 담는 컨테이너.
- **4x2 Grid Layout**: 총 8개의 슬롯을 `UniformGridPanel`에 배치.
- **Initial Data**: 시작 시 3개의 슬롯에 숫자 1, 2, 3을 무작위 혹은 순차적으로 채우고 나머지는 빈 슬롯으로 설정.
- 슬롯 간 데이터 스와프(`SwapValues`) 로직 담당.

### 2. Automation System (Editor Module)
블루프린트 자산 생성을 자동화합니다.

#### [NEW] [GeminiFlashSimpleBuildCommandlet.h](file:///d:/github/ue5_codex/CodexUMG/Source/CodexUMGBootstrapEditor/Public/Commandlets/GeminiFlashSimpleBuildCommandlet.h) / [.cpp](file:///d:/github/ue5_codex/CodexUMG/Source/CodexUMGBootstrapEditor/Private/Commandlets/GeminiFlashSimpleBuildCommandlet.cpp)
- `-run=GeminiFlashSimpleBuild` 명령어를 통해 실행되는 진입점.

#### [NEW] [GeminiFlashAssetBuilder.h](file:///d:/github/ue5_codex/CodexUMG/Source/CodexUMGBootstrapEditor/Private/Simple/GeminiFlashAssetBuilder.h) / [.cpp](file:///d:/github/ue5_codex/CodexUMG/Source/CodexUMGBootstrapEditor/Private/Simple/GeminiFlashAssetBuilder.cpp)
- `UWidgetBlueprintFactory`를 사용하여 `WBP_...` 자산을 생성하고 위젯 트리를 구성(Hierarchy Construction)하는 핵심 빌더 로직.

## Verification Plan

### Automated
- Commandlet 실행 시 에러 카운트 체크.
- 생성된 자산의 `ParentClass`가 올바르게 설정되었는지 확인.

### Manual
- 생성된 `WBP_SimplePopup`을 뷰포트에 띄워 드래그 앤 드롭 동작 확인.
