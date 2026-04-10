# VOX 메시 애셋 생성 계획

## 문서 목적
플레이어/적 테스트용 VOX 메시를 `32 x 32 x 32` 규격으로 준비하고, `CodexUMGBootstrapEditor` 에디터 모듈 코드와 커맨드렛을 이용해 UE5 프로젝트용 `StaticMesh` 애셋으로 생성하는 구현 계획과 운영 기준을 정리한다.

이 문서는 단순 아이디어 메모가 아니라, 다음 실행 때 같은 문제가 반복되지 않도록 좌표계, winding, 노멀, 피벗, 파일명 규칙, 검증 절차까지 포함하는 기준 문서다.

## 목표
1. 모든 원본 VOX 소스는 `32^3` 해상도로 통일한다.
2. 색상은 버텍스 컬러로만 표현한다.
3. 모든 메시가 공용 머터리얼 `M_VoxVertexColor` 하나만 사용하도록 만든다.
4. 생성은 실행 중인 에디터 UI 세션이 없는 상태에서 `Commandlet`로 수행한다.
5. 에디터 실행 중 충돌 가능성이 있으면 즉시 사용자에게 알리고 작업을 중단한다.
6. 다음 실행에서도 메시가 눕거나, 노멀이 뒤집히거나, 파일 연결이 틀어지지 않도록 재발 방지 기준을 문서화한다.
7. 샘플 팔레트는 전체적으로 이전보다 높은 채도로 유지하고, SourceArt 단계에서 비스듬한 미리보기 PNG도 함께 생성한다.

## 대상 애셋 목록

### 캐릭터 / 동물 / 생물
- 흰닭
- 갈색닭
- 병아리
- 돼지

### 음식 / 아이템
- 사과
- 병우유
- 딸기
- 바나나
- 포도송이
- 생고기
- 뼈가 있는 구운 고기

### 소품 / 구조물
- 나무 팻말

### 반복 배치 가능한 요소
- 울타리
- 덤불
- 풀
- 노란꽃
- 흰꽃
- 붉은꽃
- 해바라기
- 조약돌 바닥
- 수련

### 진단 / 검증용
- 고채도 무지개 + 16단계 그레이 진단 메시

## 산출물

### SourceArt
- `SourceArt/Vox/Sources/` 아래의 `.vox` 파일
- 샘플 생성 스크립트: `Scripts/GenerateSampleVoxSources.ps1`
- 비스듬한 미리보기 PNG 생성 스크립트: `Scripts/GenerateVoxPreviewPngs.py`
- 매니페스트: `SourceArt/Vox/VoxAssetManifest.json`
- 미리보기 출력: `SourceArt/Vox/Previews/<Category>/SM_Vox_*.png`

### UE 애셋
- 공용 머터리얼
  - `/Game/Vox/Materials/M_VoxVertexColor`
- 정적 메시
  - `/Game/Vox/Meshes/Characters/`
  - `/Game/Vox/Meshes/Food/`
  - `/Game/Vox/Meshes/Props/`
    - `/Game/Vox/Meshes/Props/SM_Vox_WoodenSignpost`
    - `/Game/Vox/Meshes/Props/SM_Vox_RainbowDiagnostic`
  - `/Game/Vox/Meshes/Foliage/`
  - `/Game/Vox/Meshes/Ground/`

## UE 애셋 명세

### 색상 처리
- `.vox` 팔레트 색상은 메시의 버텍스 컬러로 기록한다.
- 머터리얼은 `Vertex Color RGB -> Base Color` 구조의 공용 머터리얼 하나만 사용한다.
- 색상별 머터리얼을 여러 개 생성하거나, 메시 하나가 많은 머터리얼 슬롯을 가지는 구조는 채택하지 않는다.
- `XYZI`의 `ColorIndex`는 리터럴 RGB 값이 아니라 VOX 팔레트 인덱스다.
- 따라서 메시 빌더는 `Voxel.ColorIndex -> Palette[ColorIndex]` 매핑으로 실제 색을 읽어야 한다.
- VOX 팔레트의 RGB 값은 보통 sRGB 기준으로 authored 되므로, `MeshDescription`에 기록할 때는 선형 색으로 변환해서 버텍스 컬러에 넣어야 한다.
- 즉 `FColor`를 단순히 `/255`로 나눈 값을 쓰지 않고, `sRGB -> Linear` 변환 후 기록한다.

### 버텍스 컬러 컬러스페이스 규칙
- UE의 `MeshDescription` 버텍스 컬러 속성은 선형 색공간 기준으로 다룬다.
- 샘플 VOX 팔레트는 sRGB authored color로 취급한다.
- 따라서 구현 기준은 아래와 같다.
  - 입력: `.vox` 팔레트의 `FColor` sRGB 값
  - 저장: `FLinearColor::FromSRGBColor(...)`로 변환한 선형 버텍스 컬러
  - 머터리얼: `Vertex Color -> custom sRGB-to-linear decode -> Base Color`
