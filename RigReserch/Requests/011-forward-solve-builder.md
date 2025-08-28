# Forward Solve Builder / Forward Solve 빌더

## Request / 요청
- EN: Node count is still 0 after building leg_ik. Please ensure nodes are added to the Forward Solve graph and that controls show up.
  KO: leg_ik 빌드 후에도 노드 수가 0입니다. Forward Solve 그래프에 노드가 추가되고 컨트롤이 보이도록 해 주세요.

## Response / 응답
- EN: I added `Scripts/leg_ik_build_forward.py`, which explicitly targets the Forward Solve (Rig Graph), imports bones, creates leg IK controls via bone-based APIs, and wires Two-Bone IK nodes with multiple fallbacks.
  KO: Forward Solve(리그 그래프)를 명시 대상으로 하여, 본을 가져오고 본 기반 API로 레그 IK 컨트롤을 생성하며, 투본 IK 노드를 연결하는 `Scripts/leg_ik_build_forward.py`를 추가했습니다.

## How to Run / 실행 방법
- EN: In UE: `Tools > Python > Execute Python Script...` → select `Scripts/leg_ik_build_forward.py`.
  KO: 언리얼에서 `Tools > Python > Execute Python Script...` → `Scripts/leg_ik_build_forward.py` 선택.

## Expected / 기대 결과
- EN: Controls `C_ik_foot_l/r`, `C_pole_l/r` appear in the hierarchy. The Forward Solve graph contains `GetControlTransform` and `TwoBoneIK*` nodes.
  KO: 하이어라키에 `C_ik_foot_l/r`, `C_pole_l/r` 컨트롤이 보이고, Forward Solve 그래프에 `GetControlTransform` 및 `TwoBoneIK*` 노드가 생성됩니다.

## If Issues Persist / 문제가 지속되면
- EN: Share Output Log lines from this script; I’ll align exact graph/controller API names for your UE 5.5 build.
  KO: 본 스크립트의 출력 로그를 알려주시면, 사용 중인 UE 5.5 빌드에 맞춰 그래프/컨트롤러 API 이름을 보정하겠습니다.

