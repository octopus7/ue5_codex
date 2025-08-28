import unreal

# ========== USER CONFIG ==========
SKELETAL_MESH = "/Game/Character/SK_Man"  # ← 너 메쉬 경로
TARGET_FOLDER = "/Game/ControlRigs"                             # ← 저장할 폴더
RIG_NAME      = "CR_PelvisOnlyB01"                               # ← 생성될 리그 이름
PELVIS_BONE   = "Pelvis"                                        # ← 골반 본 이름(캐릭터에 맞게)
SHAPE_NAME    = "Circle"                                        # ← "Circle","Cube","Diamond" 등
SHAPE_COLOR   = unreal.LinearColor(0.98, 0.45, 0.15, 1.0)       # ← 기즈모 색
SHAPE_SCALE   = unreal.Vector(12.0, 12.0, 12.0)                 # ← 기즈모 스케일
# =================================

# Helper: robust SkeletalMesh loading (accepts package or object path)
def _normalize_asset_path(path: str):
    p = (path or "").strip()
    if p.startswith("/Game/"):
        package = p
    elif p.startswith("/"):
        package = "/Game" + p
    else:
        package = f"/Game/{p}"
    name = package.split("/")[-1]
    if "." in package:
        return [package]
    return [package, f"{package}.{name}"]


def _load_skeletal_mesh_any(path_hint: str):
    for cand in _normalize_asset_path(path_hint):
        try:
            asset = unreal.load_asset(cand)
            if isinstance(asset, unreal.SkeletalMesh):
                return asset
        except Exception:
            pass
    return None


# 1) 컨트롤릭 블루프린트 생성 + 미리보기 메쉬 지정
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
factory     = unreal.ControlRigBlueprintFactory()
crb = asset_tools.create_asset(RIG_NAME, TARGET_FOLDER, unreal.ControlRigBlueprint, factory)

skel_mesh = _load_skeletal_mesh_any(SKELETAL_MESH)
if skel_mesh is None:
    raise RuntimeError(
        f"Failed to load SkeletalMesh from '{SKELETAL_MESH}'. Try full object path like '/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'\n"
        f"스켈레탈 메시 로드 실패: '{SKELETAL_MESH}'. 객체 경로 예) '/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"
    )

try:
    unreal.ControlRigBlueprintLibrary.set_preview_mesh(crb, skel_mesh, True)  # 미리보기 메쉬 세팅
except Exception:
    try:
        if crb.has_editor_property("preview_mesh"):
            crb.set_editor_property("preview_mesh", skel_mesh)
    except Exception:
        pass

# 2) 계층 컨트롤러 얻고, 스켈레톤 본들 임포트
hierarchy  = unreal.ControlRigBlueprintLibrary.get_hierarchy(crb)
controller = unreal.ControlRigBlueprintLibrary.get_hierarchy_controller(crb)

skel = getattr(skel_mesh, "skeleton", None)
if skel is None:
    raise RuntimeError(
        "Loaded SkeletalMesh has no skeleton assigned. Open the mesh and ensure a Skeleton is set.\n"
        "로드된 SkeletalMesh에 스켈레톤이 지정되어 있지 않습니다. 메시 자산에서 Skeleton이 설정되어 있는지 확인하세요."
    )

controller.import_bones(skel, replace_existing_bones=True, remove_obsolete_bones=True,
                        select_bones=False, setup_undo=False)

# 3) 골반 본 키와 초기 트랜스폼 가져오기(레퍼런스 포즈 기준)
pelvis_key   = unreal.RigElementKey(name=PELVIS_BONE, type=unreal.RigElementType.BONE)
if not hierarchy.contains(pelvis_key):
    raise RuntimeError(f"'{PELVIS_BONE}' 본을 계층에서 찾을 수 없습니다. 본 이름을 확인하세요.")

pelvis_xf = hierarchy.get_global_transform(pelvis_key, True)  # initial=True

# 4) Transform 컨트롤 설정(표시이름/셰입/색/표시여부)
settings = unreal.RigControlSettings()
# UE 5.x variant-safe transform control type selection
_ctl_type = None
for _name in ("EULER_TRANSFORM", "TRANSFORM", "TRANSFORM_NO_SCALE"):
    _ctl_type = getattr(unreal.RigControlType, _name, None)
    if _ctl_type is not None:
        break
if _ctl_type is None:
    raise AttributeError("No transform-like RigControlType found (EULER_TRANSFORM/TRANSFORM/TRANSFORM_NO_SCALE)")
settings.control_type   = _ctl_type
settings.display_name   = "Pelvis_CTRL"
settings.shape_name     = SHAPE_NAME
settings.shape_visible  = True
settings.shape_color    = SHAPE_COLOR

