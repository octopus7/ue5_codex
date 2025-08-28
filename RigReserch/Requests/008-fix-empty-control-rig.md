# Fix Empty Control Rig / 빈 컨트롤 릭 수정

## Request / 요청
- EN: Rig hierarchy and graph are empty even though the Skeletal Mesh is linked. It looks like no rig work happened.
  KO: 스켈레탈 메시는 연결되어 있지만 릭 하이어라키와 그래프가 비어 있습니다. 릭 작업이 전혀 안 된 상태 같습니다.

## Response / 응답
- EN: I updated the scripts to import skeleton bones into the rig hierarchy and to persist edits using editor transactions. This addresses empty hierarchy and missing graph nodes.
  KO: 스켈레톤 본을 릭 하이어라키로 가져오고, 에디터 트랜잭션으로 변경을 보존하도록 스크립트를 보강했습니다. 이로써 빈 하이어라키와 그래프 누락 문제를 해결합니다.

### Changes / 변경 사항
- EN: `Scripts/create_control_rig.py`: added `_import_bones_from_skeleton_if_possible()` and call it after assigning the preview mesh; wraps changes in `ScopedEditorTransaction`, recompiles, and saves.
  KO: `Scripts/create_control_rig.py`: `_import_bones_from_skeleton_if_possible()`를 추가하고 프리뷰 메시 지정 후 호출합니다. `ScopedEditorTransaction`으로 감싸고 리컴파일/저장을 수행합니다.
- EN: `Scripts/setup_fk_spine_ik_legs.py`: imports bones before control creation; wraps control/graph edits in transactions; recompiles and saves.
  KO: `Scripts/setup_fk_spine_ik_legs.py`: 컨트롤 생성 전에 본을 가져오고, 컨트롤/그래프 편집을 트랜잭션으로 감쌉니다. 리컴파일 및 저장을 수행합니다.

### How to Re-run / 재실행 방법
- EN: Use `Tools > Python > Execute Python Script...` to run `Scripts/run_fk_spine_ik_legs.py` (no parameters), or run `Scripts/setup_fk_spine_ik_legs.py` directly.
  KO: `Tools > Python > Execute Python Script...`에서 `Scripts/run_fk_spine_ik_legs.py`(무파라미터) 또는 `Scripts/setup_fk_spine_ik_legs.py`를 직접 실행하세요.

### Expected Result / 기대 결과
- EN: The Control Rig Blueprint should now show imported bones in the hierarchy, added controls (`C_pelvis`, `C_spine_01~03`, `C_ik_foot_l/r`, `C_pole_l/r`), and initial FK/IK nodes in the graph.
  KO: 컨트롤 릭 블루프린트의 하이어라키에 본이 가져와지고, 컨트롤(`C_pelvis`, `C_spine_01~03`, `C_ik_foot_l/r`, `C_pole_l/r`)이 추가되며, 그래프에 FK/IK 초기 노드가 표시됩니다.

### If Issues Persist / 문제가 지속되면
- EN: Share any Output Log errors. Pin-name or function name mismatches vary across UE 5.x; I’ll patch the calls for your exact version.
  KO: 출력 로그 오류를 공유해 주세요. UE 5.x 버전에 따라 핀/함수명이 다를 수 있어, 환경에 맞춰 즉시 패치하겠습니다.

