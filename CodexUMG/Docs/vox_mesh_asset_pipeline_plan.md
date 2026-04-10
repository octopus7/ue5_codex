# VOX 테스트 메시 에셋 생성 계획

## 문서 목적

플레이어와 적 테스트에 사용할 저해상도 VOX 메시를 제작하고, 이를 `CodexUMGBootstrapEditor` 에디터 모듈의 코드와 커맨드렛을 통해 UE5 프로젝트용 `StaticMesh` 에셋으로 생성하는 작업 계획을 정리한다.

이번 계획의 핵심 목표는 아래와 같다.

1. 원본 VOX 소스는 `32 x 32 x 32` 해상도로 통일한다.
2. 색상은 머터리얼 색상이 아니라 버텍스 컬러로 표현한다.
3. 메시마다 색상별 머터리얼 슬롯을 늘리지 않고, 모든 메시가 공용 VOX 머터리얼 하나를 사용한다.
4. 생성은 실행 중인 에디터 UI 세션이 없는 상태를 전제로 `Commandlet` 로 수행한다.
5. 에디터 실행 중 충돌로 보이는 에러가 나면 사용자에게 즉시 알리고 중단한다.

## 대상 에셋 목록

### 캐릭터 / 아이템
- 흰닭
- 갈색닭
- 병아리
- 돼지
- 사과
- 병우유
- 딸기
- 바나나
- 포도송이
- 생고기
- 뼈가있는구은고기

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

## 최종 산출물

### 소스 아트
- `SourceArt/Vox/` 아래에 `32^3` 규격의 `.vox` 파일을 둔다.
- 반복 배치 요소를 포함한 모든 모델은 1개 파일당 1개 오브젝트 기준으로 관리한다.
- 소스 파일명과 생성될 UE 에셋명은 1:1 매핑한다.

### UE 에셋
- 공용 머터리얼
  - `/Game/Vox/Materials/M_VoxVertexColor`
- 정적 메시
  - `/Game/Vox/Meshes/Characters/`
  - `/Game/Vox/Meshes/Food/`
  - `/Game/Vox/Meshes/Props/`
  - `/Game/Vox/Meshes/Foliage/`
  - `/Game/Vox/Meshes/Ground/`
- 생성 규칙 메타데이터 또는 매니페스트
  - `SourceArt/Vox/VoxAssetManifest.json`

## 에셋 명세

### 색상 처리
- `.vox` 팔레트 색상은 각 메시의 버텍스 컬러로 기록한다.
- 머터리얼은 `Vertex Color` 노드의 RGB를 `Base Color` 에 연결하는 공용 머터리얼 하나만 사용한다.
- 메시당 머터리얼 슬롯은 기본적으로 1개만 둔다.
- 색상별 머터리얼을 여러 개 만드는 구조는 채택하지 않는다.

### 메시 생성 규칙
- 각 채워진 복셀은 기본 큐브 셀로 간주한다.
- 메시 생성 시 내부 면은 제거하고 외부로 드러나는 면만 생성한다.
- 1차 구현은 단순 노출면 생성 방식으로 진행한다.
- 해상도가 `32^3` 고 테스트 자산 수가 제한적이므로, 1차 구현에서 그리디 메시까지 강제하지 않는다.
- 삼각형 수가 예상보다 커질 때만 2차 최적화 항목으로 그리디 메시 병합을 검토한다.

### 피벗 및 배치 규칙
- 동물과 음식은 바닥 기준 중앙 피벗을 사용한다.
- 꽃, 풀, 덤불, 수련은 지면 접점이 `Z=0` 에 오도록 맞춘다.
- 울타리는 연결 방향이 명확하도록 로컬 `+X` 방향을 전면으로 통일한다.
- 조약돌 바닥은 반복 배치 전용 타일로 보고 경계가 어색하지 않도록 제작한다.

## 실제 프로젝트 기준 구현 방향

현재 프로젝트에는 `CodexUMGBootstrapEditor` 모듈이 이미 존재하고, `AssetTools`, 팩토리, 저장 유틸을 이용해 블루프린트와 데이터 에셋을 만드는 패턴이 잡혀 있다. 이번 기능은 그 패턴을 재사용하되, VOX 파싱과 `StaticMesh` 빌드 부분만 신규 구현으로 추가한다.

### 재사용할 기존 패턴
- 디렉터리 생성
- 기존 에셋 존재 여부 확인
- 에셋 생성
- 저장
- 필요 시 부트스트랩 진입점과 헤드리스 진입점 분리

### 신규로 추가할 핵심 구성
- VOX 파일 파서
- VOX 생성용 매니페스트 로더
- 버텍스 컬러 포함 `StaticMesh` 빌더
- 공용 VOX 머터리얼 생성 또는 갱신 로직
- 헤드리스 실행용 `UCommandlet`

## 제안 파일 구조