- VOX 파이프라인에서는 이 머터리얼 디코드까지 포함해야 UE의 정적 메시 버텍스 컬러가 authored sRGB와 같은 체감 밝기로 나온다.
- UE 정적 메시 빌드 경로는 버텍스 컬러를 다시 sRGB 바이트로 양자화해 저장할 수 있으므로, 공유 머터리얼에서 이 디코드를 빼면 `FenceBrown`, 중간 그레이 같은 중간톤이 authored 값보다 밝게 뜰 수 있다.

### 절대 금지 사항
- sRGB 팔레트 색을 `R/255`, `G/255`, `B/255`로만 나눠서 선형 버텍스 컬러처럼 저장하지 않는다.
  - 이 경우 화면에서 감마가 한 번 더 적용되어 색이 뿌옇고 파스텔처럼 뜬다.
- "UE에서 밝게 뜨니 `FLinearColor::FromSRGBColor(...)`를 제거하자"는 식으로 해결하지 않는다.
  - 이 변경은 원색에는 티가 덜 나도 갈색, 회색, 아이보리 같은 중간톤을 더 밝게 밀어 올릴 수 있다.
- 공유 VOX 머터리얼에서 wide-gamut 변환용 노드를 감마 디코드 대용으로 오해해 사용하지 않는다.
  - 기본 working color space가 sRGB일 때는 원하는 감마 디코드가 일어나지 않을 수 있다.
- `Vertex Color`를 보정 없이 바로 `Base Color`에 꽂아 중간톤이 밝게 뜨는 상태를 정상으로 간주하지 않는다.
- 감마 문제를 머터리얼 슬롯 추가나 색상별 머터리얼 분기로 해결하지 않는다.
  - 원인은 색공간 변환 경로이므로, 해결은 공유 VOX 머터리얼과 버텍스 컬러 저장 규칙을 같은 기준으로 맞추는 방식이어야 한다.

### VOX 팔레트 직렬화 규칙
- 샘플 `.vox`는 MagicaVoxel `RGBA` 청크 규칙을 그대로 따른다.
- `RGBA` raw entry `0`은 palette index `1`에 대응한다.
- `RGBA` raw entry `255`는 unused 슬롯으로 둔다.
- 따라서 샘플 생성 스크립트는 `RGBA` 청크 앞에 더미 색을 하나 밀어 넣지 않는다.
- `XYZI.ColorIndex`는 `1-based` 팔레트 인덱스처럼 동작하므로, 샘플 소스 작성과 파서 구현 모두 이 규칙을 기준으로 맞춘다.
- 샘플 생성기에서 색 배열 첫 항목은 실제로 palette index `1`에 들어갈 색이어야 한다.

### 팔레트 재발 방지 규칙
- 샘플 생성 스크립트 수정 시 `RGBA` 청크와 `XYZI.ColorIndex`가 한 칸 밀리지 않는지 먼저 확인한다.
- 딸기처럼 색이 분명한 샘플을 기준 검증용으로 사용한다.
  - `SM_Vox_Strawberry` 본체는 빨강
  - `SM_Vox_Strawberry` 꼭지는 초록
- 원색 경로와 무채색 계조 재현을 같이 검증하기 위해 별도 진단 메시를 항상 함께 생성한다.
  - `SM_Vox_RainbowDiagnostic` 상단 절반은 순수 sRGB `255/0/0`, `255/127/0`, `255/255/0`, `0/255/0`, `0/255/255`, `0/0/255`, `255/0/255` 줄무늬를 사용한다.
  - 같은 메시의 하단 절반은 `0, 17, 34, ..., 255` 값의 16단계 중성 그레이 램프를 좌에서 우로 배치한다.
  - 상단 원색이 `Unlit`에서도 선명하면 기존 샘플의 저채도는 SourceArt 의도일 가능성이 높다.
  - 하단 그레이 램프까지 전체적으로 들뜨거나 파스텔처럼 뜨면 `sRGB -> Linear` 변환이나 버텍스 컬러 저장 경로를 먼저 점검한다.
- 현재 샘플 팔레트는 테스트 가독성을 위해 전체적으로 높은 채도로 유지한다.
  - 예: `Red = (248, 48, 48)`, `LeafGreen = (84, 224, 78)`, `BananaYellow = (250, 224, 52)`
  - 다만 `WhitePetal`, `Bone`, `PebbleGray` 같은 중성색은 재질 성격상 완전 원색으로 밀지 않는다.
