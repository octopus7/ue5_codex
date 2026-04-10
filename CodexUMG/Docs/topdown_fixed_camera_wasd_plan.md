# 고정 탑다운 카메라 + WASD 이동 구조 계획

## 문서 목적

회전하지 않는 탑다운 카메라 시점에서 `WASD`로 이동하는 플레이 구조를 만든다.
입력은 `Enhanced Input` 기반으로 구성하고, `IA_` / `IMC_`는 실제 에셋으로 생성한다.
입력 에셋 참조는 플레이어 캐릭터나 플레이어 BP에 직접 박지 않고, `DA_` 에셋에 모은 뒤 `GameInstance` BP에 걸어 플레이어가 접근하는 구조로 정리한다.

이 문서는 바로 구현 코드로 들어가기 전에 구조를 고정하기 위한 초안이다.

## 핵심 제약

1. `IA_` 와 `IMC_` 는 실제 Content Browser 에셋으로 존재해야 한다.
2. 플레이어 캐릭터 / 플레이어 BP는 `IA_`, `IMC_`를 직접 기본값으로 들고 있지 않는다.
3. 입력 관련 실제 참조는 `DA_` 에셋에 모으고, 그 `DA_` 는 `BP_GameInstance`에 할당한다.
4. 플레이어는 `GameInstance`를 통해 필요한 입력 세트를 조회한다.
5. 모든 연결 지점은 네이티브 클래스 직접 참조 대신 BP로 실체화된 에셋을 우선 사용한다.
6. 에셋 생성은 런타임 코드가 아니라 에디터 모듈에서 수행한다.
7. 이 에디터 모듈은 프로젝트 초기 세팅을 부트스트랩하는 용도이므로, 자주 다시 실행하는 도구가 아니라 1회성 생성 도구로 본다.

## 추천 구조

### 1. 입력 소스 오브 트루스

`BP_GameInstance`가 입력 설정의 진입점이 된다.

참조 흐름:

`BP_GameInstance` -> `DA_TopDownInputConfig` -> `IMC_TopDown` / `IA_Move`

플레이어 쪽 원칙:

- 플레이어는 `GameInstance`에서 `DA_TopDownInputConfig`를 받아 쓴다.
- 플레이어는 `IA_Move` 에셋 경로를 하드코딩하지 않는다.
- 플레이어는 `IMC_TopDown`을 직접 기본값으로 소유하지 않는다.

### 2. 입력 적용 위치

`IMC`를 실제로 `Add Mapping Context` 하는 시점은 `LocalPlayer`가 유효한 뒤여야 하므로, 적용 실행 자체는 `BP_PlayerController`가 담당하는 편이 안전하다.

다만 참조 소유권은 아래처럼 분리한다.

- 소유: `BP_GameInstance`
- 조회: `BP_PlayerController`
- 소비: `BP_PlayerController` 또는 입력을 전달받는 `BP_Character`

즉, `GameInstance`가 입력 세트를 들고 있고, 플레이어는 그 세트를 가져와 적용만 한다.

### 3. 이동 처리 책임

권장 분리:

- `BP_PlayerController`
  `Enhanced Input` 바인딩 담당
- `BP_Character_TopDown`
  실제 이동 담당
- `BP_GameInstance`
  입력 세트 참조 담당

권장 흐름:

1. `BP_PlayerController` BeginPlay
2. `BP_GameInstance`에서 `DA_TopDownInputConfig` 획득
3. `DA_` 안의 `IMC_TopDown`을 `EnhancedInputLocalPlayerSubsystem`에 추가
4. `DA_` 안의 `IA_Move`를 바인딩
5. 입력값 `Vector2D`를 캐릭터 이동 로직으로 전달

이 구조로 가면 플레이어 BP가 직접 어떤 입력 에셋을 쓸지 결정하지 않는다.

## 카메라 / 이동 설계

### 카메라

- 탑다운 고정 시점
- 카메라 회전 입력 없음
- `SpringArm`은 `Absolute Rotation` 기반으로 고정
- `Use Pawn Control Rotation` 비활성화
- `Inherit Pitch/Yaw/Roll` 비활성화
- 마우스 입력으로 카메라를 돌리지 않음

추천 기본값 예시:

- Pitch: `-55`
- Yaw: `45`
- Arm Length: 프로젝트 감도에 맞게 조절

핵심은 "플레이 중 카메라 방향이 변하지 않는다"는 점이다.

### 이동

- `IA_Move`는 `Axis2D`
- `WASD`는 `IMC_TopDown`에서 `IA_Move`로 매핑
- 이동 방향은 카메라 기준 전방/우측 벡터를 평면에 투영해서 계산
- 카메라가 고정이므로 체감상 항상 동일한 방향으로 동작

권장 매핑:

- `W` -> `Y +1`
- `S` -> `Y -1`
- `A` -> `X -1`
- `D` -> `X +1`

실행 규칙:

- 캐릭터는 평면 이동만 한다
- 회전은 자동 회전으로 둘지, 고정 방향으로 둘지 별도 선택 가능
- 1차 목표는 "카메라 회전 없이 자연스럽게 이동"이다

## 에셋 구성안

### 폴더

- `/Game/Blueprints/Core`
- `/Game/Blueprints/GameMode`
- `/Game/Blueprints/Player`
- `/Game/Data/Input`
- `/Game/Input/Actions`
- `/Game/Input/Contexts`

### 실제 생성 대상

- `IA_Move`
- `IMC_TopDown`
- `DA_TopDownInputConfig`
- `BP_GI_CodexUMG`
- `BP_GM_TopDown`
- `BP_PC_TopDown`
- `BP_Character_TopDown`

필요하면 추가:

- `BPI_TopDownInputConsumer`
- `BP_CameraRig_TopDown`

