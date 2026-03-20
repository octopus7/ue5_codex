# Clock Widget Task Template

다른 대화에서 동일한 형태의 일반 시계 위젯 작업을 다시 요청할 때 사용할 수 있는 템플릿이다.

## 변수 정의

- `<EnginePath>`: 사용할 Unreal Engine 경로
- `<ProjectRoot>`: 프로젝트 루트 경로
- `<ProjectName>`: 프로젝트명
  - 예: `CodexInven`
- `<ProjectEditorModuleName>`: 프로젝트의 에디터 모듈명
  - 보통 `<ProjectName>Editor`
  - 예: `CodexInvenEditor`
- `<ProjectUprojectPath>`: `.uproject` 전체 경로
- `<CommandletName>`: 1회성 생성용 커맨드렛 이름

## 요청 템플릿

```text
Unreal Engine 5.7.4 프로젝트다. 엔진 경로는 <EnginePath> 이고, 빌드/실행도 이 경로 기준으로만 해라.
먼저 AGENTS.md, <ProjectName>.uproject, Source/*.Target.cs 로 엔진 버전을 다시 확인해라.

이 문서에서:
- <ProjectName> 은 프로젝트명이다. 예: CodexInven
- <ProjectEditorModuleName> 은 그 프로젝트의 에디터 모듈명이다. 보통 <ProjectName>Editor 형태다. 예: CodexInvenEditor

기존 작업은 마이그레이션하지 말고, 일반 시계 위젯을 새로 구현해라.

요구사항:
- 런타임 위젯 클래스: UUserWidget 상속 C++ 클래스 하나 생성
- 디자인 트리 구성 코드는 런타임 클래스에 넣지 말 것
- 런타임 클래스는 BindWidget으로 필요한 위젯만 받아서 시간 텍스트와 시/분/초 바늘 각도만 갱신
- 디지털 시계와 아날로그 바늘 시계를 동시에 표시
- 아날로그 시계는 원형 시계판
- 눈금은 12개, 3시/6시/9시/12시 방향은 더 진하게
- 화면 중앙 상단 배치
- 사각 컨테이너 배경은 따뜻한 색
- WBP는 C++ 부모를 상속한 새 Widget Blueprint로 생성
- WBP 트리 구성은 에디터 전용 1회성 코드로 생성
- 이미 만들어진 WBP 트리 구조를 읽는 공용 유틸은 별도 permanent 코드로 분리
- WBP 생성/수정용 코드는 transient 폴더로 분리해서 나중에 제거 가능하게 할 것
- 원형 시계판이 필요하면 텍스처를 생성해서 Image로 써도 됨
- 플레이어 컨트롤러에서 이 WBP를 로드해 뷰포트에 추가
- Python Remote Execution, 외부 Python 스크립트, DefaultEngine.ini 수정은 하지 말 것

휘발성 코드 실행 방식:
- 에디터 전용 1회성 코드는 commandlet 또는 editor-only scaffold 로 구현
- commandlet 로 만들었다면 실제 실행 명령도 함께 맞춰라
- 실행 형식:
  "<EnginePath>\\Engine\\Binaries\\Win64\\UnrealEditor-Cmd.exe" "<ProjectUprojectPath>" -run=<CommandletName> -unattended -nop4 -nosplash -NoSound
- 예시:
  "<EnginePath>\\Engine\\Binaries\\Win64\\UnrealEditor-Cmd.exe" "<ProjectUprojectPath>" -run=<ProjectName>ClockWidgetScaffold -unattended -nop4 -nosplash -NoSound

산출물:
- 런타임 클래스
- 플레이어 컨트롤러 연결
- 공용 WBP 트리 조회 유틸
- 1회성 WBP/텍스처 생성 commandlet 또는 editor scaffold
- /Game/UI/WBP_CodexClock 생성
- 필요 시 /Game/UI/T_ClockFace_Analog 생성

검증:
- <ProjectEditorModuleName> 빌드
- 위 commandlet 실행으로 WBP 생성
- 0 error(s), 0 warning(s) 확인
- 에디터가 꺼져 있으면 프로젝트 다시 열기
```

## 현재 프로젝트 예시

- `<EnginePath>`: 로컬 Unreal Engine `5.7` 설치 경로
- `<ProjectRoot>`: `D:\github\ue5_codex\CodexInven`
- `<ProjectName>`: `CodexInven`
- `<ProjectEditorModuleName>`: `CodexInvenEditor`
- `<ProjectUprojectPath>`: `D:\github\ue5_codex\CodexInven\CodexInven.uproject`
- `<CommandletName>`: `CodexInvenClockWidgetScaffold`

실행 예시:

```powershell
& "<EnginePath>\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
  "D:\github\ue5_codex\CodexInven\CodexInven.uproject" `
  -run=CodexInvenClockWidgetScaffold `
  -unattended -nop4 -nosplash -NoSound
```

## 참고 구현 위치

- 런타임 위젯: `Source/CodexInven/CodexInvenClockWidget.h`, `Source/CodexInven/CodexInvenClockWidget.cpp`
- 플레이어 컨트롤러 연결: `Source/CodexInven/CodexInvenTopDownPlayerController.h`, `Source/CodexInven/CodexInvenTopDownPlayerController.cpp`
- 공용 트리 유틸: `Source/CodexInvenEditor/Public/WidgetBlueprint/CodexInvenWidgetBlueprintTreeUtils.h`, `Source/CodexInvenEditor/Private/WidgetBlueprint/CodexInvenWidgetBlueprintTreeUtils.cpp`
- 휘발성 스캐폴드: `Source/CodexInvenEditor/Private/Transient/ClockWidgetScaffolding/CodexInvenClockWidgetScaffoldCommandlet.h`, `Source/CodexInvenEditor/Private/Transient/ClockWidgetScaffolding/CodexInvenClockWidgetScaffoldCommandlet.cpp`