- 채도를 더 조정할 때는 머터리얼이나 버텍스 컬러 저장 경로를 건드리지 말고 `Scripts/GenerateSampleVoxSources.ps1`의 팔레트 정의를 수정한 뒤 `.vox`와 미리보기 PNG를 같이 재생성한다.
- 메시가 흐릿한 파스텔 톤이나 전혀 다른 색으로 보이면 머터리얼보다 먼저 팔레트 오프셋 버그를 의심한다.
- 팔레트 인덱스는 맞는데 전체가 밝고 뿌옇게 뜨면 컬러스페이스 경로를 먼저 의심한다.
  - 대표 증상:
  - 빨강이 연분홍으로 보임
  - 초록이 민트색처럼 보임
  - Unlit인데도 전체가 파스텔 톤처럼 뜸

### 메시 생성 기본 규칙
- 내부에 가려지는 면은 제거하고 외부로 드러나는 면만 메시로 생성한다.
- 1차 구현은 단순 face extraction 방식으로 진행한다.
- 각 면은 4개의 코너와 2개의 삼각형으로 생성한다.
- 같은 면의 4개 버텍스 인스턴스에는 동일한 VOX 색을 버텍스 컬러로 기록한다.

## 좌표계, Winding, 피벗 규칙

이 섹션은 재발 방지용 핵심 기준이다. 메시가 눕거나, 노멀이 뒤집히거나, 썸네일이 비정상적으로 보이면 먼저 이 규칙부터 확인한다.

### VOX 원본 좌표계
- 샘플 `.vox`는 `Y-up` 기준으로 만든다.
- 즉 VOX 원본에서 높이축은 `+Y`다.
- 닭, 돼지, 병우유 같은 세로형 오브젝트는 높이 방향이 VOX `Y`축이다.
- 울타리의 길이 방향은 기본적으로 `+X`를 사용한다.

### UE 변환 좌표계
- UE는 `Z-up` 기준이므로 가져올 때 축 변환을 반드시 적용한다.
- 위치 변환 공식:
  - `VOX (X, Y, Z) -> UE (X, -Z, Y) * VoxelSize`
- 방향 벡터 변환 공식:
  - `VOX (X, Y, Z) -> UE (X, -Z, Y)`
- 노멀과 탄젠트도 위치와 동일한 축 변환을 적용해야 한다.

### UE winding 규칙
- UE static mesh는 좌수계 좌표계와 `CCW front face` 기준을 사용한다.
- 따라서 VOX 코너를 UE 축으로 변환한 뒤에는 각 면의 삼각형 winding을 한 번 반전해서 기록해야 한다.
- quad 분할 예시:
  - 원본 순서가 `(0, 1, 2)` / `(0, 2, 3)`이면
  - UE 기록은 `(0, 2, 1)` / `(0, 3, 2)`를 사용한다.
- 이 규칙을 빼먹으면 메시 자세는 정상이어도 front face와 노멀 방향이 반대로 보일 수 있다.

### 삼각형 인덱스 재발 방지 규칙
- 이 프로젝트에서는 축 변환식, face corner 순서, 최종 triangle index 순서를 한 세트로 취급한다.
- 현재 기준 구현에서 face corner 순서를 유지하는 한, 최종 triangle index 기준값은 `(0, 2, 1)` / `(0, 3, 2)`다.
- `FaceDefinitions` 또는 `GetFaceCorners`의 corner 순서를 바꾸면, triangle index도 함께 다시 계산해야 한다.
- corner 순서를 그대로 둔 채 triangle index 기본값만 `(0, 1, 2)` / `(0, 2, 3)`로 바꾸지 않는다.
  - 이 경우 여섯 방향 face가 전부 안쪽을 향하는 회귀가 다시 발생할 수 있다.
- triangle index를 조건부로 뒤집는 로직을 넣더라도, 기준은 "변환된 코너로 계산한 후보 노멀"과 "변환된 면 노멀"이 같은 바깥 방향을 유지하는지 여부여야 한다.
- 구현 단순화를 이유로 "face corner 배열 재정의"와 "triangle 반전 로직 변경"을 동시에 적용하지 않는다.
  - 둘 다 함께 바꾸면 겉보기에는 대칭처럼 보여도 실제 front face가 다시 반대로 뒤집힐 수 있다.
- `CodexVoxMeshBuilder` 리팩터링 시 아래 네 항목은 항상 같이 검토한다.
  - `TransformPosition`
  - `TransformDirection`
  - face별 corner 순서
  - `AppendTriangle`에 넘기는 최종 인덱스 순서

### 절대 금지 사항
- VOX 좌표를 `UE (X, Y, Z)`로 그대로 사용하지 않는다.
  - 이 경우 `Y-up` 원본이 UE에서 옆으로 눕는다.
