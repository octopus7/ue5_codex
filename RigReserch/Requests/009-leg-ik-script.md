# Leg IK Script / 레그 IK 스크립트

## Request / 요청
- EN: Do not modify existing scripts; create a new script. I need a leg IK rig implemented with the name "leg ik".
  KO: 기존 스크립트는 수정하지 말고, 새 스크립트를 생성해 주세요. "leg ik"라는 이름으로 레그 IK 릭이 필요합니다.

## Response / 응답
- EN: Added a new script `Scripts/leg_ik.py` that creates a Control Rig Blueprint named `leg_ik` (underscores used since asset names cannot contain spaces). It imports skeleton bones, adds IK foot and pole controls for L/R, and builds Two-Bone IK nodes wired to those controls.
  KO: 새 스크립트 `Scripts/leg_ik.py`를 추가했습니다. 에셋명에는 공백을 사용할 수 없으므로 `leg_ik` 이름으로 컨트롤 릭 블루프린트를 생성합니다. 스켈레톤 본을 가져오고, 좌/우 발 IK 및 폴 벡터 컨트롤을 추가하며, 해당 컨트롤에 연결된 투본 IK 노드를 구성합니다.

## How to Run / 실행 방법
- EN: In UE: `Tools > Python > Execute Python Script...` → select `Scripts/leg_ik.py` (no parameters needed). Defaults: mesh `/Character/SK_Man`, folder `/Game/Rigs`.
  KO: 언리얼에서 `Tools > Python > Execute Python Script...` → `Scripts/leg_ik.py` 선택(파라미터 불필요). 기본값: 메시 `/Character/SK_Man`, 폴더 `/Game/Rigs`.

## Notes / 참고
- EN: Uses standard UE bones (`thigh_l/r`, `calf_l/r`, `foot_l/r`). If your bones differ, tell me exact names and I will adjust.
  KO: 표준 UE 본(`thigh_l/r`, `calf_l/r`, `foot_l/r`)을 사용합니다. 다르면 정확한 본명을 알려주시면 조정하겠습니다.

## Files / 파일
- EN: Added `Scripts/leg_ik.py`.
  KO: `Scripts/leg_ik.py`를 추가했습니다.