### 에디터 모듈 코드
- `Source/CodexUMGBootstrapEditor/Public/Commandlets/CodexVoxAssetBuildCommandlet.h`
- `Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexVoxAssetBuildCommandlet.cpp`
- `Source/CodexUMGBootstrapEditor/Public/Vox/CodexVoxTypes.h`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxParser.cpp`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxMeshBuilder.cpp`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxAssetGenerator.cpp`
- `Source/CodexUMGBootstrapEditor/Private/Vox/CodexVoxMaterialBuilder.cpp`

### 보조 파일
- `SourceArt/Vox/VoxAssetManifest.json`
- `Scripts/RunVoxAssetBuild.ps1`

## 매니페스트 설계

매니페스트는 원본 `.vox` 와 목적지 UE 에셋 경로를 명시하고, 카테고리와 반복 배치 여부를 함께 관리한다.

예시 필드:

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

예시 ID:

- `SM_Vox_WhiteChicken`
- `SM_Vox_BrownChicken`
- `SM_Vox_Chick`
- `SM_Vox_Pig`
- `SM_Vox_Apple`
- `SM_Vox_MilkBottle`
- `SM_Vox_Strawberry`
- `SM_Vox_Banana`
- `SM_Vox_GrapeCluster`
- `SM_Vox_RawMeat`
- `SM_Vox_CookedMeatWithBone`
- `SM_Vox_Fence`
- `SM_Vox_Bush`
- `SM_Vox_Grass`
- `SM_Vox_YellowFlower`
- `SM_Vox_WhiteFlower`
- `SM_Vox_RedFlower`
- `SM_Vox_Sunflower`
- `SM_Vox_PebbleGroundTile`
- `SM_Vox_WaterLily`

## 커맨드렛 설계

### 커맨드렛 이름
- 클래스명: `UCodexVoxAssetBuildCommandlet`
- 실행명: `CodexVoxAssetBuild`

### 실행 예시

```powershell
UnrealEditor-Cmd.exe "D:\github\ue5_codex\CodexUMG\CodexUMG.uproject" -run=CodexVoxAssetBuild -Manifest="D:\github\ue5_codex\CodexUMG\SourceArt\Vox\VoxAssetManifest.json" -unattended -nop4 -nosplash
```

### 기본 동작 순서
1. 입력 인자와 매니페스트 경로를 검증한다.
2. 실행 중인 일반 에디터 세션이 없는지 사전 점검한다.
3. 공용 머터리얼 `M_VoxVertexColor` 를 생성하거나 갱신한다.
4. 매니페스트를 순회하며 `.vox` 파일을 읽는다.
5. 각 복셀 데이터에서 외부 면만 추출해 메시 데이터를 만든다.
6. 면 색상을 버텍스 컬러에 기록한다.
7. `StaticMesh` 에셋을 생성 또는 갱신한다.
8. 공용 머터리얼을 슬롯 0에 지정한다.
9. 저장 후 결과를 리포트한다.
10. 하나라도 치명적 오류가 나면 즉시 실패 코드로 종료한다.

## 에디터 실행 중 충돌 대응

이번 기능은 에디터가 실행 중이지 않다는 가정에서 동작해야 한다. 따라서 정상 경로는 항상 헤드리스 커맨드렛 실행이다.

### 사전 점검
- `Scripts/RunVoxAssetBuild.ps1` 에서 `UnrealEditor.exe` 또는 같은 프로젝트를 잡고 있는 관련 프로세스를 먼저 확인한다.
- 실행 중인 에디터가 감지되면 커맨드렛을 실행하지 않고 아래 메시지로 종료한다.

권장 메시지:

`CodexUMG 에디터 세션이 실행 중이므로 VOX 에셋 빌드를 중단합니다. 에디터를 종료한 뒤 다시 실행하세요.`

### 런타임 에러 대응
- 사전 점검을 통과했더라도 패키지 저장 실패, 파일 잠금, 에셋 레지스트리 충돌, 이미 열려 있는 에디터 세션 관련 에러가 발생하면 그 즉시 중단한다.
- 커맨드렛은 실패 원인을 로그에 남기고 비정상 종료 코드를 반환한다.
- 사용자에게는 "에디터 실행 상태 또는 파일 잠금으로 인해 중단되었다" 는 점을 분명히 알린다.
- 실패한 상태에서 일부 에셋만 생성된 경우, 다음 실행에서 안전하게 덮어쓸 수 있게 생성 로직을 idempotent 하게 설계한다.

## 공용 머터리얼 설계

### 목표
- 모든 VOX 메시가 동일한 공용 머터리얼 하나를 사용한다.
- 메시의 시각적 차이는 오직 버텍스 컬러로 표현한다.

### 머터리얼 구성
- `Base Color` <- `Vertex Color RGB`
- `Opacity Mask` 는 사용하지 않는다.
- 기본 표면은 불투명 `Opaque` 로 둔다.
- `Roughness` 는 상수값으로 단순화한다.
- 필요 시 `AO`, `Metallic`, `Specular` 는 상수로 고정한다.

### 구현 방안
- 1안: 코드로 `UMaterial` 과 필요한 표현식을 생성한다.
- 2안: 템플릿 머터리얼을 하나 두고, 커맨드렛은 존재 확인과 할당만 담당한다.

초기 구현은 재현성과 자동화를 위해 1안을 우선 검토하되, 머터리얼 그래프 생성 코드가 과하게 복잡해지면 2안으로 즉시 전환한다.

## 메시 빌드 세부 방향

### 1차 구현
- `.vox` 의 복셀 occupancy 와 팔레트를 읽는다.
- 각 복셀에 대해 6면 중 외부 노출면만 검사한다.
- 각 면을 4개 버텍스와 2개 삼각형으로 만든다.
- 각 버텍스에 동일한 면 색을 버텍스 컬러로 기록한다.
- 노멀과 UV는 단순 기본값으로 구성한다.
- 콜리전은 자동 생성 또는 단순 박스 기반으로 시작한다.

### 2차 구현 후보
- 그리디 메시 병합
- 카테고리별 충돌 설정 차등화
- 자동 LOD 또는 Nanite 검토
- 반복 배치 요소 전용 피벗/스냅 규칙 강화

이번 범위에서는 1차 구현만 완료해도 목적 달성으로 본다.

## 구현 단계

### 1단계. 소스 아트 기준 확정
- `32^3` 해상도와 피벗 규칙을 고정한다.
- 자산명과 카테고리를 확정한다.
- `.vox` 파일 제작 규칙을 문서화한다.

### 2단계. 매니페스트와 디렉터리 준비
- `SourceArt/Vox/` 디렉터리를 추가한다.
- `VoxAssetManifest.json` 에 전체 목록을 등록한다.
- `/Game/Vox/Materials` 와 `/Game/Vox/Meshes/*` 경로를 준비한다.

### 3단계. VOX 파서 구현
- `.vox` 파일에서 크기, 복셀, 팔레트 정보를 읽는다.
- `32^3` 규격 위반 시 실패 처리한다.
- 필수 chunk 누락 시 실패 처리한다.

### 4단계. 메시 생성기 구현
- 복셀 데이터를 `MeshDescription` 또는 동등한 생성 경로로 변환한다.
- 외부 면만 만들고 버텍스 컬러를 채운다.
- 머터리얼 슬롯은 1개만 유지한다.

### 5단계. 공용 머터리얼 생성
- `M_VoxVertexColor` 존재 여부를 확인한다.
- 없으면 생성하고, 있으면 구조가 요구사항과 맞는지 검증한다.
- 메시 빌드 결과는 항상 이 머터리얼을 사용하게 한다.

### 6단계. 커맨드렛 구현
- 매니페스트 기반 일괄 생성 진입점을 만든다.
- 실패 시 즉시 중단하고 0 이외의 종료 코드를 반환한다.
- 에디터 실행 충돌 관련 메시지를 명확하게 남긴다.

### 7단계. 검증
- 전체 목록이 지정 경로에 생성되는지 확인한다.
- 머터리얼 슬롯이 1개인지 확인한다.
- 버텍스 컬러가 보이는지 확인한다.
- 반복 배치 요소의 피벗과 스냅 감각을 점검한다.

## 에이전트 병렬 처리 전략

작업 중 에이전트를 사용할 수 있다면 병렬 분해로 리드타임을 줄인다. 단, 같은 파일을 여러 에이전트가 동시에 수정하지 않도록 책임 범위를 분리한다.

### 권장 분해
- Agent A
  - `SourceArt/Vox/*.vox` 원본 제작
  - `VoxAssetManifest.json` 초안 작성
- Agent B
  - `CodexVoxParser` 와 관련 타입 구현
- Agent C
  - `CodexVoxMeshBuilder` 와 `CodexVoxAssetGenerator` 구현
- Agent D
  - `M_VoxVertexColor` 생성 로직과 커맨드렛 진입점 구현
- Agent E
  - 검증 체크리스트 정리와 실패 메시지/운영 스크립트 작성

### 병렬 처리 원칙
- 즉시 막히는 핵심 경로는 메인 작업자가 직접 처리한다.
- 에이전트에는 독립 파일 집합만 맡긴다.
- 머지 시에는 다른 에이전트의 변경을 되돌리지 않는다.
- 공용 타입 파일은 가능한 늦게 확정하지 말고 먼저 잠근다.
- 리뷰 전용 에이전트는 구현과 분리한다.

## 범위 밖

이번 계획에서는 아래 항목을 필수 범위로 보지 않는다.

- 애니메이션 지원
- 복셀 파괴 표현
- 색상 교체용 머터리얼 인스턴스 체계
- 런타임 동적 생성
- 복잡한 자동 리토폴로지

## 완료 조건

아래가 모두 만족되면 1차 완료로 본다.

1. 목록의 모든 대상에 대해 `32^3` 기반 `.vox` 소스가 준비된다.
2. 커맨드렛 1회 실행으로 공용 머터리얼과 메시 에셋이 생성된다.
3. 모든 메시는 버텍스 컬러를 사용하고 머터리얼 슬롯은 1개다.
4. 생성 실패 시 즉시 멈추고 원인을 사용자에게 알린다.
5. 에디터 실행 중 충돌로 보이는 경우 작업을 계속하지 않는다.
6. 반복 배치 요소가 최소한 테스트 맵에서 무리 없이 배치된다.
