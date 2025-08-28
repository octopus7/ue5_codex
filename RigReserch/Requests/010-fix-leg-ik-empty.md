# Fix Empty leg_ik Rig / leg_ik 빈 릭 보정

## Request / 요청
- EN: The leg_ik rig still shows only bones in the hierarchy and the Forward Solve graph is empty except the start node.
  KO: leg_ik 릭에서 하이어라키에 본만 보이고, 포워드 솔브 그래프는 시작 노드만 보이며 비어 있습니다.

## Response / 응답
- EN: Added a new fixer script `Scripts/leg_ik_build_fix.py` that force-imports bones, creates IK foot/pole controls (L/R), and wires Two-Bone IK with robust fallbacks and debug prints.
  KO: 본 강제 보정 스크립트 `Scripts/leg_ik_build_fix.py`를 추가했습니다. 본을 가져오고, 좌/우 발 IK 및 폴 벡터 컨트롤을 생성한 뒤, 투본 IK를 연결합니다. 다양한 폴백과 디버그 출력을 포함합니다.

## How to Run / 실행 방법
- EN: In UE: `Tools > Python > Execute Python Script...` → select `Scripts/leg_ik_build_fix.py` (no parameters). It will create or update `/Game/Rigs/leg_ik`.
  KO: 언리얼에서 `Tools > Python > Execute Python Script...` → `Scripts/leg_ik_build_fix.py` 선택(파라미터 불필요). `/Game/Rigs/leg_ik`를 생성/갱신합니다.

## What It Does / 동작 내용
- EN: Ensures preview mesh, imports skeleton bones to the rig hierarchy, adds controls (`C_ik_foot_l/r`, `C_pole_l/r`), builds Two-Bone IK nodes, recompiles and saves.
  KO: 프리뷰 메시 설정, 릭 하이어라키로 본 가져오기, 컨트롤(`C_ik_foot_l/r`, `C_pole_l/r`) 추가, 투본 IK 노드 구성, 리컴파일/저장을 수행합니다.

## If Still Empty / 여전히 비어있다면
- EN: Share Output Log errors printed by the script (e.g., failures to add controls or nodes). I will patch pin or API names to match your exact 5.5 build.
  KO: 스크립트가 출력하는 로그(컨트롤/노드 추가 실패 등)를 공유해 주세요. 사용 중인 5.5 빌드에 맞춰 핀/API명을 즉시 보정하겠습니다.

