# 고정 탑다운 카메라 + WASD 이동 구조 계획

## 문서 목적

회전하지 않는 탑다운 카메라 시점에서 `WASD`로 이동하는 플레이 구조를 만든다.
카메라 구도는 비스듬한 사선 시점이 아니라, 화면 상단이 정북과 일치하는 정방향 탑다운 구도를 전제로 한다.
입력은 `Enhanced Input` 기반으로 구성하고, `IA_` / `IMC_`는 실제 에셋으로 생성한다.
입력 에셋 참조는 플레이어 캐릭터나 플레이어 BP에 직접 박지 않고, `DA_` 에셋에 모은 뒤 `GameInstance` BP에 걸어 플레이어가 접근하는 구조로 정리한다.

이 문서는 바로 구현 코드로 들어가기 전에 구조를 고정하기 위한 초안이다.

## 작업 지시

- 병렬 처리 가능하면 에이전트 별도로 처리해서 리드 타임을 줄인다.
- 단, 같은 파일이나 같은 BP를 동시에 만지는 식의 충돌성 병렬화는 피한다.
- 에셋 생성 단계는 사용자에게 메뉴 실행을 넘기지 말고, 생성용 커맨드렛 실행까지 자율적으로 진행한다.
- 모든 작업이 테스트 가능 상태까지 완료되면 마지막에 에디터로 프로젝트를 연다.

## 핵심 제약

1. `IA_` 와 `IMC_` 는 실제 Content Browser 에셋으로 존재해야 한다.
2. 플레이어 캐릭터 / 플레이어 BP는 `IA_`, `IMC_`를 직접 기본값으로 들고 있지 않는다.
3. 입력 관련 실제 참조는 `DA_` 에셋에 모으고, 그 `DA_` 는 `BP_GameInstance`에 할당한다.
4. 플레이어는 `GameInstance`를 통해 필요한 입력 세트를 조회한다.
5. 모든 연결 지점은 네이티브 클래스 직접 참조 대신 BP로 실체화된 에셋을 우선 사용한다.
6. 에셋 생성은 런타임 코드가 아니라 에디터 모듈에서 수행한다.
7. 이 에디터 모듈은 프로젝트 초기 세팅을 부트스트랩하는 용도이므로, 자주 다시 실행하는 도구가 아니라 1회성 생성 도구로 본다.
8. 카메라는 비스듬한 `Yaw 45` 계열 사선 구도를 사용하지 않는다.
9. 게임 화면 상단이 정북과 일치하도록 카메라 방향을 고정한다.
10. 플레이어는 스켈레탈 메시 외에 별도 `StaticMeshComponent`를 하나 더 가져야 한다.
11. 자산 생성의 기본 진입점은 사용자 수동 메뉴가 아니라 재시도 중 직접 실행하는 커맨드렛이어야 한다.
12. 전체 작업이 테스트 가능 상태에 도달하면 종료 전에 에디터를 열어 즉시 검증 가능한 상태로 둔다.

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
- 비스듬한 아이소메트릭 구도 금지
- 화면 상단과 정북 방향이 일치해야 함
- `SpringArm`은 `Absolute Rotation` 기반으로 고정
- `Use Pawn Control Rotation` 비활성화
- `Inherit Pitch/Yaw/Roll` 비활성화
- 마우스 입력으로 카메라를 돌리지 않음

추천 기본값 예시:

- Pitch: `-55`
- Yaw: 정북 방향 고정
- Arm Length: 프로젝트 감도에 맞게 조절

핵심은 아래 두 가지다.

- 플레이 중 카메라 방향이 변하지 않는다
- 정북이 항상 화면 상단에 오도록 유지한다

### 이동

- `IA_Move`는 `Axis2D`
- `WASD`는 `IMC_TopDown`에서 `IA_Move`로 매핑
- 이동 방향은 카메라 기준 전방/우측 벡터를 평면에 투영해서 계산
- 카메라가 정북 기준으로 고정되므로 체감상 항상 동일한 방향으로 동작

권장 매핑:

- `W` -> `Y +1`
- `S` -> `Y -1`
- `A` -> `X -1`
- `D` -> `X +1`

실행 규칙:

- 캐릭터는 평면 이동만 한다
- 회전은 자동 회전으로 둘지, 고정 방향으로 둘지 별도 선택 가능
- 1차 목표는 "정북 기준 고정 카메라에서 자연스럽게 이동"이다

## 플레이어 가시성 규칙

- `BP_Character_TopDown`에는 캐릭터 기본 `SkeletalMeshComponent`와 별도로 `StaticMeshComponent`를 하나 더 단다.
- 이 스태틱 메시는 디버그용이 아니라 실제 플레이에서 플레이어가 바로 보이도록 하는 가시성 보조 수단으로 취급한다.
- 스태틱 메시는 루트 또는 적절한 시각 기준 컴포넌트에 부착하고, 탑다운 시점에서 식별이 잘 되는 크기와 오프셋을 준다.
- 현재 캐릭터용 VOX 메시의 시각적 정면이 로컬 `+X`가 아니라 오른손 방향인 로컬 `+Y`를 향하는 경우, 메시 에셋 자체를 다시 틀지 않고 플레이어 가시성용 별도 `StaticMeshComponent` 또는 그 부모 `SceneComponent`에서 상대 `Yaw = -90` 보정으로 `BP_Character_TopDown`의 정면 `+X`와 맞춘다.
- 이 축 보정은 플레이어 비주얼 부착 계층에서만 처리하고, 이 문제만으로 VOX 공용 축 변환 규칙이나 다른 메시 생성 규칙을 바꾸지 않는다.
- 즉, "캐릭터니까 스켈레탈 메시가 있으니 충분하다"로 끝내지 않는다.

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

