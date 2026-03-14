English: [README.md](./README.md)

# Project Bootstrapper

Project Bootstrapper는 프로젝트 전용 시작 클래스와 맵 기본값 구성을 빠르게 잡아주는 Unreal Engine 에디터 플러그인입니다.

다음 작업을 하나의 흐름으로 처리합니다.

- `/Game/Maps` 아래에서 관리 대상 맵 생성 또는 재오픈
- 대상 런타임 모듈에 네이티브 `GameInstance` 및 `GameMode` 클래스 생성
- 생성된 네이티브 클래스를 부모로 사용하는 Blueprint 에셋 생성
- 선택한 맵, 에디터 시작 맵, 게임 기본 맵, 선택적 글로벌 기본 게임모드 적용

현재 이 저장소에서는 플러그인이 `Plugins/ProjectBootstrapper` 아래의 프로젝트 플러그인으로 포함되어 있습니다.

## Plugin Type

- Category: `Editor`
- Module type: `Editor`
- Current version: `1.0.0`

## Where To Find It In The Editor

플러그인을 활성화한 뒤 다음 메뉴에서 열 수 있습니다.

- `Tools > Custom Tools > Project Bootstrapper`

## Main Workflow

1. 관리할 맵 이름을 입력하고 맵을 생성하거나 엽니다.
2. 런타임 모듈과 네이티브 클래스 이름을 확인합니다.
3. `Generate Code`를 클릭합니다.
4. 필요하면 Unreal의 컴파일 또는 모듈 리로드가 끝날 때까지 기다립니다.
5. `Create Blueprints & Apply`를 클릭합니다.

## Key Inputs

- `TargetRuntimeModule`: 네이티브 클래스를 생성할 런타임 모듈
- `ManagedMapName`: `/Game/Maps/<Name>`에 사용할 맵 이름
- `ManagedMapTemplate`: `Basic`, `Blank`, `TimeOfDay`
- `TargetMap`: 생성된 `GameMode`를 적용할 맵
- `EditorStartupMap`: 에디터 시작 맵
- `GameDefaultMap`: 게임 기본 맵
- `bSetAsGlobalDefaultGameMode`: 생성된 `GameMode`를 글로벌 기본 게임모드로도 설정할지 여부
- `GameInstanceBlueprintFolder` 및 `GameInstanceBlueprintName`
- `GameModeBlueprintFolder` 및 `GameModeBlueprintName`

## Repository Layout

- `Plugins/ProjectBootstrapper`: 플러그인 루트
- `Plugins/ProjectBootstrapper/Source`: C++ 소스
- `Plugins/ProjectBootstrapper/Resources`: 아이콘 및 도움말 텍스트

## Binary Distribution

이 플러그인은 바이너리 플러그인 패키지 형태로 배포하는 것을 기준으로 합니다.

패키징, 검증, 릴리스 절차는 [DEPLOYMENT_KO.md](./DEPLOYMENT_KO.md)를 참고하세요.
영문 배포 문서는 [DEPLOYMENT.md](./DEPLOYMENT.md)에서 볼 수 있습니다.