### `DA_TopDownInputConfig`가 들고 있을 값

- `IMC_Default`
- `IA_Move`
- `MappingPriority`

확장 대비로 아래도 후보:

- `IA_Zoom`
- `IA_ClickMove`
- `IA_Confirm`
- `IA_Cancel`

단, 현재 범위에서는 `Move`만 먼저 고정하는 편이 낫다.

## BP 실체 우선 원칙

이 프로젝트에서는 "코드 클래스는 부모 타입 정도로만 두고, 실제 참조 연결은 BP 에셋으로 마감"하는 원칙으로 간다.

예시:

- 허용: 프로젝트 기본 게임 인스턴스를 `BP_GI_CodexUMG`로 설정
- 지양: 프로젝트 설정에서 네이티브 `UYourGameInstance`를 직접 가리키기
- 허용: 게임모드의 `Default Pawn Class`를 `BP_Character_TopDown`으로 설정
- 지양: `ACodexTopDownCharacter`를 직접 설정
- 허용: `BP_GI_CodexUMG`가 `DA_TopDownInputConfig` 에셋을 참조
- 지양: 플레이어가 네이티브 클래스 내부에서 `ConstructorHelpers`로 입력 에셋 경로를 직접 박기

정리하면, 코드가 있더라도 최종 연결은 항상 Content Browser 상의 실체 에셋에서 끝나야 한다.

## 에디터 모듈 계획

### 모듈 이름

`CodexUMGBootstrapEditor`

이유:

- 프로젝트 초기 세팅을 "부트스트랩"하는 역할이 이름에 드러난다
- 반복 사용 툴이라기보다 시작 자산을 한 번 생성하는 성격과 맞다
- 런타임 모듈과 책임 구분이 명확하다

### 모듈 책임

1. 필요한 폴더 생성
2. `IA_` / `IMC_` 입력 에셋 생성
3. `DA_` 입력 설정 에셋 생성
4. `BP_GameInstance`, `BP_GameMode`, `BP_PlayerController`, `BP_Character` 생성
5. 기본 참조를 BP 기준으로 연결
6. 필요 시 프로젝트 기본 `GameMode`, `GameInstance` 설정까지 마감

### 생성 방식

에디터 모듈에서 아래 계열 API를 사용해 자산을 만든다.

- `AssetTools`
- `BlueprintFactory`
- DataAsset 생성용 팩토리
- 필요 시 프로젝트 세팅 쓰기

실행 형태는 다음 중 하나로 잡으면 된다.

- 에디터 메뉴 커맨드 1회 실행
- 툴바 버튼 1회 실행
- 개발 전용 커맨드렛 성격의 에디터 액션

현재 요구에는 메뉴 커맨드 1회 실행이 가장 단순하다.

## 구현 순서 초안

1. 런타임용 베이스 타입 준비
   `GameInstance`, `PlayerController`, `Character`, `InputConfig DataAsset`의 부모 타입만 코드로 준비
2. `CodexUMGBootstrapEditor` 모듈 추가
3. 모듈에서 입력 에셋 `IA_Move`, `IMC_TopDown` 생성
4. 모듈에서 `DA_TopDownInputConfig` 생성 및 입력 에셋 연결
5. 모듈에서 `BP_GI_CodexUMG`, `BP_GM_TopDown`, `BP_PC_TopDown`, `BP_Character_TopDown` 생성
6. 모듈에서 BP 간 참조 연결
7. 프로젝트 기본 `GameInstance`, 기본 `GameMode`를 BP 기준으로 설정
8. `BP_PlayerController`가 `GameInstance -> DA_` 경유로 입력을 적용하도록 BP 그래프 구성
9. `BP_Character_TopDown`에 고정 카메라 및 평면 이동 로직 구성

## 권장 검증 항목

- 에디터 실행 후 생성된 `IA_Move`, `IMC_TopDown`, `DA_TopDownInputConfig`가 실제로 존재하는가
- 프로젝트 기본 `GameInstance`가 `BP_GI_CodexUMG`인가
- 프로젝트 기본 플레이 흐름이 `BP_GM_TopDown` 기준으로 묶였는가
- 플레이어 BP가 `IA_`, `IMC_`를 직접 기본값으로 들고 있지 않은가
- BeginPlay 이후 `GameInstance`를 통해 입력 세트를 조회하는가
- 카메라가 플레이 도중 회전하지 않는가
- `WASD`로 의도한 평면 이동이 되는가

## 메모

- `GameInstance`는 입력 에셋의 저장소로 쓰고, 실제 `MappingContext` 적용은 `LocalPlayer`가 준비된 시점의 `PlayerController`가 맡는 구조가 현실적이다.
- 이 방식이면 맵이 바뀌어도 입력 기준 데이터는 `GameInstance`에 유지된다.
- 이후 입력 프리셋이 늘어나면 `DA_`만 교체해서 다른 조작 체계로 확장하기 쉽다.

## 현재 초안의 결정 사항

- 카메라는 고정 탑다운
- 입력은 `Enhanced Input`
- `IA_`, `IMC_`는 실제 에셋 생성
- 입력 참조는 `DA_`에 모음
- `DA_`는 `BP_GameInstance`에 할당
- 플레이어는 `GameInstance`를 통해 접근
- 최종 연결은 BP 에셋 기준
- 자산 생성용 에디터 모듈 이름은 `CodexUMGBootstrapEditor`

## 나중에 취향 따라 조절할 부분

- 카메라 각도
- 캐릭터가 이동 방향으로 회전할지 여부
- 이동 기준을 월드축으로 둘지, 카메라 투영축으로 둘지
- `Character` 대신 `Pawn`으로 갈지 여부
- `DA_`를 단일 에셋로 둘지 입력 프리셋 묶음 구조로 확장할지 여부
