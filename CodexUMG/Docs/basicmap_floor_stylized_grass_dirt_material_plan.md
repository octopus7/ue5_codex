# BasicMap Floor Stylized Grass Dirt 머터리얼 적용 계획

## 문서 목적
- `/Game/Maps/BasicMap` 레벨의 `Floor` 액터에 단색 50% 그레이 대신, `SourceArt/T_Stylized_Grass_Dirt_01.png`를 임포트한 텍스처 기반 머터리얼을 적용하는 구현 기준을 정리한다.
- 작업은 수동 에디터 클릭이 아니라, 현재 프로젝트에서 이미 사용하는 에디터 모듈 코드 + `Commandlet` 경로로 반복 실행 가능하게 만드는 것을 기본 원칙으로 둔다.
- 같은 PNG를 다시 수정한 뒤 빌드를 재실행하면 텍스처와 머터리얼, `BasicMap` 적용 상태가 함께 갱신되도록 한다.

## 소스 파일
- 원본 PNG: `SourceArt/T_Stylized_Grass_Dirt_01.png`
- 본 문서 기준 원본 소스는 위 파일 하나만 사용한다.
- 구현은 파일명을 추정하지 말고 위 상대 경로를 기준으로 실제 파일 존재 여부를 먼저 확인해야 한다.

## 목표 상태
1. `SourceArt/T_Stylized_Grass_Dirt_01.png`가 실제 UE 텍스처 애셋으로 임포트 또는 리임포트된다.
2. 텍스처 애셋 `/Game/Materials/T_Stylized_Grass_Dirt_01`가 생성 또는 갱신된다.
3. 머터리얼 애셋 `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01`가 생성 또는 갱신된다.
4. 머터리얼은 `TexCoord`를 `U/V 4.0`으로 타일링한 뒤, 엔진 머터리얼 함수 `TextureVariation`으로 UV를 섞어 반복감을 줄인 결과를 `Base Color`에 연결하고, `Roughness = 1.0`, `Specular = 0.0`을 사용한다.
5. `/Game/Maps/BasicMap`을 열면 `Floor` 액터의 머터리얼 오버라이드가 새 텍스처 기반 머터리얼로 지정되어 있다.
6. 같은 빌드를 반복 실행해도 애셋이 중복 생성되지 않고, 항상 같은 결과로 수렴한다.
7. `Floor` 외의 액터, VOX 메시 공용 머터리얼, 상호작용 테스트 액터 배치는 건드리지 않는다.

## 현재 프로젝트 기준 접점
- 머터리얼 생성/갱신 패턴은 [Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxMaterialBuilder.cpp](../Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxMaterialBuilder.cpp)에 이미 있다.
- `BasicMap`을 직접 열고 수정한 뒤 저장하는 패턴은 [Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp](../Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp)에 이미 있다.
- 맵 직렬화 문자열 기준으로 `BasicMap`에는 `PersistentLevel.Floor_0`가 존재하며, 액터 라벨은 `Floor`다. 따라서 이번 작업은 "첫 번째 `StaticMeshActor`" 같은 추정이 아니라, 액터 라벨 `Floor`를 기준으로 대상을 찾아야 한다.
- 현재 프로젝트에는 `/Game/Vox/Materials/M_VoxVertexColor`라는 VOX 전용 공용 머터리얼이 이미 있으므로, 이번 작업은 그 애셋을 재활용하거나 수정하지 않고 별도 바닥 머터리얼을 만든다.

## 핵심 원칙
- 실제로 프로젝트 `Content` 아래에 존재하는 텍스처 애셋과 머터리얼 애셋을 만든다. 엔진 기본 머터리얼 인스턴스를 덮어쓰지 않는다.
- 텍스처 원본은 반드시 `SourceArt/T_Stylized_Grass_Dirt_01.png`에서 가져온다.
- 레벨 적용은 `StaticMesh` 원본 애셋 수정이 아니라, `BasicMap` 안의 `Floor` 액터 인스턴스에 대한 머터리얼 오버라이드로 처리한다.
- `Floor` 액터를 찾지 못하면 작업을 조용히 넘기지 말고 즉시 실패 처리한다.
- PNG 원본을 찾지 못하거나 임포트에 실패해도 즉시 실패 처리한다.
- 이미 열려 있는 에디터 세션과 저장 충돌 가능성이 있으면 작업을 멈추고 사용자에게 알린다.
- 이번 작업은 `BasicMap`의 `Floor`에만 국한한다. 다른 맵, 다른 바닥 액터, 다른 머터리얼은 건드리지 않는다.

