# Blender 4.x | Vertex Weights: Replace-All from Source to Target (Same Topology)
import bpy

def replace_weights_same_topology(src_obj, dst_obj, *, normalize=True, limit_influences=4, clean_thresh=0.0):
    # 0) 기본 체크
    if src_obj.type != 'MESH' or dst_obj.type != 'MESH':
        raise ValueError("Both source and target must be Mesh objects.")
    if len(src_obj.data.vertices) != len(dst_obj.data.vertices):
        raise ValueError("Topology mismatch: vertex counts differ (src {}, dst {})."
                         .format(len(src_obj.data.vertices), len(dst_obj.data.vertices)))

    # 1) 타겟의 기존 버텍스 그룹 전체 삭제
    if dst_obj.vertex_groups:
        dst_obj.vertex_groups.clear()

    # 2) 소스의 버텍스 그룹을 이름 그대로 타겟에 생성
    #    src_index -> dst_group(object) 매핑을 만들어 둔다.
    src_groups = src_obj.vertex_groups
    dst_group_by_src_index = {}
    for g in src_groups:
        dst_group_by_src_index[g.index] = dst_obj.vertex_groups.new(name=g.name)

    # 3) 웨이트 복사 (토폴로지 동일하므로 버텍스 인덱스 1:1 매칭)
    #    src의 각 버텍스가 가진 g_elem(group index, weight)을 타겟의 동일 그룹에 기록
    src_mesh = src_obj.data
    for v in src_mesh.vertices:
        for g_elem in v.groups:
            w = g_elem.weight
            if w > 0.0:
                dst_group = dst_group_by_src_index.get(g_elem.group)
                if dst_group:
                    dst_group.add([v.index], w, 'REPLACE')

    # 4) 후처리(정규화/인플루언스 제한/정규화)
    prev_active = bpy.context.view_layer.objects.active
    try:
        bpy.context.view_layer.objects.active = dst_obj
        # Weight Paint 모드로 들어가야 일부 오퍼레이터가 정상 동작
        bpy.ops.object.mode_set(mode='WEIGHT_PAINT')
        if normalize:
            bpy.ops.object.vertex_group_normalize_all(lock_active=False)
        if limit_influences and limit_influences > 0:
            bpy.ops.object.vertex_group_limit_total(limit=limit_influences)
            if normalize:
                bpy.ops.object.vertex_group_normalize_all(lock_active=False)
        if clean_thresh and clean_thresh > 0.0:
            bpy.ops.object.vertex_group_clean(group_select_mode='ALL', limit=clean_thresh)
            if normalize:
                bpy.ops.object.vertex_group_normalize_all(lock_active=False)
        bpy.ops.object.mode_set(mode='OBJECT')
    finally:
        bpy.context.view_layer.objects.active = prev_active

    print("Weights replaced: {} → {} (groups: {})."
          .format(src_obj.name, dst_obj.name, len(src_groups)))


# -----------------------------
# 실행부: 선택 규칙
#   - 소스 먼저 선택
#   - 타겟을 마지막(active)으로 선택
# -----------------------------
sel = [o for o in bpy.context.selected_objects if o.type == 'MESH']
if len(sel) < 2:
    raise RuntimeError("Select source mesh first, then target mesh as ACTIVE (last).")

target = bpy.context.view_layer.objects.active
sources = [o for o in sel if o != target]
if len(sources) != 1:
    raise RuntimeError("Exactly one source and one target required. (Multiple sources not supported in this run)")

source = sources[0]

# 안전: 모드 정리
for o in (source, target):
    if o.mode != 'OBJECT':
        bpy.context.view_layer.objects.active = o
        bpy.ops.object.mode_set(mode='OBJECT')

# 실행: limit_influences=4 는 UE용 권장
replace_weights_same_topology(
    source,
    target,
    normalize=True,
    limit_influences=4,   # UE에서 버텍스당 4 인플루언스 권장. 필요없으면 0 또는 None
    clean_thresh=0.0      # 0.001 등으로 잡으면 미세 웨이트 삭제
)
print("Done.")
