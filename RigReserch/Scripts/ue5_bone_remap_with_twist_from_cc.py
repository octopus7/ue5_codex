# Blender 4.x / 3.x
# 사용법:
# 1) Object Mode에서 Armature 오브젝트 선택
# 2) Scripting 탭에서 실행. 아래 옵션 조정 후 run()
import bpy
import re
from collections import defaultdict

# ========== 사용자 옵션 ==========
ARMATURE_NAME = None  # None: 활성 오브젝트 사용
DRY_RUN = False       # True: 미리보기(실제 변경 X)
VERBOSE = True

# ====== 기본 매핑(정확 일치) : Spine/Neck/Head/Clavicle/Arm/Leg/Foot/ToeBase→ball ======
EXACT_MAP = {
    # Root/Spine/Neck/Head
    "CC_Base_Hip": "pelvis",
    "CC_Base_Pelvis": "pelvis",
    "Hips": "pelvis",

    "CC_Base_Spine01": "spine_01",
    "CC_Base_Spine02": "spine_02",
    "CC_Base_Spine03": "spine_03",
    "Spine": "spine_01",
    "Spine1": "spine_02",
    "Spine2": "spine_03",

    "CC_Base_NeckTwist01": "neck_01",
    "CC_Base_Neck": "neck_01",
    "Neck": "neck_01",
    "CC_Base_Head": "head",
    "Head": "head",

    # Clavicle / Arm / Forearm / Hand
    "CC_Base_L_Clavicle": "clavicle_l",
    "CC_Base_R_Clavicle": "clavicle_r",
    "LeftShoulder": "clavicle_l",
    "RightShoulder": "clavicle_r",

    "CC_Base_L_Upperarm": "upperarm_l",
    "CC_Base_R_Upperarm": "upperarm_r",
    "LeftArm": "upperarm_l",
    "RightArm": "upperarm_r",

    "CC_Base_L_Forearm": "lowerarm_l",
    "CC_Base_R_Forearm": "lowerarm_r",
    "LeftForeArm": "lowerarm_l",
    "RightForeArm": "lowerarm_r",

    "CC_Base_L_Hand": "hand_l",
    "CC_Base_R_Hand": "hand_r",
    "LeftHand": "hand_l",
    "RightHand": "hand_r",

    # Leg / Calf / Foot / ToeBase → ball
    "CC_Base_L_Thigh": "thigh_l",
    "CC_Base_R_Thigh": "thigh_r",
    "LeftUpLeg": "thigh_l",
    "RightUpLeg": "thigh_r",

    "CC_Base_L_Calf": "calf_l",
    "CC_Base_R_Calf": "calf_r",
    "LeftLeg": "calf_l",
    "RightLeg": "calf_r",

    "CC_Base_L_Foot": "foot_l",
    "CC_Base_R_Foot": "foot_r",
    "LeftFoot": "foot_l",
    "RightFoot": "foot_r",

    "CC_Base_L_ToeBase": "ball_l",
    "CC_Base_R_ToeBase": "ball_r",
    "LeftToeBase": "ball_l",
    "RightToeBase": "ball_r",
}

# ====== 손가락/발가락 규칙 ======
FINGER_ORDER = {
    "Thumb": "thumb",
    "Index": "index",
    "Middle": "middle",
    "Ring": "ring",
    "Pinky": "pinky",
}
FINGER_PATTERN = re.compile(r"CC_Base_(L|R)_(Thumb|Index|Middle|Ring|Pinky)(\d+)$")
ALT_FINGER_PATTERN = re.compile(r"(Left|Right)Hand(Thumb|Index|Middle|Ring|Pinky)(\d+)$")
TOE_PATTERN = re.compile(r"CC_Base_(L|R)_(BigToe|SmallToe)(\d+)$")

SIDE_SUFFIX = {"L": "_l", "R": "_r", "Left": "_l", "Right": "_r"}

# ====== 트위스트 규칙 ======
# 예) CC_Base_L_UpperarmTwist01 → upperarm_twist_01_l
#     CC_Base_R_ForearmTwist2   → lowerarm_twist_02_r
#     LeftUpLegTwist            → thigh_twist_01_l
#     RightLegTwist02           → calf_twist_02_r
TWIST_CC_PATTERN = re.compile(
    r"CC_Base_(L|R)_(Upperarm|Forearm|Thigh|Calf)Twist(?:0?(\d+))?$", re.IGNORECASE
)
TWIST_ALT_PATTERN = re.compile(
    r"(Left|Right)(Arm|ForeArm|UpLeg|Leg)Twist(?:0?(\d+))?$", re.IGNORECASE
)

def norm_index(idx_str: str | None) -> str:
    """없으면 01, 있으면 2자리로."""
    if not idx_str or not idx_str.isdigit():
        return "01"
    return f"{int(idx_str):02d}"