## 산출물

### 에디터 / C++ 산출물
- `Source/CodexUMGBootstrapEditor/Private/Maps/CodexBasicMapFloorBuilder.h`
- `Source/CodexUMGBootstrapEditor/Private/Maps/CodexBasicMapFloorBuilder.cpp`
- `Source/CodexUMGBootstrapEditor/Public/Commandlets/CodexBasicMapFloorBuildCommandlet.h`
- `Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexBasicMapFloorBuildCommandlet.cpp`

### 실제 콘텐츠 산출물
- `/Game/Materials/T_Stylized_Grass_Dirt_01`
- `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01`
- `/Game/Maps/BasicMap` 내 `Floor` 액터 머터리얼 오버라이드 갱신

## 텍스처 명세

### 원본 경로
- 파일 경로: `SourceArt/T_Stylized_Grass_Dirt_01.png`

### UE 애셋 경로
- 패키지 경로: `/Game/Materials`
- 애셋 이름: `T_Stylized_Grass_Dirt_01`

### 임포트 규칙
- 원본 PNG를 새로 임포트하거나, 같은 이름 애셋이 이미 있으면 리임포트 또는 교체 갱신한다.
- `sRGB = true`
- 텍스처는 컬러 알베도 용도로만 사용한다.
- 텍스처 주소 모드는 기본적으로 `Wrap`을 사용한다.
- 필요 이상으로 UI용 압축이나 아이콘용 설정을 쓰지 않는다.

## 머터리얼 명세

### 애셋 경로
- 패키지 경로: `/Game/Materials`
- 애셋 이름: `M_BasicMapFloor_StylizedGrassDirt01`

### 그래프 규칙
- `Blend Mode = Opaque`
- `Shading Model = Default Lit`
- `TexCoord(UTiling = 4.0, VTiling = 4.0)`를 기본 UV로 사용한다.
- `/Engine/Functions/Engine_MaterialFunctions03/Texturing/TextureVariation` 함수를 사용한다.
- `TextureVariation.UVs <- TexCoord(4x)`로 연결한다.
- `TextureVariation.Shifted UVs -> TextureSample(T_Stylized_Grass_Dirt_01).UVs`로 연결한다.
- `TextureVariation.Raw UVs -> DDX/DDY -> TextureSample`의 derivative 입력으로 연결하고, `TextureSample.MipValueMode = Derivative`로 지정한다.
- `TextureSample(T_Stylized_Grass_Dirt_01).RGB -> Base Color`
- `Roughness <- Constant(1.0)`
- `Specular <- Constant(0.0)`

### UV 규칙
- `Floor` 메시의 UV 채널 0을 기준으로 `TexCoord` 노드를 사용한다.
- `TexCoord`의 `UTiling`, `VTiling`은 모두 `4.0`으로 지정한다.
- 반복 무늬가 단조롭게 보이지 않도록, 엔진 제공 `TextureVariation` 함수가 계산한 `Shifted UVs`를 사용한다.
- `TextureVariation`이 내부적으로 사용하는 변형 UV와 보정용 `Raw UVs`를 그대로 사용하고, 외부에서 임의 오프셋 블렌드를 다시 만들지 않는다.

### 금지 사항
- `Base Color`를 `Constant3Vector(0.5, 0.5, 0.5)`로 고정하는 방식
- 텍스처를 UI용 임시 자산처럼 생성하는 방식
- 머터리얼 인스턴스를 만들기 위해 별도 부모 머터리얼 체인을 도입하는 방식
- `/Game/Vox/Materials/M_VoxVertexColor`를 수정해 바닥에도 공용으로 재사용하는 방식
- UV를 1배 샘플 한 번만 써서 반복 패턴을 그대로 노출하는 방식
- `TextureVariation` 대신 수동 이중 샘플 + 임의 노이즈 블렌드를 다시 구현하는 방식

## 레벨 적용 방식

### 대상 찾기 규칙
- 대상 맵은 `/Game/Maps/BasicMap` 하나만 사용한다.
- 대상 액터는 액터 라벨 `Floor`로 찾는다.
- `PersistentLevel.Floor_0`라는 객체명은 현재 상태 확인용 정보로만 사용하고, 구현에서는 레벨 저장 시 이름이 바뀔 수 있으므로 라벨 `Floor`를 우선 기준으로 쓴다.

