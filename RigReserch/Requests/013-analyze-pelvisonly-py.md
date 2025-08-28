# Analyze pelvisonly.py / pelvisonly.py 분석

## Request / 요청
- EN: Analyze `Scripts/pelvisonly.py`.
  KO: `Scripts/pelvisonly.py`를 분석해 주세요.

## Summary / 요약
- EN: The script creates a new Control Rig Blueprint, imports skeleton bones, adds a single Transform control for the pelvis, aligns its gizmo to the pelvis bone, compiles, and saves. It does not add any Forward Solve nodes, so the graph remains empty by design.
  KO: 새 컨트롤 릭 블루프린트를 만들고 본을 가져온 뒤, 골반용 Transform 컨트롤을 하나 추가해 골반 위치로 정렬하고 컴파일/저장합니다. 포워드 솔브 그래프 노드는 추가하지 않으므로, 설계상 그래프는 비어 있습니다.

## Step-by-Step / 동작 단계
- EN: Creates asset via `ControlRigBlueprintFactory` with `RIG_NAME` under `TARGET_FOLDER`.
  KO: `ControlRigBlueprintFactory`로 `TARGET_FOLDER`에 `RIG_NAME` 자산을 생성합니다.
- EN: Sets preview mesh to `SKELETAL_MESH` and imports bones from `skel_mesh.skeleton` using `controller.import_bones(...)`.
  KO: 프리뷰 메시에 `SKELETAL_MESH`를 지정하고 `controller.import_bones(...)`로 스켈레톤 본을 가져옵니다.
- EN: Validates the pelvis bone exists, gets its initial global transform.
  KO: 골반 본 존재를 확인하고 초기 글로벌 트랜스폼을 얻습니다.
- EN: Defines a Transform-type control `Pelvis_CTRL` with shape, color, and scale settings.
  KO: 셰입/색/스케일 설정을 가진 Transform 타입 컨트롤 `Pelvis_CTRL`을 정의합니다.
- EN: Adds the control parented to the pelvis bone, sets offset to pelvis transform, keeps control value identity.
  KO: 컨트롤을 골반 본에 부모로 추가하고, 오프셋을 골반 트랜스폼으로, 값은 아이덴티티로 둡니다.
- EN: Requests rig init, compiles blueprint, and saves.
  KO: 리그 초기화를 요청하고 블루프린트를 컴파일/저장합니다.

## What You’ll See / 기대 결과
- EN: In Rig Hierarchy: a control `Pelvis_CTRL` under Controls; in Graph: still only the start node (no units added).
  KO: 릭 하이어라키에 `Pelvis_CTRL` 컨트롤이 보이고, 그래프는 시작 노드만 보입니다(유닛 추가 없음).

## Notable Details / 주목 포인트
- EN: Graph is empty by design: the script does not create `SetBoneTransform` or other units. Moving `Pelvis_CTRL` will not drive the bone without a graph.
  KO: 의도적으로 그래프를 만들지 않아 `SetBoneTransform` 등의 유닛이 없습니다. 그래프가 없으면 `Pelvis_CTRL`을 움직여도 본이 구동되지 않습니다.
- EN: Uses `controller.add_control(name, parentKey, settings, value, setup_undo=True)`; this attaches a control element with pelvis as parent.
  KO: `controller.add_control(name, parentKey, settings, value, setup_undo=True)`로 골반 본을 부모로 컨트롤을 추가합니다.
- EN: Bone import uses `replace_existing_bones=True`; if rerun, it may refresh the hierarchy, which is generally safe but may reset manual tweaks.
  KO: 본 임포트에 `replace_existing_bones=True`가 설정되어 재실행 시 하이어라키가 새로고침될 수 있습니다. 수동 수정은 초기화될 수 있습니다.
- EN: Asset write requires checkout when Source Control is enabled; otherwise changes may not persist.
  KO: 소스 컨트롤 사용 시 자산 체크아웃이 필요하며, 그렇지 않으면 변경 사항이 저장되지 않을 수 있습니다.

## Suggestions / 개선 제안
- EN: If you want the pelvis control to actually drive the pelvis bone, add Forward Solve nodes: `RigUnit_GetControlTransform(Pelvis_CTRL)` → `RigUnit_SetBoneTransform(pelvis)`.
  KO: 골반 컨트롤이 본을 구동하도록 하려면 포워드 솔브 그래프에 `RigUnit_GetControlTransform(Pelvis_CTRL)` → `RigUnit_SetBoneTransform(pelvis)` 노드를 추가하세요.
- EN: Make the script idempotent: check if the control exists before adding; skip re-importing bones unless needed.
  KO: 스크립트를 멱등하게: 컨트롤 존재 여부를 확인해 중복 생성을 방지하고, 필요할 때만 본 재임포트.
- EN: Wrap edits in `ScopedEditorTransaction` to ensure undo/redo and better persistence.
  KO: 변경을 `ScopedEditorTransaction`으로 감싸 언두/리두 및 보존성을 강화하세요.
- EN: Add a `main()` guard so importing the module doesn’t execute side-effects inadvertently.
  KO: 모듈 임포트 시 부작용을 방지하려면 `main()` 가드(`if __name__ == "__main__":`)를 추가하세요.
- EN: Align `TARGET_FOLDER` with your project convention (e.g., `/Game/Rigs`).
  KO: `TARGET_FOLDER`를 프로젝트 규칙(예: `/Game/Rigs`)에 맞추세요.

## Compatibility / 호환성
- EN: Functions from `ControlRigBlueprintLibrary` and the controller API vary slightly across UE 5.x; this script matches typical 5.2–5.5 usage. If an API mismatch error appears, share it and I will adapt.
  KO: `ControlRigBlueprintLibrary`와 컨트롤러 API는 UE 5.x에서 약간씩 다릅니다. 본 스크립트는 5.2–5.5 범위의 일반 사용법에 맞춰져 있습니다. 불일치 오류가 있으면 알려주시면 맞춰 수정하겠습니다.