def twist_target_name(segment: str, side: str, idx_str: str | None) -> str:
    # 상지/하지 & UE 표준 명칭 정규화
    seg = segment.lower()
    if seg == "forearm": seg = "lowerarm"
    if seg == "upleg":   seg = "thigh"
    if seg == "leg":     seg = "calf"
    index = norm_index(idx_str)
    side_suf = SIDE_SUFFIX["Left" if side.lower()=="left" else ("L" if side.upper()=="L" else ("Right" if side.lower()=="right" else "R"))]
    return f"{seg}_twist_{index}{side_suf}"

def bone_new_name(bname: str) -> str | None:
    # 1) Exact 매핑
    if bname in EXACT_MAP:
        return EXACT_MAP[bname]

    # 2) 손가락
    m = FINGER_PATTERN.match(bname)
    if m:
        side, fname, idx = m.groups()
        ue_f = FINGER_ORDER.get(fname, fname.lower())
        return f"{ue_f}_{int(idx):02d}{SIDE_SUFFIX[side]}"

    m = ALT_FINGER_PATTERN.match(bname)
    if m:
        side, fname, idx = m.groups()
        ue_f = FINGER_ORDER.get(fname, fname.lower())
        return f"{ue_f}_{int(idx):02d}{SIDE_SUFFIX[side]}"

    # 3) 발가락(체인 유지 버전: toe_01_l/r)
    m = TOE_PATTERN.match(bname)
    if m:
        side, toe_name, idx = m.groups()
        return f"toe_{int(idx):02d}{SIDE_SUFFIX[side]}"

    # 4) 트위스트(CC 스타일)
    m = TWIST_CC_PATTERN.match(bname)
    if m:
        side, segment, idx = m.groups()
        return twist_target_name(segment, side, idx)

    # 5) 트위스트(대체/믹사모 스타일)
    m = TWIST_ALT_PATTERN.match(bname)
    if m:
        side, segment, idx = m.groups()
        return twist_target_name(segment, side, idx)

    return None  # 변경 없음

def collect_vgroup_users(arm_obj):
    users = []
    for ob in bpy.data.objects:
        if ob.type == 'MESH' and ob.find_armature() == arm_obj:
            users.append(ob)
    return users

def rename_vertex_groups(mesh_obj, mapping):
    renamed = []
    for vg in list(mesh_obj.vertex_groups):
        if vg.name in mapping:
            try:
                vg.name = mapping[vg.name]
                renamed.append(vg.name)
            except Exception as e:
                print(f"[VG] rename failed {vg.name}: {e}")
    return renamed

def run():
    arm_obj = bpy.data.objects.get(ARMATURE_NAME) if ARMATURE_NAME else bpy.context.active_object
    if not arm_obj or arm_obj.type != 'ARMATURE':
        raise RuntimeError("Armature 오브젝트를 선택하거나 ARMATURE_NAME를 올바르게 지정하세요.")

    arm = arm_obj.data
    old_to_new = {}
    conflicts = defaultdict(list)

    # 매핑 수집
    for bone in arm.bones:
        new_name = bone_new_name(bone.name)
        if new_name and new_name != bone.name:
            old_to_new[bone.name] = new_name
            conflicts[new_name].append(bone.name)

    if VERBOSE:
        print("=== Rename Plan (Bone) ===")
        for k, v in old_to_new.items():
            print(f"{k} -> {v}")
        multi = {t: olds for t, olds in conflicts.items() if len(olds) > 1}
        if multi:
            print("=== Conflicts (same target) ===")
            for tgt, olds in multi.items():
                print(f"{tgt} <= {olds}")

    if DRY_RUN:
        print("[DRY_RUN] 미리보기 모드(본/버텍스그룹 변경 없음)")
        return

    # 충돌 방지: 임시 프리픽스 → 실명 → 임시 복구
    temp_prefix = "__TMP__"
    taken = set(b.name for b in arm.bones)
    for old, new in list(old_to_new.items()):
        if new in taken:
            for b in arm.bones:
                if b.name == new:
                    b.name = temp_prefix + new
            taken = set(b.name for b in arm.bones)

    for old, new in old_to_new.items():
        for b in arm.bones:
            if b.name == old:
                b.name = new
                break

    for b in arm.bones:
        if b.name.startswith(temp_prefix):
            b.name = b.name[len(temp_prefix):]

    # Vertex Group 동기화
    users = collect_vgroup_users(arm_obj)
    for m in users:
        rename_vertex_groups(m, old_to_new)

    print(f"[Done] Bones renamed: {len(old_to_new)} | Mesh users: {[m.name for m in users]}")

if __name__ == "__main__":
    run()