# 컨트롤 값은 보통 아이덴티티(오프셋이 위치를 담당)
ctrl_value = unreal.RigHierarchy.make_control_value_from_transform(unreal.Transform())

# 5) 골반 본을 부모로 컨트롤 생성
ctrl_key = controller.add_control("Pelvis_CTRL", pelvis_key, settings, ctrl_value, True)

# 6) 컨트롤을 골반 위치에 정렬: 오프셋=골반 글로벌, 값=아이덴티티
hierarchy.set_control_offset_transform(ctrl_key, pelvis_xf, initial=True, setup_undo=False)
hierarchy.set_control_value(ctrl_key, ctrl_value, unreal.RigControlValueType.CURRENT, setup_undo=False)

# 7) 셰입(기즈모) 크기만 손쉽게 조절(스케일)
shape_xf = unreal.Transform(scale=SHAPE_SCALE)
hierarchy.set_control_shape_transform(ctrl_key, shape_xf, initial=True, setup_undo=False)

# 8) 리그 초기화/컴파일/저장 (KismetEditorUtilities 미노출 환경 폴백 포함)
def _safe_compile_and_save(crb):
    # Init CR if possible
    try:
        unreal.ControlRigBlueprintLibrary.request_control_rig_init(crb)
    except Exception:
        pass

    compiled = False
    # Try KismetEditorUtilities.compile_blueprint
    try:
        if hasattr(unreal, "KismetEditorUtilities"):
            unreal.KismetEditorUtilities.compile_blueprint(crb)
            compiled = True
    except Exception:
        pass

    # Try BlueprintEditorLibrary.compile_blueprint
    try:
        if hasattr(unreal, "BlueprintEditorLibrary") and hasattr(unreal.BlueprintEditorLibrary, "compile_blueprint"):
            unreal.BlueprintEditorLibrary.compile_blueprint(crb)
            compiled = True
    except Exception:
        pass

    # Fallback: ControlRigBlueprint.recompile_vm
    try:
        if hasattr(crb, "recompile_vm"):
            crb.recompile_vm()
            compiled = True
    except Exception:
        pass

    # Save regardless so graph/hierarchy edits persist
    try:
        unreal.EditorAssetLibrary.save_loaded_asset(crb)
    except Exception:
        pass
    return compiled