- 축 변환만 하고 winding 반전을 생략하지 않는다.
  - 이 경우 노멀, 셰이딩, culling 방향이 뒤집힐 수 있다.
- winding을 두 번 반전하지 않는다.
  - 필요한 반전은 한 번뿐이며, 중복 반전은 다시 잘못된 방향을 만든다.
- 피벗 계산에서 높이축을 VOX `Z`로 가정하지 않는다.
  - 샘플 VOX 소스의 높이축은 VOX `Y`다.

### 피벗 규칙
- `GroundCentered`
  - 바닥 접지 기준 피벗
  - VOX 기준 최저 `Y`가 UE에서 `Z=0`이 되도록 맞춘다.
- `Centered`
  - 모델 중심 피벗
  - VOX 기준 `Y`를 포함한 전체 bounding box 중심을 사용한다.

## 좌표계 및 노멀 재발 방지 체크리스트
메시 재생성 후 아래 항목을 반드시 확인한다.

1. `SM_Vox_WhiteChicken`, `SM_Vox_Pig` 썸네일이 옆으로 누워 있지 않아야 한다.
2. 캐릭터류 메시는 `Z extent`가 `Y extent`보다 크거나 비슷해야 한다.
3. `SM_Vox_PebbleGroundTile`는 납작한 바닥 메시여야 하며 `Z extent`가 작아야 한다.
4. 메시 표면이 바깥이 아니라 안쪽처럼 보이면 winding/노멀 반전 가능성을 먼저 본다.
5. Static Mesh Editor에서 face normal 표시를 켰을 때 노멀이 바깥을 향해야 한다.
6. Static Mesh Editor에서 `Show > Advanced > Vertex Colors`를 켰을 때 색이 정상적으로 보인다.
7. `SM_Vox_Strawberry`를 Unlit으로 봤을 때 본체가 노랑/아이보리가 아니라 빨강이어야 한다.
8. `SM_Vox_Strawberry` 꼭지가 민트색이 아니라 초록이어야 한다.
9. `SM_Vox_RainbowDiagnostic`를 Unlit으로 봤을 때 상단 절반의 빨강, 주황, 노랑, 초록, 시안, 파랑, 마젠타 줄무늬가 탁하지 않고 강하게 보여야 한다.
10. 같은 메시 하단 절반의 16단계 그레이가 검정에서 흰색까지 뭉개지지 않고 단계적으로 분리되어 보여야 한다.
11. 딸기만 부드럽고 `SM_Vox_RainbowDiagnostic`의 상단/하단 진단 패턴이 모두 정상이라면 기존 샘플 팔레트가 원래 저채도인 것이다.
12. 색이 전반적으로 한 칸씩 밀린 것처럼 보이면 `.vox` `RGBA` 청크 직렬화부터 재검토한다.
13. 레인보우 메시나 그레이 램프까지 전체적으로 밝고 뿌옇게 보이면 `sRGB -> Linear` 변환 누락 여부를 확인한다.
14. `SM_Vox_Pig`, `SM_Vox_WhiteChicken`, `SM_Vox_Fence`를 열었을 때 내부 면만 보이는 느낌이나 backface culling 이상이 없어야 한다.
15. VOX 메시 빌더를 수정했다면, 최소 한 번은 `Face Normal` 표시와 `Unlit` 보기 둘 다로 앞면 방향을 확인한다.

현재 구현 기준 기대 확인값 예시:
- `SM_Vox_Pig`: `extent=(42.5, 22.5, 32.5)`
- `SM_Vox_WhiteChicken`: `extent=(32.5, 17.5, 37.5)`
- `SM_Vox_PebbleGroundTile`: `extent=(80.0, 80.0, 10.0)`

## 구현 방향

