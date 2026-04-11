# Docs README

`Docs` 폴더에 있는 계획 문서들의 역할과 권장 읽기 순서를 정리한 안내 문서다.

## 권장 읽기 순서

1. [multi_plan_batch_execution_plan.md](./multi_plan_batch_execution_plan.md)
   여러 구현 플랜을 한 번에 진행할 때의 상위 실행 계획이다. 어떤 플랜이 선행인지, 무엇을 병렬 처리할 수 있는지, 공용 파일과 Blueprint 충돌을 어떻게 피할지 먼저 설명한다.

2. [topdown_fixed_camera_wasd_plan.md](./topdown_fixed_camera_wasd_plan.md)
   고정 탑다운 카메라와 `WASD` 이동 구조를 만드는 기반 계획이다. `Enhanced Input`, `DA_` 기반 입력 참조, `GameInstance` 경유 접근, commandlet 기반 애셋 생성과 테스트 가능 상태까지를 다룬다.

3. [player_projectile_firing_plan.md](./player_projectile_firing_plan.md)
   플레이어가 발사체를 쏘는 기능의 구현 계획이다. 발사 입력, 발사체 Blueprint, `DA_` 기반 참조 연결, `GameInstance` 연계, 구현 에이전트와 평가 에이전트 분리 원칙을 정의한다.

4. [interaction_umg_component_plan.md](./interaction_umg_component_plan.md)
   플레이어가 가까이 갔을 때 상호작용 가능 여부를 UMG로 표시하는 컴포넌트 구현 계획이다. 실제 위젯 애셋 생성, 전역 서브시스템 흐름, 단일 타겟 선택, 테스트용 상호작용 BP, 병렬 작업 지침을 포함한다.

5. [vox_mesh_asset_pipeline_plan.md](./vox_mesh_asset_pipeline_plan.md)
   `.vox` 원본을 UE5 `StaticMesh` 애셋으로 변환하는 파이프라인 계획이다. 공용 VOX 머티리얼, 버텍스 컬러 사용, 매니페스트 구조, VOX 전용 commandlet과 출력 경로를 정리한다.

6. [interaction_message_popup_plan.md](./interaction_message_popup_plan.md)
   나무 팻말 상호작용으로 여는 중앙 메시지 팝업 구현 계획이다. 실제 `Widget Blueprint` 애셋, `Space` 닫기 입력, `OK` / `YesNo` 버튼 레이아웃, 상호작용 서브시스템을 통한 결과 전달 구조를 다룬다.

7. [interaction_scroll_message_popup_plan.md](./interaction_scroll_message_popup_plan.md)
   긴 텍스트를 스크롤해서 보여주는 중앙 팝업 구현 계획이다. 기존 단순 팝업과 거의 같은 레이아웃을 유지하면서, 노란 스마일 아이콘, 스크롤 영역, `OK` 단일 버튼, 노란 계열 틴트 차이만 추가한 변형 계획이다.

## 파일별 요약

### [multi_plan_batch_execution_plan.md](./multi_plan_batch_execution_plan.md)

- `topdown_fixed_camera_wasd_plan`, `interaction_umg_component_plan`, `interaction_message_popup_plan`, `interaction_scroll_message_popup_plan`, `player_projectile_firing_plan`, `vox_mesh_asset_pipeline_plan`을 묶는 상위 배치 실행 계획이다.
- `topdown_fixed_camera_wasd_plan`을 선행 게이트로 두고, `vox_mesh_asset_pipeline_plan`은 별도 병렬 트랙으로 처리한다.
- `interaction_message_popup_plan`과 `interaction_scroll_message_popup_plan`은 둘 다 `interaction_umg_component_plan`과 `vox_mesh_asset_pipeline_plan` 완료 후 착수하는 후행 팝업 트랙이다.
- 두 팝업 트랙은 가능한 시점부터 병렬 처리 가능한 독립 작업으로 본다.

### [topdown_fixed_camera_wasd_plan.md](./topdown_fixed_camera_wasd_plan.md)

- 정북 기준 고정 탑다운 카메라와 `WASD` 이동을 만드는 기반 문서다.
- 입력 애셋 `IA_`, `IMC_`를 실제 애셋으로 만들고 참조를 `DA_`에 모아 `BP_GameInstance`를 통해 접근하는 구조를 제안한다.
- commandlet로 애셋 생성과 기본 셋업을 마무리하고, 테스트 가능한 상태가 되면 에디터를 여는 지침을 포함한다.

### [player_projectile_firing_plan.md](./player_projectile_firing_plan.md)

- 플레이어 발사 기능을 확장 가능한 구조로 설계하는 문서다.
- 발사체 Blueprint와 관련 설정을 `DA_` 계열 애셋에 두고, 플레이어가 직접 발사체 클래스를 들고 있지 않게 설계한다.
- 기존 입력 체계를 우선 재사용하고, 필요 시 최소 범위만 확장하며, 구현과 평가 역할을 분리하는 원칙을 정리한다.

### [interaction_umg_component_plan.md](./interaction_umg_component_plan.md)

- 플레이어 접근에 따라 월드 상호작용 가능 상태를 UMG로 보여주는 컴포넌트 계획 문서다.
- 실제 `Widget Blueprint`를 에디터 모듈 코드와 `Commandlet`로 생성하고, 전역 서브시스템이 가장 가까운 대상 하나만 `상호작용 가능` 상태로 선택하는 구조를 정의한다.
- 테스트용 과일 BP, `먹기` 상호작용, 화면 디버그 기준, 병렬 에이전트 운영 지침까지 포함한다.

### [vox_mesh_asset_pipeline_plan.md](./vox_mesh_asset_pipeline_plan.md)

- 테스트용 VOX 메시를 만들고 UE5 `StaticMesh`로 자동 생성하는 파이프라인 문서다.
- `32 x 32 x 32` 해상도, 버텍스 컬러 표현, 공용 머티리얼 1개 사용, editor UI 없는 commandlet 생성 조건을 고정한다.
- 원본 아트 위치, 출력 메시 카테고리, UE 애셋 경로, 매니페스트, 자동화 범위를 설명한다.

### [interaction_message_popup_plan.md](./interaction_message_popup_plan.md)

- 나무 팻말 상호작용으로 여는 단순 메시지 팝업 계획 문서다.
- 실제 `WBP_InteractionMessagePopup` 애셋을 에디터 모듈 코드와 `Commandlet`로 생성하고, 위젯 트리는 WBP 안에 실제로 존재해야 한다는 규칙을 따른다.
- `OK` / `YesNo` 버튼 레이아웃, 닫기 버튼과 `Space` 닫기, 팝업 결과를 `UCodexInteractionSubsystem`으로 먼저 전달하는 구조를 정의한다.

### [interaction_scroll_message_popup_plan.md](./interaction_scroll_message_popup_plan.md)

- 긴 텍스트를 스크롤해서 보여주는 변형 팝업 계획 문서다.
- 기존 `interaction_message_popup_plan.md`와 거의 같은 패널 구조를 유지하면서, 노란 스마일 아이콘, `ScrollBox` 기반 긴 본문, `OK` 단일 버튼, 노란 계열 저투명 틴트를 추가한다.
- 상호작용 이름은 `읽기`로 고정하고, 결과는 기존 상호작용 서브시스템을 통해 먼저 전달되도록 설계한다.
