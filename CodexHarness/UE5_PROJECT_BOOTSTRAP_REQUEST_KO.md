# UE5 프로젝트 부트스트랩 요청서

이 파일은 Unreal Editor를 실행하지 않고 새 Unreal Engine 5 프로젝트를 초기 설정할 때 재사용하는 요청 템플릿입니다.

이 템플릿은 아래 작업을 위한 것입니다.
- UE5용 `.gitignore` 생성 또는 병합
- 안전한 경우에만 엔진 기본 템플릿에서 `BasicMap` 생성
- `GameDefaultMap`과 `EditorStartupMap` 설정

## 사용 방법

아래 요청 블록을 복사한 뒤 `<ABSOLUTE_PROJECT_PATH>`만 바꿔서 그대로 요청하면 됩니다.

## 요청 템플릿

```md
먼저 이 파일을 읽어줘:
<ABSOLUTE_PATH_TO_THIS_MD_FILE>

그 다음 아래 UE5 프로젝트를 Unreal Editor를 실행하지 않고 초기 설정해줘.

프로젝트 경로:
<ABSOLUTE_PROJECT_PATH>

중요한 작업 절차:
1. 요청 파일 전체를 먼저 읽어줘.
2. 어떤 파일도 수정하기 전에, 무엇을 할지 먼저 정확히 설명해줘.
3. 아래 내용을 먼저 요약해줘:
   - 기존 `.gitignore`가 있는지
   - `.uproject`의 `EngineAssociation`에 맞는 설치된 UE 엔진을 찾았는지
   - `Template_Default.umap`가 파일 복사만으로 안전해 보이는지
   - 생성 또는 수정하려는 파일이 무엇인지
4. 그 요약을 보여준 뒤 멈추고, 계속 진행할지 물어봐줘.
5. 내가 명시적으로 확인한 뒤에만 실제 변경을 진행해줘.

확인 이후 작업 범위:
1. 이 파일에 있는 UE5 `.gitignore` 템플릿을 기준으로 프로젝트 루트의 `.gitignore`를 생성하거나 업데이트해줘.
2. 프로젝트의 `.uproject` 파일을 읽고 `EngineAssociation` 기준으로 설치된 엔진 경로를 찾아줘.
3. `/Engine/Content/Maps/Templates/Template_Default.umap`를 점검해줘.
4. 파일 복사만으로 안전한 경우에만 `Content/Maps/BasicMap.umap`를 생성해줘.
5. `Config/DefaultEngine.ini`에 아래 값들이 들어가게 해줘:
   - `GameDefaultMap=/Game/Maps/BasicMap`
   - `EditorStartupMap=/Game/Maps/BasicMap`
6. 필요한 폴더가 없으면 생성해줘.

제약 조건:
- Unreal Editor는 실행하지 마.
- 가능한 한 파일 시스템 작업과 설정 파일 수정만으로 처리해줘.
- 사용자 변경사항과 무관한 기존 변경은 되돌리지 마.
- Git 저장소 루트가 프로젝트 상위 폴더일 수 있다는 점을 감안해줘.

맵 생성 안전 규칙:
- 템플릿 맵이 이번 작업 기준으로 사실상 단독 복사가 가능한 경우에만 일반 파일 복사를 사용해줘.
- 아래와 같은 동반 데이터가 필요하면 일반 파일 복사를 사용하지 마:
  - `ExternalActors`
  - `ExternalObjects`
  - `*_BuiltData.uasset`
- 단순 복사가 안전하지 않으면 멈추고 이유를 설명해줘.

`.gitignore` 처리 규칙:
- `.gitignore`가 없으면 새로 생성해줘.
- `.gitignore`가 이미 있으면 신중하게 병합하고, 필수 UE5 규칙과 직접 충돌하지 않는 한 사용자 정의 규칙은 삭제하지 마.
- 최소한 이 파일에 적힌 UE5 ignore 항목들은 포함되게 해줘.

변경 후 검증:
- 생성하거나 수정한 모든 파일 경로를 알려줘.
- `Content/Maps/BasicMap.umap`가 실제로 존재하는지 확인해줘.
- `Config/DefaultEngine.ini`에서 최종 `GameDefaultMap`과 `EditorStartupMap` 값을 보여줘.
- Unreal Editor를 실행하지 않았다고 명시해줘.
- 템플릿 맵이 왜 파일 복사로 안전했는지, 또는 왜 중단했는지 짧게 설명해줘.
```

## UE5 .gitignore 템플릿

프로젝트 `.gitignore`를 새로 만들거나 병합할 때 아래 내용을 기준으로 사용합니다.

```gitignore
# Unreal Engine 5
Binaries/
DerivedDataCache/
Intermediate/
Saved/

# Plugin-generated artifacts
Plugins/*/Binaries/
Plugins/*/Intermediate/

# Visual Studio / Rider / VS Code
.vs/
.idea/
.vscode/

# Generated project files
*.sln
*.VC.db
*.VC.opendb
*.opensdf
*.sdf
*.suo
*.user
*.userprefs
*.vcxproj
*.vcxproj.filters
*.vcxproj.user

# Unreal generated files
*.code-workspace
*.xcodeproj
*.xcworkspace

# Crash dumps and logs
*.log
*.dmp
*.tmp

# Per-user editor content
Collections/
Developers/
```

## 메모

- 이 워크플로는 새 UE5 프로젝트에서 반복적으로 하는 초기 설정 작업을 에디터 없이 처리하기 위한 것입니다.
- 확인 단계는 의도적으로 넣은 것입니다. 먼저 예정 작업을 설명하고, 사용자 승인을 받은 뒤에만 실제 수정이 진행되어야 합니다.
- 맵 복사는 선택한 템플릿 맵이 복사 대상 `.umap` 바깥의 동반 데이터 없이 성립할 때만 허용됩니다.
