# Pelvis Forward Solve / 골반 Forward Solve 구성

## Request / 요청
- EN: Proceed to add a script that wires Forward Solve for the pelvis in the existing pelvis-only rig.
  KO: 기존 골반 전용 릭에 대해 골반 Forward Solve을 구성하는 스크립트를 추가 진행해 주세요.

## Response / 응답
- EN: Added `Scripts/pelvis_forward_solve.py`. It loads or creates `/Game/ControlRigs4/CR_PelvisOnly`, ensures the `Pelvis_CTRL` control exists, and builds a minimal Forward Solve graph: `GetControlTransform(Pelvis_CTRL) -> SetBoneTransform(pelvis)` with execution linked from the entry node.
  KO: `Scripts/pelvis_forward_solve.py`를 추가했습니다. `/Game/ControlRigs4/CR_PelvisOnly`를 로드/생성하고 `Pelvis_CTRL` 컨트롤을 보장한 뒤, Forward Solve 그래프에 `GetControlTransform(Pelvis_CTRL) -> SetBoneTransform(pelvis)`를 구성하고 엔트리에서 실행 핀을 연결합니다.

## How to Run / 실행 방법
- EN: In UE: `Tools > Python > Execute Python Script...` → select `Scripts/pelvis_forward_solve.py`.
  KO: 언리얼에서 `Tools > Python > Execute Python Script...` → `Scripts/pelvis_forward_solve.py` 선택.

## Notes / 참고
- EN: If Source Control is enabled, check out `/Game/ControlRigs4/CR_PelvisOnly` before running so changes persist.
  KO: 소스 컨트롤 사용 중이면 실행 전 `/Game/ControlRigs4/CR_PelvisOnly`를 체크아웃하세요. 그렇지 않으면 변경이 저장되지 않습니다.
- EN: The script includes multiple fallbacks for node and pin names to cover UE 5.x variants.
  KO: UE 5.x 변형을 고려하여 노드/핀 이름에 대한 폴백을 포함합니다.

## Files / 파일
- EN: Added `Scripts/pelvis_forward_solve.py`.
  KO: `Scripts/pelvis_forward_solve.py`를 추가했습니다.

