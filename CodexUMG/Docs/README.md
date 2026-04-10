# Docs README

`Docs` 폴더에 있는 계획 문서들의 역할과 읽기 순서를 정리한 안내 문서다.

## 권장 읽기 순서

1. [multi_plan_batch_execution_plan.md](./multi_plan_batch_execution_plan.md)
   여러 개의 구현 플랜을 한 번에 진행할 때의 최상위 실행 계획이다. 어떤 플랜이 선행인지, 무엇을 병렬 처리할 수 있는지, 공용 파일과 Blueprint 충돌을 어떻게 피할지를 먼저 설명한다.

2. [topdown_fixed_camera_wasd_plan.md](./topdown_fixed_camera_wasd_plan.md)
   고정 탑다운 카메라와 `WASD` 이동 구조를 만드는 기반 계획이다. `Enhanced Input`, `DA_` 기반 입력 참조, `GameInstance` 경유 접근, 커맨드렛 기반 자산 생성, 테스트 가능 상태 후 에디터 오픈까지의 기준을 담고 있다.

3. [player_projectile_firing_plan.md](./player_projectile_firing_plan.md)
   플레이어가 투사체를 발사하는 기능의 상위 구현 계획이다. 투사체 Blueprint, 발사 입력, `DA_` 기반 참조 연결, `GameInstance` 연계, 평가 에이전트 분리 원칙 등을 정의한다.

4. [vox_mesh_asset_pipeline_plan.md](./vox_mesh_asset_pipeline_plan.md)
   `.vox` 원본 파일을 UE5용 `StaticMesh` 에셋으로 변환하는 파이프라인 계획이다. 공용 VOX 머터리얼, 버텍스 컬러 사용, 매니페스트 구조, VOX 전용 커맨드렛과 산출물 경로를 정리한다.

## 파일별 요약

### [multi_plan_batch_execution_plan.md](./multi_plan_batch_execution_plan.md)

- `topdown_fixed_camera_wasd_plan`, `player_projectile_firing_plan`, `vox_mesh_asset_pipeline_plan` 세 문서를 묶는 상위 배치 실행 계획이다.
- `topdown_fixed_camera_wasd_plan`을 선행 게이트로 두고, `vox_mesh_asset_pipeline_plan`은 별도 병렬 트랙으로 처리하는 구조를 정의한다.
- 메인 작업자와 병렬 작업 에이전트의 책임 분리, 공용 파일 충돌 회피, 최종 검증 흐름을 설명한다.

### [topdown_fixed_camera_wasd_plan.md](./topdown_fixed_camera_wasd_plan.md)

- 정북 기준 고정 탑다운 카메라와 `WASD` 이동을 만드는 기준 문서다.
- 입력 에셋 `IA_`, `IMC_`를 실제 에셋으로 만들고, 참조는 `DA_`에 모아 `BP_GameInstance`를 통해 접근하게 하는 구조를 제안한다.
- 사용자 수동 메뉴 대신 커맨드렛으로 자산 생성과 기본 세팅을 마감하고, 테스트 가능 상태가 되면 에디터를 여는 지침을 포함한다.

### [player_projectile_firing_plan.md](./player_projectile_firing_plan.md)

- 플레이어 발사 기능을 특정 현재 구현에 강하게 묶지 않고 확장 가능한 구조로 설계하는 문서다.
- 투사체 Blueprint와 관련 설정을 `DA_` 계열 자산에 두고, 플레이어는 직접 투사체 클래스를 들지 않도록 제한한다.
- 기존 입력 체계 재사용 우선, 필요 시 최소 범위 입력 확장, 구현 에이전트와 평가 에이전트 분리 원칙을 정의한다.

### [vox_mesh_asset_pipeline_plan.md](./vox_mesh_asset_pipeline_plan.md)

- 테스트용 저해상도 VOX 메시를 제작하고 UE5 `StaticMesh`로 자동 생성하는 파이프라인 문서다.
- `32 x 32 x 32` 해상도, 버텍스 컬러 표현, 공용 머터리얼 1개 사용, 에디터 UI 없이 `Commandlet`로 생성하는 조건을 고정한다.
- 소스 아트 위치, 생성 대상 메시 카테고리, UE 에셋 출력 경로, 매니페스트와 자동화 범위를 설명한다.
