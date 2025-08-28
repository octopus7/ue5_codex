# FK Spine + IK Legs Script / FK 스파인 + IK 레그 스크립트

## Request / 요청
- EN: The Skeletal Mesh path is `/Character/SK_Man`, bones follow Unreal’s standard naming. Write a script that sets up FK spine and IK legs.
  KO: 스켈레탈 메시 경로는 `/Character/SK_Man`이고, 언리얼 표준 본 네이밍입니다. FK 스파인과 IK 레그를 구성하는 스크립트를 작성해 주세요.

## Response / 응답
- EN: Added `Scripts/setup_fk_spine_ik_legs.py`. It creates/updates a Control Rig Blueprint, adds FK spine controls (`spine_01~03`) and IK leg controls (IK feet + pole vectors L/R), and best-effort wires a minimal graph using common RigVM units (`GetControlTransform`, `SetBoneTransform`, `TwoBoneIKSimple`).
  KO: `Scripts/setup_fk_spine_ik_legs.py`를 추가했습니다. 컨트롤 릭 블루프린트를 생성/갱신하고, FK 스파인 컨트롤(`spine_01~03`)과 IK 레그 컨트롤(발 IK, 폴벡터 좌/우)을 만들며, 일반적인 RigVM 유닛(`GetControlTransform`, `SetBoneTransform`, `TwoBoneIKSimple`)을 사용해 최소 그래프를 구성합니다.

### How to Run / 실행 방법
- EN: In UE 5.5 Python Console:
  KO: UE 5.5 파이썬 콘솔에서:

```
import sys, unreal
sys.path.append(unreal.Paths.project_dir())

import Scripts.setup_fk_spine_ik_legs as rig
# Uses your hint "/Character/SK_Man" and normalizes to /Game/Character/SK_Man.SK_Man
rig.create_fk_spine_ik_legs(
    rig_name="CR_Man",
    package_path="/Game/Rigs",
    skeletal_mesh_hint="/Character/SK_Man",
)
```

- EN: Open the created Control Rig asset and verify controls and execution pins; small pin-name tweaks may be required depending on minor UE version.
  KO: 생성된 컨트롤 릭 자산을 열어 컨트롤 및 실행 핀 연결을 확인하세요. UE 마이너 버전에 따라 핀 이름을 약간 수정해야 할 수 있습니다.

### Assumptions / 가정
- EN: Standard UE skeleton bones: `spine_01`, `spine_02`, `spine_03`, `thigh_l/r`, `calf_l/r`, `foot_l/r`.
  KO: 표준 UE 스켈레톤 본을 가정합니다: `spine_01`, `spine_02`, `spine_03`, `thigh_l/r`, `calf_l/r`, `foot_l/r`.

### Notes / 참고
- EN: APIs differ across 5.x; the script wraps calls with fallbacks. If any unit/pin name mismatch occurs, tell me the error and I’ll patch quickly.
  KO: 5.x 버전 간 API가 다를 수 있어, 스크립트는 폴백을 포함합니다. 유닗/핀 이름 불일치 오류가 나면 메시지를 알려주시면 즉시 패치하겠습니다.

## Files / 파일
- EN: Added `Scripts/setup_fk_spine_ik_legs.py`.
  KO: `Scripts/setup_fk_spine_ik_legs.py`를 추가했습니다.

## Next Steps / 다음 단계
- EN: If your mesh uses Manny/Quinn or custom naming, share exact bone names for spine and legs; I’ll tailor the script and add FK/IK blending controls.
  KO: 메시가 Manny/Quinn 또는 커스텀 네이밍이면 스파인/레그의 정확한 본명을 알려주세요. 스크립트를 맞춤 조정하고 FK/IK 블렌딩 컨트롤도 추가하겠습니다.

