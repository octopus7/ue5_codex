# UE5 탑다운 슈터 프로젝트 하네스 템플릿

이 파일은 Unreal Engine 5 프로젝트에서 탑다운 슈터 게임을 바닥부터 구축할 때 사용하는 재사용 가능한 요청 템플릿입니다.

입력, 플레이어 이동, 조준, 전투, 적 행동 같은 핵심 게임플레이 기반이 아직 없는 상태에서, 작업을 단일 태스크가 아니라 여러 단계로 구성된 프로젝트로 다루고 싶을 때 사용합니다.

## 사용 방법

아래 요청 블록을 복사한 뒤, 플레이스홀더를 교체해서 그대로 보내면 됩니다.

## 요청 템플릿

```md
먼저 이 파일을 읽어줘:
<ABSOLUTE_PATH_TO_THIS_MD_FILE>

그다음 아래 UE5 프로젝트의 작업용 프로젝트 하네스로 사용해줘.

프로젝트 경로:
<ABSOLUTE_PROJECT_PATH>

프로젝트 목표:
탑다운 슈터 게임을 바닥부터 구축한다.

현재 프로젝트 상태:
- 입력 시스템: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- 플레이어 폰과 이동: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- 카메라와 조준: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- 무기와 전투 루프: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- 적과 AI 루프: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- HUD와 UI: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- 매치 흐름 또는 웨이브 흐름: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- 저장, 설정, 진행도: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>

목표 플레이 가능 결과물:
<원하는 첫 플레이 가능 버전을 설명>

게임플레이 핵심 축:
- <PILLAR_1>
- <PILLAR_2>
- <PILLAR_3>

카메라와 조작 스타일:
- 카메라 모델: <FIXED_TOP_DOWN / FOLLOW_CAMERA / ROTATING_CAMERA / OTHER>
- 이동 모델: <WASD / CLICK_TO_MOVE / TWIN_STICK / OTHER>
- 조준 모델: <MOUSE_CURSOR / RIGHT_STICK / AUTO_AIM / OTHER>
- 발사 모델: <HITSCAN / PROJECTILE / BOTH / OTHER>

플랫폼 및 엔진 제약:
- 엔진 버전: <UE_VERSION>
- 대상 플랫폼: <PC / CONSOLE / MOBILE / OTHER>
- 네트워크 모델: <SINGLE_PLAYER / COOP / PVP / TBD>
- 성능 목표: <FPS OR HARDWARE TARGET>

구현 규칙:
- C++ 담당 범위: <무엇을 C++에 둘지>
- Blueprint 담당 범위: <무엇을 Blueprint에 둘지>
- 폴더 및 네이밍 규칙: <RULES>
- 플러그인 규칙: <허용 또는 금지 플러그인>
- 호환성 제약: <안정적으로 유지해야 할 시스템 또는 API>
- 기본 연결 규칙: `GameMode`, `DefaultPawnClass`, `PlayerControllerClass` 등에 직접 C++ 클래스를 연결하지 말고 구체적인 Blueprint 애셋을 연결할 것
- 가시 표현 규칙: 화면에 보여야 하는 플레이어/적/무기는 실제 메시 애셋이 연결된 상태여야 하며 투명 placeholder 상태를 완료로 보지 말 것

프로젝트 범위:
- 포함 범위: <LIST>
- 제외 범위: <LIST>

예상 프로젝트 마일스톤:
1. 기반 구성
   - 프로젝트 부트스트랩, 맵, 게임 모드, 플레이어 컨트롤러, 입력 설정
2. 핵심 이동
   - 탑다운 카메라, 이동, 바라보는 방향, 조준 방향
3. 전투 루프
   - 발사 입력, 무기 로직, 히트 판정, 데미지, 사망 처리
4. 적 루프
   - 적 액터, 추적 또는 공격 행동, 스폰 흐름, 전투 상호작용
5. HUD와 게임 흐름
   - 체력 또는 탄약 UI, 웨이브 또는 미션 흐름, 재시작 또는 실패 처리
6. 폴리시 단계
   - 피드백, 밸런스 조정, 정리, 버그 수정, 검증

프로젝트 실행 규칙:
- 프로젝트 수준의 계획을 유지하고, 마일스톤이 완료될 때마다 갱신해줘.
- 작동 가능한 결과를 유지하면서도 가능한 한 작은 응집된 단계로 작업을 나눠줘.
- 각 단계에서 파일을 수정하기 전에 정확한 범위와 수정 예정 파일을 설명해줘.
- 각 단계가 끝나면 무엇이 바뀌었는지, 무엇을 검증했는지, 무엇이 남았는지, 다음 추천 단계가 무엇인지 보고해줘.
- 필요하면 `generator`와 `evaluator` 에이전트를 분리된 컨텍스트로 운영해줘.
- `generator`는 구현과 초안 패치, `evaluator`는 리뷰와 검증 관점 평가를 맡겨줘.
- 두 에이전트가 직접 대화하지 말고 메인 에이전트가 결과와 피드백을 중계해줘.
- 이 방식을 실제로 썼다면 역할 분리 요약은 단계 문서에, 중계 방식은 작업 시간 로그 메모에 남겨줘.
- 런타임 연결에 필요한 Blueprint 애셋과 VOX 기반 `StaticMesh` import는 커맨드렛으로 실제 생성 또는 갱신해줘.
- 기본 연결 지점에 직접 C++ 클래스가 남지 않게 실제 BP GameMode/Pawn/Controller를 연결해줘.
- 에셋 부족이나 제품 결정의 모호함 때문에 막히면, 짧고 명확한 질문 하나만 해줘.

현재 실행 경계:
- 시작할 마일스톤: <NUMBER OR NAME>
- 이번 실행의 현재 단계 목표: <EXACT GOAL FOR THIS RUN>
- 여기서 멈출 기준: <ONE_PHASE / ONE_MILESTONE / SPECIFIC_GOAL>
- 내가 명시적으로 요청하지 않으면 이 경계를 넘어서 계속 진행하지 마.

반드시 지켜야 하는 제약:
- <CONSTRAINT_1>
- <CONSTRAINT_2>
- <CONSTRAINT_3>
- 에이전트 역할을 분리할 경우 쓰기 충돌이 나지 않게 파일 책임을 분리할 것
- 보이는 플레이어가 필요하면 `.vox -> StaticMesh -> Blueprint 연결`까지 완료할 것

검증 요구사항:
- <BUILD REQUIREMENT>
- <TEST REQUIREMENT>
- <MANUAL PLAYCHECK REQUIREMENT>

보고 요구사항:
- 큰 수정 전에 계획을 요약해줘.
- 변경 후 아래 항목을 보고해줘:
  - 변경된 파일
  - 구현된 동작
  - 수행한 검증
  - 남아 있는 리스크 또는 미정 사항
  - 다음 추천 마일스톤 또는 단계
  - `generator/evaluator` 분리 운영을 썼다면 역할 분리와 중계 방식이 어느 문서에 기록됐는지
  - 실제 생성된 Blueprint 애셋 경로와 import 된 `StaticMesh` 애셋 경로
```

## 선택적 단계 슬라이스 템플릿

전체 프로젝트 하네스는 이미 있고, 이번 실행에서 다룰 범위만 따로 지정하고 싶을 때 아래 블록을 사용합니다.

```md
현재 단계 목표:
<EXACT GOAL>

이번 단계 포함 범위:
- <ITEM>
- <ITEM>

이번 단계 제외 범위:
- <ITEM>
- <ITEM>

제약:
- <ITEM>
- <ITEM>

검증:
- <ITEM>
- <ITEM>

여기서 멈출 기준:
<ONE_PHASE / ONE_MILESTONE / SPECIFIC_RESULT>
```

## 메모

- 이 파일은 단일 태스크용이 아니라 프로젝트 규모의 하네스입니다.
- 상위 목표는 유지하고, 현재 실행 경계로 각 실행 단위의 범위를 좁히는 방식입니다.
- 프로젝트가 아직 게임플레이 구현이 없는 상태라면, 첫 마일스톤은 보통 전투보다 먼저 입력, 컨트롤러 또는 폰 구조, 카메라, 이동을 세우는 쪽이 적절합니다.
- 더 강한 통제가 필요하면, 현재 마일스톤용 별도 태스크 하네스와 함께 사용하면 됩니다.