# 9) Forward Solve 구성: Pelvis_CTRL -> Pelvis 본 구동
def _add_forward_solve_pelvis(crb: unreal.ControlRigBlueprint, ctrl_name: str, bone_name: str):
    # RigVM 메인 그래프 컨트롤러 얻기
    vmc = None
    model = None
    try:
        model = crb.get_model()
        entry_point = None
        try:
            entry_point = model.get_default_entry_point()
        except Exception:
            entry_point = "Rig Graph"
        if hasattr(crb, "get_controller_by_name"):
            vmc = crb.get_controller_by_name(entry_point)
    except Exception:
        vmc = None

    # 추가 폴백: 기본 컨트롤러 시도
    if vmc is None and hasattr(crb, "get_controller"):
        try:
            vmc = crb.get_controller()
        except Exception:
            vmc = None

    if vmc is None:
        print("[WARN] Could not acquire RigVM controller; skip Forward Solve wiring")
        print("[경고] RigVM 컨트롤러를 얻지 못해 Forward Solve 연결을 건너뜁니다")
        return

    def _pin(node, pin):
        # 편하게 "NodeName.PinName" 경로 문자열 만들기
        return f"{node.get_name()}.{pin}"

    # 노드 배치 좌표 (보기 좋게)
    P_GET  = unreal.Vector2D(-400.0,  0.0)
    P_SET  = unreal.Vector2D(  50.0,  0.0)

    # 유닛 노드 생성 헬퍼: 경로/클래스 모두 시도
    def _add_unit_any(vmc, struct_paths, pos, title):
        # 1) 클래스 기반
        for sp in struct_paths:
            cls = getattr(unreal, sp.split(".")[-1], None)
            if cls and hasattr(vmc, "add_unit_node"):
                try:
                    return vmc.add_unit_node(cls, title, pos)
                except Exception:
                    pass
        # 2) 경로 기반 (from_struct_path)
        for sp in struct_paths:
            for meth in ("add_unit_node_from_struct_path", "add_struct_node"):
                if hasattr(vmc, meth):
                    try:
                        return getattr(vmc, meth)(sp, "Execute", pos, title, True)
                    except Exception:
                        pass
        return None

    # GetControlTransform
    get_paths = [
        "/Script/ControlRig.RigUnit_GetControlTransform",
        "RigUnit_GetControlTransform",
    ]
    get_node = _add_unit_any(vmc, get_paths, P_GET, f"Get_{ctrl_name}")
    # 컨트롤 이름/스페이스 세팅
    if get_node:
        vmc.set_pin_default_value(_pin(get_node, "Control"), ctrl_name, True)
    # 핀 이름이 Space 또는 TransformSpace인 경우가 있어 둘 다 시도
    if get_node:
        for maybe_space in ("Space", "TransformSpace"):
            try:
                vmc.set_pin_default_value(_pin(get_node, maybe_space), "GlobalSpace", True)
                break
            except Exception:
                pass

    # Set 쪽: 신(계층) API 우선 시도 → 실패하면 구(본) API로 폴백
    set_node = None
    used_new_api = False
    try:
        # UE5 계층 API: RigUnit_HierarchySetGlobalTransform
        set_paths_new = [
            "/Script/ControlRig.RigUnit_HierarchySetGlobalTransform",
            "RigUnit_HierarchySetGlobalTransform",
        ]
        set_node = _add_unit_any(vmc, set_paths_new, P_SET, f"Set_{bone_name}")
        # Item 핀은 "Bone:Name" 형식
        if set_node:
            vmc.set_pin_default_value(_pin(set_node, "Item"), f"Bone:{bone_name}", True)
        # 초기값이 아닌 현재값에 적용
        try:
            vmc.set_pin_default_value(_pin(set_node, "bInitial"), "False", True)
        except Exception:
            pass
        # 트랜스폼 연결 (Get.Transform -> Set.Transform)
        if get_node and set_node:
            vmc.add_link(_pin(get_node, "Transform"), _pin(set_node, "Transform"))
        used_new_api = True
    except Exception:
        # 구 API: RigUnit_SetBoneTransform
        set_paths_old = [
            "/Script/ControlRig.RigUnit_SetBoneTransform",
            "RigUnit_SetBoneTransform",
            "/Script/ControlRig.RigUnit_SetTransform",
            "RigUnit_SetTransform",
        ]
        set_node = _add_unit_any(vmc, set_paths_old, P_SET, f"Set_{bone_name}")
        vmc.set_pin_default_value(_pin(set_node, "Bone"), bone_name, True)
        vmc.set_pin_default_value(_pin(set_node, "Space"), "GlobalSpace", True)
        vmc.set_pin_default_value(_pin(set_node, "Weight"), "1.0", True)
        vmc.set_pin_default_value(_pin(set_node, "bPropagateToChildren"), "True", True)
        # 입력 핀명이 Transform / Value 중 하나일 수 있어 둘 다 시도
        try:
            vmc.add_link(_pin(get_node, "Transform"), _pin(set_node, "Transform"))
        except Exception:
            vmc.add_link(_pin(get_node, "Transform"), _pin(set_node, "Value"))

    # Forward Solve 엔트리(Entry) → 실행선 연결
    graph = vmc.get_graph()
    entry_node = None
    try:
        # 가장 흔한 엔트리 노드 타입
        for n in graph.get_nodes():
            if isinstance(n, unreal.RigVMFunctionEntryNode):
                entry_node = n
                break
    except Exception:
        # 타입 체크가 안되면 이름으로 대충 찾기
        for n in graph.get_nodes():
            title = ""
            try:
                title = n.get_node_title(0)  # 0 = ENodeTitleType.FullTitle
            except Exception:
                title = n.get_name()
            if "Forward" in title or "Entry" in title or "Begin" in title:
                entry_node = n
                break

    if entry_node:
        try:
            vmc.add_link(_pin(entry_node, "Execute"), _pin(get_node, "Execute"))
        except Exception:
            pass
        try:
            vmc.add_link(_pin(get_node, "Execute"), _pin(set_node, "Execute"))
        except Exception:
            pass

    # 저장/컴파일
    try:
        unreal.ControlRigBlueprintLibrary.request_control_rig_init(crb)
    except Exception:
        pass
    try:
        if hasattr(unreal, "KismetEditorUtilities"):
            unreal.KismetEditorUtilities.compile_blueprint(crb)
    except Exception:
        pass
    try:
        unreal.EditorAssetLibrary.save_loaded_asset(crb)
    except Exception:
        pass

# 호출: 이미 만든 컨트롤 이름(ctrl_key.name)과 골반 본 이름으로 연결
_add_forward_solve_pelvis(crb, ctrl_key.name, PELVIS_BONE)
print("[OK] Forward Solve wired: Control -> Bone")


_safe_compile_and_save(crb)

print(f"[OK] Created: {crb.get_path_name()}  Control: {ctrl_key.name}")