### 모듈 구성
- `Source/CodexUMGBootstrapEditor/Public/Commandlets/CodexVoxAssetBuildCommandlet.h`
- `Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexVoxAssetBuildCommandlet.cpp`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxTypes.h`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxParser.h`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxMeshBuilder.h`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxMaterialBuilder.h`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxAssetGenerator.h`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxParser.cpp`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxMeshBuilder.cpp`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxMaterialBuilder.cpp`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxAssetGenerator.cpp`

### 구성요소 책임
- `CodexVoxParser`
  - `.vox` 파일에서 `SIZE`, `XYZI`, `RGBA`를 읽는다.
  - 단일 모델만 사용한다.
  - `32^3` 해상도 검증을 수행한다.
  - `XYZI.ColorIndex`와 `RGBA` 팔레트의 MagicaVoxel 인덱싱 규칙을 그대로 따른다.
- `CodexVoxMeshBuilder`
  - 외부 face만 추출한다.
  - 버텍스 컬러를 기록한다.
  - VOX 팔레트 sRGB 색을 선형 버텍스 컬러로 변환해서 저장한다.
  - `VOX -> UE` 축 변환, winding 반전, 피벗 보정을 담당한다.
  - face corner 순서와 triangle index 순서를 독립적으로 바꾸지 않도록 유지한다.
  - 리팩터링 후에도 기준 인덱스 `(0, 2, 1)` / `(0, 3, 2)`가 바깥 면을 유지하는지 검증한다.
- `CodexVoxMaterialBuilder`
  - 공용 머터리얼 `M_VoxVertexColor`를 생성 또는 갱신한다.
- `CodexVoxAssetGenerator`
  - 매니페스트 로드
  - 에디터 세션 실행 여부 확인
  - 메시 생성/저장
  - 공용 머터리얼 연결
  - 기존 공용 머터리얼이나 기존 메시를 건드리지 않고 신규 메시만 추가해야 하는 예외 빌드 경로를 지원한다.
- `CodexVoxAssetBuildCommandlet`
  - 헤드리스 실행 진입점
  - `-Manifest=`, `-Verbose`, `-NoOverwrite`, `-SkipMaterialUpdate` 처리

### 개별 추가 예외 구현 원칙
- 기본 경로는 공용 파서, 공용 메시 빌더, 공용 머터리얼, 매니페스트 기반 생성 흐름을 유지한다.
- 다만 특정 메시 하나만 추가하거나, 사용자가 수정 중인 공용 VOX 머터리얼을 덮으면 안 되는 경우 같은 예외는 개별 추가 가능한 형태로 구현해야 한다.
- 예외는 전역 동작을 뒤집는 방식이 아니라, 영향 범위가 명확한 옵션이나 asset 단위 분기로 국소화한다.
- 샘플 SourceArt 단계에서는 `Build-<AssetName>` 형태의 개별 builder 함수를 추가해 형태 예외를 분리한다.
- UE 빌드 단계에서는 `-NoOverwrite`, `-SkipMaterialUpdate` 같은 옵션으로 기존 메시와 공용 머터리얼을 보존한 채 신규 애셋만 생성할 수 있어야 한다.
- 한 메시를 위한 예외 구현이 다른 VOX 메시의 기본 생성 규칙, 색 경로, 공용 머터리얼 규칙을 바꾸지 않도록 유지한다.

## 매니페스트 규칙

매니페스트는 `SourceArt/Vox/VoxAssetManifest.json`에 둔다.

### 필드
- `id`
- `displayName`
- `sourceVoxFile`
- `targetPackagePath`
- `targetAssetName`
- `category`
- `repeatable`
- `pivotRule`
- `collisionType`
- `notes`

### sourceVoxFile 규칙
- 샘플 생성 스크립트는 `SourceArt/Vox/Sources/SM_Vox_*.vox` 형식으로 파일을 만든다.
- 따라서 `sourceVoxFile`은 실제 생성 파일명과 정확히 일치해야 한다.
- 잘못된 예:
  - `Sources/white_chicken.vox`
- 올바른 예:
  - `Sources/SM_Vox_WhiteChicken.vox`

이 규칙을 어기면 커맨드렛이 정상 실행되더라도 `.vox` 파일을 찾지 못해 실패한다.

### SourceArt 팔레트 규칙
- 샘플 `.vox`를 다시 생성했으면, 변경된 SourceArt만으로 끝내지 않고 커맨드렛으로 UE 메시도 재생성해야 한다.
- 팔레트나 복셀 형태가 바뀌면 `SourceArt/Vox/Previews/` 아래의 비스듬한 PNG도 같이 재생성해 SourceArt 단계에서 색감과 실루엣을 먼저 확인한다.
- SourceArt와 UE 애셋 사이에 색이 어긋나면 먼저 `.vox` 팔레트와 `XYZI.ColorIndex` 매핑을 확인한다.
- 팔레트 검증 기준 예시:
  - 딸기 본체에 쓰는 `ColorIndex=5`는 빨강 계열이어야 한다.
  - 잎에 쓰는 `ColorIndex=12`는 초록 계열이어야 한다.
  - `SM_Vox_RainbowDiagnostic` 상단 줄무늬는 순서대로 빨강, 주황, 노랑, 초록, 시안, 파랑, 마젠타여야 한다.
  - 같은 메시 하단 줄무늬는 좌에서 우로 검정에서 흰색까지 16단계 그레이여야 한다.

## 비스듬한 미리보기 PNG

### 목적
- UE 에디터를 열기 전에 SourceArt 단계에서 색감, 실루엣, 팔레트 오프셋 이상을 빠르게 확인한다.
- `SM_Vox_Strawberry`와 `SM_Vox_RainbowDiagnostic`를 바로 비교해 authored palette 문제와 컬러스페이스 문제를 초기에 분리한다.

### 생성 방식
- `Scripts/GenerateVoxPreviewPngs.py`는 `VoxAssetManifest.json`을 읽고 각 `.vox` 파일을 파싱한다.
- 렌더링은 투명 배경의 비스듬한 orthographic voxel preview 기준으로 수행한다.
- 노출된 `Top`, `+X`, `+Z` face만 그려서 가벼운 검수용 이미지를 만든다.
- 프리뷰 PNG는 per-face 암부 셰이딩이나 복셀 격자선을 넣지 않고 authored sRGB 색을 평면적으로 보여준다.
- 즉 프리뷰 목표는 스타일화된 썸네일이 아니라 UE `Unlit` 색 검수와 최대한 비슷한 기준 이미지를 만드는 것이다.
- 출력은 `SourceArt/Vox/Previews/<Category>/SM_Vox_*.png` 형식으로 정리한다.

### 사용 예시
```powershell
.\Scripts\GenerateSampleVoxSources.ps1 -GeneratePreviews
```

```powershell
python .\Scripts\GenerateVoxPreviewPngs.py --manifest .\SourceArt\Vox\VoxAssetManifest.json --source-root .\SourceArt\Vox --output-root .\SourceArt\Vox\Previews
```

## 커맨드렛 설계

### 클래스명
- `UCodexVoxAssetBuildCommandlet`

### 실제 실행 토큰
- `CodexUMGBootstrapEditor.CodexVoxAssetBuildCommandlet`

### 실행 예시
```powershell
UnrealEditor-Cmd.exe "D:\github\ue5_codex\CodexUMG\CodexUMG.uproject" -run=CodexUMGBootstrapEditor.CodexVoxAssetBuildCommandlet -Manifest="D:\github\ue5_codex\CodexUMG\SourceArt\Vox\VoxAssetManifest.json" -unattended -nop4 -nosplash
```

### 개별 추가 예외 실행 예시
기존 메시와 공용 머터리얼은 유지하고, 아직 없는 신규 메시만 추가할 때는 아래 조합을 사용한다.

```powershell
.\Scripts\RunVoxAssetBuild.ps1 -ManifestPath .\SourceArt\Vox\VoxAssetManifest.json -NoOverwrite -SkipMaterialUpdate -VerboseBuild
```

### 동작 순서
1. 입력 인자와 매니페스트 경로를 검증한다.
2. 현재 프로젝트를 연 `UnrealEditor.exe` 프로세스가 있는지 확인한다.
3. 에디터가 실행 중이면 사용자에게 알리고 즉시 중단한다.
4. 공용 머터리얼 `M_VoxVertexColor`를 생성 또는 갱신한다. 단, 개별 추가 예외 빌드에서 `-SkipMaterialUpdate`를 주면 기존 공용 머터리얼을 그대로 사용한다.
5. 매니페스트를 순회하며 각 `.vox` 파일을 로드한다.
6. VOX 복셀 데이터를 외부 face 메시로 변환한다.
7. 버텍스 컬러와 공용 머터리얼 슬롯을 적용한다.
8. `StaticMesh` 애셋을 생성 또는 갱신한다. `-NoOverwrite`가 켜져 있으면 기존 애셋은 건너뛰고 없는 애셋만 새로 만든다.
9. 하나라도 치명 오류가 나면 즉시 실패 코드로 종료한다.

## 에디터 실행 중 충돌 대응
이 기능은 에디터가 닫혀 있다는 가정에서 동작한다.

### 러너 스크립트
- `Scripts/RunVoxAssetBuild.ps1`
- 역할:
  - 샘플 `.vox` 생성 옵션 처리
  - 실행 중인 `UnrealEditor.exe` 확인
  - 개별 추가 예외 빌드를 위한 `-NoOverwrite`, `-SkipMaterialUpdate` 옵션 전달
  - 커맨드렛 호출

### 중단 정책
- 같은 프로젝트를 연 에디터 세션이 보이면 작업을 멈춘다.
- 메시 예시:
  - `An Unreal Editor session for this project is running. Stop the editor and rerun the VOX asset build.`

## 공용 머터리얼 설계

### 목표
- 모든 VOX 메시가 동일한 머터리얼 하나를 사용한다.
- 시각 차이는 버텍스 컬러로만 표현한다.

### 머터리얼 구성
- `Base Color <- Vertex Color`
- `Blend Mode = Opaque`
- `Roughness = 1.0`
- `Specular = 0.0`

## 메시 빌드 방향

### 1차 구현
- 각 복셀에 대해 6방향 이웃을 검사한다.
- 이웃이 없거나 모델 바깥이면 해당 face를 생성한다.
- face당 4개 vertex instance와 2개 triangle을 생성한다.
- 버텍스 컬러는 face 단위로 동일값을 넣는다.
- UV는 공통 기본값만 유지하고, 셰이딩 문제는 winding과 노멀 기준으로 해결한다.
- face corner 배열을 유지한다면 triangle 생성 순서는 `(0, 2, 1)` / `(0, 3, 2)`를 기준값으로 사용한다.
- face corner 배열을 변경했다면 기존 인덱스를 그대로 재사용하지 말고, 여섯 방향 face 각각에서 바깥 노멀을 다시 검증한다.

### 빌드 세팅
- `bRecomputeNormals = false`
- `bRecomputeTangents = false`
- `bGenerateLightmapUVs = false`
- `bRemoveDegenerates = false`

직접 기록한 노멀/탄젠트와 winding 규칙이 기준이므로, 이 기준을 바꾸지 않는 한 재계산에 의존하지 않는다.
직접 기록한 버텍스 컬러도 선형값 기준이므로, sRGB 팔레트를 넣을 때는 먼저 선형 변환을 적용한다.

## 병렬 처리 및 에이전트 활용
작업 중 가능하면 에이전트를 이용해 병렬 처리하여 리드타임을 줄인다. 단, 쓰기 충돌이 없는 범위에서만 병렬화한다.

### 병렬화 후보
- SourceArt 샘플 `.vox` 생성 규칙 정리
- 매니페스트 검증
- 에셋 카테고리별 대상 목록 정리
- 문서/체크리스트 보강
- 읽기 전용 검증 스크립트 작성

### 병렬화 금지 대상
- 동일 파일을 동시에 수정하는 작업
- 동일 StaticMesh 애셋을 동시에 생성/저장하는 작업
- 공용 머터리얼 단일 애셋을 동시에 갱신하는 작업

### 권장 운영 방식
1. 메인 작업자는 커맨드렛/메시 빌더 구현을 담당한다.
2. 보조 에이전트는 문서, 매니페스트 점검, 샘플 소스 생성 규칙 검토 같은 부가 작업을 맡는다.
3. 결과 통합은 메인 작업자가 한 번에 수행한다.

## 플레이어 BP 연계 규칙
- 이 문서에서 다루는 갈색닭 메시의 플레이어 적용 시점은 순수 VOX 생성 단계가 아니라 Track A와 Track B가 만나는 통합 지점으로 취급한다.
- Track B에서 `/Game/Vox/Meshes/Characters/SM_Vox_BrownChicken` 생성이 끝났고, Track A에서 `/Game/Blueprints/Player/BP_Character_TopDown` 생성이 끝났으면 그 다음 단계에서 플레이어 BP에 메시를 연결한다.
- 연결 대상은 `BP_Character_TopDown`의 플레이어 가시성용 별도 `StaticMeshComponent`다.
- 메시 지정 값은 `/Game/Vox/Meshes/Characters/SM_Vox_BrownChicken`로 고정한다.
- 둘 중 하나라도 아직 없으면 억지로 선행하지 않고 대기한다.
- 이 연결 작업은 VOX 메시 생성 성공 직후가 아니라 "갈색닭 메시 생성 완료"와 "플레이어 BP 생성 완료" 두 조건이 모두 충족된 뒤 수행한다.

## 실제 실행 순서
1. `Scripts/GenerateSampleVoxSources.ps1`로 `32^3` 샘플 `.vox`를 준비한다.
2. 같은 단계에서 `-GeneratePreviews` 또는 `Scripts/GenerateVoxPreviewPngs.py`로 비스듬한 PNG 미리보기를 생성한다.
3. `SourceArt/Vox/VoxAssetManifest.json`을 작성하거나 재생성한다.
4. `SM_Vox_Strawberry.png`와 `SM_Vox_RainbowDiagnostic.png`를 먼저 확인해 색감과 팔레트 오프셋 이상이 없는지 본다.
5. `CodexUMGBootstrapEditor` 모듈에 parser/mesh builder/material builder/generator/commandlet을 구현한다.
6. `Scripts/RunVoxAssetBuild.ps1`로 에디터 실행 여부를 먼저 검사한다.
7. 에디터가 꺼져 있으면 커맨드렛을 실행해 애셋을 생성한다.
8. `SM_Vox_BrownChicken`와 `BP_Character_TopDown`가 모두 존재하면 `BP_Character_TopDown`의 별도 `StaticMeshComponent`에 갈색닭 메시를 지정한다.
9. 결과 메시를 Static Mesh Editor에서 열어 자세, 노멀 방향, 버텍스 컬러를 검증한다.
10. 갈색닭 메시를 지정한 플레이어 BP도 함께 열어 탑다운 시점에서 가시성, 크기, 오프셋이 적절한지 확인한다.
11. `SM_Vox_Strawberry`만 부드럽고 `SM_Vox_RainbowDiagnostic`의 상단 원색과 하단 그레이 램프가 모두 정상이라면 SourceArt 팔레트 의도로 판단한다.
12. `SM_Vox_RainbowDiagnostic` 상단 원색이나 하단 그레이 램프까지 연분홍/민트/파스텔처럼 보이면 버텍스 컬러 컬러스페이스 경로를 점검한다.
13. 좌표계, winding, 피벗, 팔레트, 컬러스페이스 규칙이 어긋난 사례가 나오면 먼저 이 문서 기준을 갱신하고 코드에 반영한다.

## 최종 검증 체크리스트
1. 모든 `.vox` 소스가 `32^3` 해상도다.
2. 모든 메시가 공용 머터리얼 `M_VoxVertexColor` 하나만 사용한다.
3. 메시의 색상은 머터리얼 슬롯 분기가 아니라 버텍스 컬러에서 보인다.
4. 흰닭, 갈색닭, 병아리, 돼지 메시가 올바르게 서 있다.
5. 울타리, 조약돌 바닥, 수련 같은 반복 배치 요소가 용도에 맞는 비율을 가진다.
6. 노멀 반전 없이 썸네일과 미리보기가 정상적으로 보인다.
7. 실행 중인 에디터가 있으면 커맨드렛이 즉시 실패하고 사용자에게 이유를 알린다.
8. 매니페스트 경로와 실제 `.vox` 파일명이 일치한다.
9. `SM_Vox_Strawberry` 본체는 빨강, 꼭지는 초록으로 보인다.
10. `SourceArt/Vox/Previews/` 아래의 비스듬한 PNG가 최신 `.vox`와 함께 재생성되어 있다.
11. `SM_Vox_RainbowDiagnostic` 상단은 순수 원색 줄무늬가 강한 채도로 보이고, 하단은 검정에서 흰색까지 16단계 그레이가 분리되어 보인다.
12. 색이 전반적으로 한 단계씩 밀린 듯 보이지 않는다.
13. 색이 전반적으로 파스텔처럼 뜨지 않는다.
14. `BP_Character_TopDown`가 존재하면, 플레이어 가시성용 별도 `StaticMeshComponent`가 `/Game/Vox/Meshes/Characters/SM_Vox_BrownChicken`를 참조한다.
15. 플레이어 BP에 갈색닭 메시를 연결한 뒤 탑다운 카메라 기준으로 실루엣 식별이 가능하다.

## 유지보수 메모
- VOX 샘플 생성 스크립트가 `Y-up` 기준을 유지하는지 먼저 확인한다.
- VOX 샘플 생성 스크립트가 `RGBA` 청크를 MagicaVoxel 규칙대로 직렬화하는지 먼저 확인한다.
- `VOX -> UE` 변환식이나 winding 규칙을 바꾸려면 캐릭터 썸네일, 바닥 타일, 울타리 결과를 다시 검증한다.
- 메시가 다시 눕거나 노멀이 뒤집히면 `CodexVoxMeshBuilder`의 축 변환, 삼각형 순서, 피벗 규칙부터 확인한다.
- triangle index 기본값을 바꾸거나 조건부 반전 로직을 단순화했다면, 기존 face corner 순서를 전제로 한 코드인지 먼저 확인한다.
- "코너 순서 변경"과 "인덱스 반전 로직 변경"이 한 번에 같이 들어갔다면, 둘 중 하나를 되돌려 원인을 분리한 뒤 다시 검증한다.
- 색이 이상하면 머터리얼보다 먼저 `GenerateSampleVoxSources.ps1`의 팔레트 엔트리 배치와 `CodexVoxParser`의 `RGBA` 해석부터 확인한다.
- 색이 탁하게 보이는 이슈는 `SM_Vox_Strawberry`와 `SM_Vox_RainbowDiagnostic`를 같이 열어 authored palette 문제인지 컬러스페이스 문제인지 먼저 분리한다.
- 팔레트나 복셀 실루엣을 바꾸면 UE를 보기 전에 `GenerateVoxPreviewPngs.py`로 비스듬한 PNG를 다시 뽑아 SourceArt 차이부터 확인한다.
- 색이 맞는 계열인데도 뿌옇고 밝게 뜨면 `CodexVoxMeshBuilder`의 `FromSRGBColor(...)`와 공유 VOX 머터리얼의 custom `sRGB -> linear` 디코드가 둘 다 유지되고 있는지 먼저 확인한다.
- 울타리, 회색 램프, 아이보리 계열이 authored palette보다 유난히 밝게 보이면 `FromSRGBColor(...)`를 제거하지 말고 공유 VOX 머터리얼 연결이 `Vertex Color -> custom sRGB-to-linear decode -> Base Color`인지 먼저 확인한다.