### 적용 규칙
- `BasicMap`을 로드한 뒤 모든 레벨 액터를 순회한다.
- `GetActorLabel() == "Floor"`인 액터를 찾는다.
- 액터는 반드시 `AStaticMeshActor`여야 한다.
- `UStaticMeshComponent`를 얻은 뒤 `GetNumMaterials()` 결과를 확인한다.
- 슬롯 수가 1개 이상이면 `0..NumMaterials-1` 전 슬롯에 `M_BasicMapFloor_StylizedGrassDirt01`를 지정한다.
- 슬롯 수가 0이면 조용히 넘어가지 말고 실패 처리한다.
- 적용 후 액터, 텍스처, 머터리얼, 월드 패키지를 모두 dirty 상태로 만들고 저장한다.

### 범위 제한
- `Floor` 액터의 머터리얼 오버라이드만 수정한다.
- `Floor`가 참조하는 `StaticMesh` 원본 애셋 자체의 슬롯 기본 머터리얼은 수정하지 않는다.
- `BasicMap` 안의 사과, 딸기, SkySphere, 광원, 플레이어 시작점은 수정하지 않는다.

## 권장 구현 순서
1. 새 빌더 유틸리티에서 `SourceArt/T_Stylized_Grass_Dirt_01.png` 존재 여부를 확인한다.
2. 같은 빌더 유틸리티에서 PNG를 `/Game/Materials/T_Stylized_Grass_Dirt_01`로 임포트 또는 리임포트한다.
3. 임포트된 텍스처를 참조하는 `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01` 머터리얼 생성/갱신 함수를 만들고, `4x` 타일링 + `TextureVariation` 함수 노드를 구성한다.
4. 같은 빌더 유틸리티에서 `BasicMap` 로드, `Floor` 탐색, 머터리얼 적용, 저장 함수를 만든다.
5. 맵 수정 흐름은 기존 [Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp](../Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp)의 `BasicMap` 로드/저장 방식과 동일한 수준으로 맞춘다.
6. `Commandlet`는 위 세 단계를 순서대로 호출하고, 중간에 실패하면 즉시 종료한다.
7. 마지막에 텍스처 애셋, 머터리얼 애셋, 맵 저장이 모두 성공했을 때만 성공 메시지를 출력한다.

## 권장 구현 세부

### 1. 텍스처 임포트
- 원본 파일의 절대 경로를 `ProjectDir/SourceArt/T_Stylized_Grass_Dirt_01.png`로 계산한다.
- 파일이 없으면 즉시 실패한다.
- `AssetTools` 기반 자동 임포트 또는 동등한 에디터 API를 사용해 `/Game/Materials`로 가져온다.
- 동일 이름 애셋이 이미 있으면 교체 갱신이 가능해야 한다.
- 임포트 직후 `Texture2D`를 다시 로드해서 후속 머터리얼 생성에 사용한다.

### 2. 머터리얼 생성
- `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01` 패키지를 준비한다.
- 기존 애셋이 있으면 로드해서 갱신하고, 없으면 새로 만든다.
- `UMaterialEditingLibrary::DeleteAllMaterialExpressions()`로 기존 그래프를 지운다.
- `UMaterialExpressionTextureCoordinate`, `UMaterialExpressionMaterialFunctionCall`, `UMaterialExpressionDDX`, `UMaterialExpressionDDY`, `UMaterialExpressionTextureSample`, `UMaterialExpressionConstant` 2개를 생성한다.
- `TextureCoordinate.UTiling = 4.0`, `TextureCoordinate.VTiling = 4.0`을 지정한다.
- `MaterialFunctionCall`에는 `/Engine/Functions/Engine_MaterialFunctions03/Texturing/TextureVariation`를 지정한다.
- `TextureCoordinate -> TextureVariation.UVs`를 연결한다.
- `TextureVariation.Shifted UVs`를 `TextureSample.UVs`에 연결한다.
- `TextureVariation.Raw UVs`를 `DDX`, `DDY`에 연결하고, 그 결과를 `TextureSample`의 derivative 입력(`DDX(UVs)`, `DDY(UVs)`)에 연결한다.
- `TextureSample.MipValueMode`는 `Derivative`로 지정한다.
- `TextureSample` 결과를 `Base Color`에 연결한다.
- `Roughness`, `Specular`는 상수로 연결한다.
- `LayoutMaterialExpressions()`와 `RecompileMaterial()`을 호출한다.
- 애셋과 패키지를 dirty 처리한다.

