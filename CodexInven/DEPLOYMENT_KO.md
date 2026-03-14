English: [DEPLOYMENT.md](./DEPLOYMENT.md)

# Binary Plugin Deployment

이 문서는 `ProjectBootstrapper`를 바이너리 Unreal Engine 플러그인으로 패키징하고 배포하는 방법을 설명합니다.

플러그인 개요와 에디터 사용 흐름은 [README_KO.md](./README_KO.md)를 참고하세요.
영문 개요 문서는 [README.md](./README.md)에서 볼 수 있습니다.

## Goal

이 저장소 전체를 복사하지 않고도 다른 Unreal Engine 프로젝트에 설치할 수 있는 패키징된 플러그인을 만드는 것이 목적입니다.

## Recommended Distribution Format

다음과 같은 버전 표기 아카이브 형태로 배포하는 것을 권장합니다.

- `ProjectBootstrapper-1.0.0-UE5.x-Win64.zip`

포함:

- `.uplugin`
- `Binaries/`
- `Resources/`
- 향후 플러그인 콘텐츠가 추가되면 `Content/`
- 향후 플러그인 전용 설정이 추가되면 `Config/`

제외:

- `Intermediate/`
- `.sln`
- 프로젝트 레벨 `Binaries/`
- 프로젝트 `Saved/`

## Packaging Methods

### Method 1: Unreal Editor

가능한 경우 Unreal Editor의 플러그인 패키징 기능을 사용합니다.

1. 호스트 프로젝트를 Unreal Editor에서 엽니다.
2. Plugins 창을 엽니다.
3. `Project Bootstrapper`를 찾습니다.
4. `Package`를 실행합니다.
5. 패키징 결과를 저장할 출력 디렉터리를 선택합니다.

이 방식은 로컬 릴리스 빌드를 위한 가장 단순한 수동 절차입니다.

### Method 2: RunUAT BuildPlugin

반복 가능한 패키징을 위해 `RunUAT BuildPlugin`을 사용합니다.

예시:

```powershell
Engine\Build\BatchFiles\RunUAT.bat BuildPlugin `
  -Plugin="D:\github\ue5_codex\CodexInven\Plugins\ProjectBootstrapper\ProjectBootstrapper.uplugin" `
  -Package="D:\Builds\ProjectBootstrapper" `
  -TargetPlatforms=Win64
```

참고:

- `Engine\Build\BatchFiles\RunUAT.bat`는 실제 Unreal Engine 설치 경로로 바꿔야 합니다.
- 가능하면 패키지 출력 경로는 프로젝트 외부 디렉터리로 두는 편이 낫습니다.
- 추가 플랫폼은 실제 검증한 경우에만 넣는 것이 맞습니다.

## Release Checklist

배포 전에 다음을 확인합니다.

1. `ProjectBootstrapper.uplugin`의 플러그인 버전을 확인합니다.
2. 대상 엔진 버전과 플랫폼 기준으로 플러그인을 패키징합니다.
3. 패키징 결과물을 깨끗한 Unreal 프로젝트의 `Plugins/ProjectBootstrapper` 아래에 설치합니다.
4. 에디터를 실행해 재컴파일 없이 플러그인이 로드되는지 확인합니다.
5. `Tools > Custom Tools > Project Bootstrapper` 메뉴가 보이는지 확인합니다.
6. 관리 맵 생성 또는 열기, 코드 생성, 블루프린트 생성, 프로젝트 기본값 적용까지 전체 흐름을 검증합니다.
7. 패키징된 폴더를 릴리스 아카이브로 묶습니다.

## Installation In Another Project

1. 패키징된 플러그인 zip을 풉니다.
2. `ProjectBootstrapper` 폴더를 대상 프로젝트의 `Plugins/` 디렉터리 아래로 복사합니다.
3. 대상 프로젝트를 Unreal Editor에서 엽니다.
4. 자동 활성화되지 않으면 플러그인을 활성화합니다.
5. 필요 시 에디터를 재시작합니다.

예상 결과:

- 플러그인이 패키징된 바이너리에서 바로 로드됩니다.
- 에디터 메뉴에서 플러그인 항목을 사용할 수 있습니다.

## Versioning Guidance

바이너리 패키지는 다음 기준과 맞춰 관리해야 합니다.

- Unreal Engine 메이저 및 마이너 버전
- 대상 플랫폼
- 플러그인 `Version` 및 `VersionName`

바이너리 플러그인은 엔진 버전이 달라지면 호환성이 보장되지 않으므로, 지원하는 UE 버전별로 패키징하고 라벨링하는 것이 안전합니다.

## Suggested Release Artifact Layout

```text
ProjectBootstrapper-1.0.0-UE5.x-Win64.zip
  ProjectBootstrapper/
    Binaries/
    Resources/
    ProjectBootstrapper.uplugin
```

## Current Plugin Source

이 저장소에서의 플러그인 소스 경로:

- `Plugins/ProjectBootstrapper`