`BP_Character_TopDown` 내부 필수 구성:

- `SkeletalMeshComponent`
- 별도 `StaticMeshComponent`

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

실행 형태는 `커맨드렛` 기반으로 고정한다.

- 사용자에게 에디터 메뉴나 툴바 버튼을 넘겨 수동 실행시키지 않는다.
- 재시도 프로세스 안에서 커맨드렛 작성, 호출 경로 연결, 실제 실행까지 자율적으로 마감한다.
- 메뉴나 툴바 진입점이 필요하더라도 보조 디버깅 수단으로만 취급하고 기본 경로로 채택하지 않는다.
- 자산 생성과 기본 세팅이 끝나 테스트 가능 상태가 되면 곧바로 에디터로 프로젝트를 연다.

## 구현 순서 초안

1. 런타임용 베이스 타입 준비
   `GameInstance`, `PlayerController`, `Character`, `InputConfig DataAsset`의 부모 타입만 코드로 준비
2. `CodexUMGBootstrapEditor` 모듈 추가
3. 자산 생성용 커맨드렛 진입점 추가
4. 커맨드렛에서 입력 에셋 `IA_Move`, `IMC_TopDown` 생성
5. 커맨드렛에서 `DA_TopDownInputConfig` 생성 및 입력 에셋 연결
6. 커맨드렛에서 `BP_GI_CodexUMG`, `BP_GM_TopDown`, `BP_PC_TopDown`, `BP_Character_TopDown` 생성
7. 커맨드렛에서 BP 간 참조 연결 및 프로젝트 기본 `GameInstance`, 기본 `GameMode`를 BP 기준으로 설정
8. 재시도 과정에서 사용자가 개입하지 않도록 커맨드렛을 직접 실행해 자산 생성을 완료
9. `BP_PlayerController`가 `GameInstance -> DA_` 경유로 입력을 적용하도록 BP 그래프 구성
10. `BP_Character_TopDown`에 정북 기준 고정 카메라 및 평면 이동 로직 구성
11. `BP_Character_TopDown`에 스켈레탈 메시와 별도 `StaticMeshComponent`를 함께 구성하고, 캐릭터 메시의 시각적 정면이 로컬 `+Y` authored 기준이면 별도 메시에 상대 `Yaw -90` 보정을 적용
12. 전체 구성이 테스트 가능 상태에 도달하면 에디터로 프로젝트를 열어 바로 검증 단계로 진입

## 권장 검증 항목

- 커맨드렛 실행 후 생성된 `IA_Move`, `IMC_TopDown`, `DA_TopDownInputConfig`가 실제로 존재하는가
- 프로젝트 기본 `GameInstance`가 `BP_GI_CodexUMG`인가
- 프로젝트 기본 플레이 흐름이 `BP_GM_TopDown` 기준으로 묶였는가
- 플레이어 BP가 `IA_`, `IMC_`를 직접 기본값으로 들고 있지 않은가
- BeginPlay 이후 `GameInstance`를 통해 입력 세트를 조회하는가
- 카메라가 플레이 도중 회전하지 않는가
- 카메라가 비스듬한 사선 구도가 아니라 정북 정렬 구도인가
- `WASD`로 의도한 평면 이동이 되는가
- 플레이어가 탑다운 시점에서 즉시 식별 가능하도록 스태틱 메시가 별도로 보이는가
- 플레이어 가시성용 스태틱 메시의 시각적 정면이 `BP_Character_TopDown`의 정면/이동 방향과 일치하는가
- 위 항목이 충족되어 테스트 가능 상태라면 에디터가 실제로 열려 곧바로 플레이 검증으로 이어지는가

## 메모

- `GameInstance`는 입력 에셋의 저장소로 쓰고, 실제 `MappingContext` 적용은 `LocalPlayer`가 준비된 시점의 `PlayerController`가 맡는 구조가 현실적이다.
- 이 방식이면 맵이 바뀌어도 입력 기준 데이터는 `GameInstance`에 유지된다.
- 이후 입력 프리셋이 늘어나면 `DA_`만 교체해서 다른 조작 체계로 확장하기 쉽다.

## 현재 초안의 결정 사항

- 카메라는 고정 탑다운
- 카메라 방향은 정북 기준으로 고정
- 입력은 `Enhanced Input`
- `IA_`, `IMC_`는 실제 에셋 생성
- 입력 참조는 `DA_`에 모음
- `DA_`는 `BP_GameInstance`에 할당
- 플레이어는 `GameInstance`를 통해 접근
- 최종 연결은 BP 에셋 기준
- 자산 생성용 에디터 모듈 이름은 `CodexUMGBootstrapEditor`
- 자산 생성은 사용자 메뉴 조작이 아니라 커맨드렛 실행까지 자율적으로 진행
- 테스트 가능 상태까지 완료되면 에디터를 연다
- 플레이어는 스켈레탈 메시 외에 별도 스태틱 메시를 가진다

## 나중에 취향 따라 조절할 부분

- 카메라 Pitch
- 캐릭터가 이동 방향으로 회전할지 여부
- 이동 기준을 월드축으로 둘지, 카메라 투영축으로 둘지
- `Character` 대신 `Pawn`으로 갈지 여부
- `DA_`를 단일 에셋로 둘지 입력 프리셋 묶음 구조로 확장할지 여부