### 3. `BasicMap` 로드와 적용
- `/Game/Maps/BasicMap`를 실제 파일 경로로 변환해 로드한다.
- `UEditorActorSubsystem`을 통해 레벨 액터를 조회한다.
- 라벨 `Floor` 액터를 찾는다.
- `StaticMeshComponent->SetMaterial(SlotIndex, NewMaterial)`를 슬롯별로 호출한다.
- 적용 뒤 `World->MarkPackageDirty()`를 호출한다.
- `UEditorLoadingAndSavingUtils::SaveMap()`으로 저장한다.

### 4. 실패 메시지 기준
- `SourceArt/T_Stylized_Grass_Dirt_01.png was not found.`
- `Failed to import or load /Game/Materials/T_Stylized_Grass_Dirt_01.`
- `Failed to create or load /Game/Materials/M_BasicMapFloor_StylizedGrassDirt01.`
- `Failed to load material function /Engine/Functions/Engine_MaterialFunctions03/Texturing/TextureVariation.TextureVariation.`
- `Failed to author material graph for /Game/Materials/M_BasicMapFloor_StylizedGrassDirt01.`
- `Failed to load /Game/Maps/BasicMap.`
- `Failed to find actor labeled Floor in BasicMap.`
- `Actor labeled Floor is not a StaticMeshActor.`
- `Floor StaticMeshComponent has no material slots to override.`
- `Failed to save /Game/Maps/BasicMap after applying floor material.`

## 권장 커맨드 예시
```powershell
$ProjectRoot = (Resolve-Path .).Path
$ProjectPath = Join-Path $ProjectRoot 'CodexUMG.uproject'
UnrealEditor-Cmd.exe $ProjectPath -run=CodexBasicMapFloorBuild -unattended -nop4 -nosplash
```

## 검증 체크리스트
- `SourceArt/T_Stylized_Grass_Dirt_01.png`가 실제로 존재하는가
- `/Game/Materials/T_Stylized_Grass_Dirt_01`가 실제 애셋으로 임포트되는가
- `/Game/Materials/M_BasicMapFloor_StylizedGrassDirt01`가 실제 애셋으로 생성되는가
- 머터리얼 그래프가 `TexCoord 4x` + `TextureVariation` + `DDX/DDY derivative sampling` + `Roughness 1.0` + `Specular 0.0`으로 구성되는가
- `BasicMap` 로드 후 `Floor` 액터를 정확히 찾는가
- `Floor` 액터의 머터리얼 슬롯이 하나 이상 존재하는가
- 모든 슬롯이 새 머터리얼로 덮어써지는가
- 레벨 저장 후 다시 열어도 `Floor`에 새 머터리얼이 유지되는가
- 같은 커맨드렛을 다시 실행해도 애셋 중복이 생기지 않는가
- PNG 원본을 교체한 뒤 다시 실행하면 임포트 텍스처와 바닥 머터리얼이 함께 갱신되는가
- `Floor` 이외의 액터에는 머터리얼 변경이 생기지 않는가
- `M_VoxVertexColor` 같은 기존 공용 머터리얼이 수정되지 않는가

## 금지 사항
- 단색 50% 그레이 머터리얼을 그대로 유지하는 방식
- 엔진 콘텐츠의 `MI_ProcGrid`나 다른 기본 머터리얼을 직접 수정하는 방식
- `SM_Template_Map_Floor` 정적 메시 애셋 자체의 기본 머터리얼을 바꿔 전역 부작용을 만드는 방식
- 첫 번째 `StaticMeshActor`를 `Floor`라고 가정하고 덮어쓰는 방식
- `Floor`를 찾지 못했는데도 성공으로 처리하는 방식
- 에디터에서 수동 클릭으로만 텍스처를 임포트하고 코드 경로를 생략하는 방식

## 메모
- 본 문서는 구현 코드가 아니라 작업 지시용 계획 문서다.
- 실제 클래스명과 파일 경로는 프로젝트 규칙에 맞게 약간 조정할 수 있다.
- 다만 아래 기준은 유지한다.
  - 원본 소스는 `SourceArt/T_Stylized_Grass_Dirt_01.png`를 사용해야 한다.
  - 텍스처 애셋과 머터리얼 애셋은 실제 프로젝트 `Content` 아래에 존재해야 한다.
  - `BasicMap`의 `Floor` 액터 라벨을 기준으로 정확히 적용해야 한다.
  - 적용 범위는 액터 인스턴스 오버라이드에 한정해야 한다.
  - 반복 실행 가능한 에디터 모듈 코드 + `Commandlet` 경로를 유지해야 한다.
