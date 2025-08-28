import unreal

# ========== USER CONFIG ==========
SKELETAL_MESH = "/Game/Character/SK_Man"  # ← 너 메쉬 경로
TARGET_FOLDER = "/Game/ControlRigs"                             # ← 저장할 폴더
RIG_NAME      = "CR_PelvisOnly2"                                 # ← 생성될 리그 이름
PELVIS_BONE   = "Pelvis"                                        # ← 골반 본 이름(캐릭터에 맞게)
SHAPE_NAME    = "Circle"                                        # ← "Circle","Cube","Diamond" 등
SHAPE_COLOR   = unreal.LinearColor(0.98, 0.45, 0.15, 1.0)       # ← 기즈모 색
SHAPE_SCALE   = unreal.Vector(12.0, 12.0, 12.0)                 # ← 기즈모 스케일
# =================================

# 1) 컨트롤릭 블루프린트 생성 + 미리보기 메쉬 지정
asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
factory     = unreal.ControlRigBlueprintFactory()
crb = asset_tools.create_asset(RIG_NAME, TARGET_FOLDER, unreal.ControlRigBlueprint, factory)

skel_mesh = unreal.load_asset(SKELETAL_MESH)
unreal.ControlRigBlueprintLibrary.set_preview_mesh(crb, skel_mesh, True)  # 미리보기 메쉬 세팅

# 2) 계층 컨트롤러 얻고, 스켈레톤 본들 임포트
hierarchy  = unreal.ControlRigBlueprintLibrary.get_hierarchy(crb)
controller = unreal.ControlRigBlueprintLibrary.get_hierarchy_controller(crb)
controller.import_bones(skel_mesh.skeleton, replace_existing_bones=True, remove_obsolete_bones=True,
                        select_bones=False, setup_undo=False)

# 3) 골반 본 키와 초기 트랜스폼 가져오기(레퍼런스 포즈 기준)
pelvis_key   = unreal.RigElementKey(name=PELVIS_BONE, type=unreal.RigElementType.BONE)
if not hierarchy.contains(pelvis_key):
    raise RuntimeError(f"'{PELVIS_BONE}' 본을 계층에서 찾을 수 없습니다. 본 이름을 확인하세요.")

pelvis_xf = hierarchy.get_global_transform(pelvis_key, True)  # initial=True

# 4) Transform 컨트롤 설정(표시이름/셰입/색/표시여부)
settings = unreal.RigControlSettings()
settings.control_type   = unreal.RigControlType.TRANSFORM
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

# 8) 리그 초기화/컴파일/저장
unreal.ControlRigBlueprintLibrary.request_control_rig_init(crb)
unreal.KismetEditorUtilities.compile_blueprint(crb)
unreal.EditorAssetLibrary.save_loaded_asset(crb)

print(f"[OK] Created: {crb.get_path_name()}  Control: {ctrl_key.name}")
