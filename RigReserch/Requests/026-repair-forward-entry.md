# Repair Forward Solve Entry / Forward Solve 엔트리 복구

## Request / 요청
- EN: Only `GetControlTransform` remains and the original Forward Solve entry node disappeared.
  KO: `GetControlTransform`만 있고 원래 있어야 할 Forward Solve 엔트리 노드가 사라졌습니다.

## Response / 응답
- EN: I hardened `Scripts/pelvisonly.py` so it acquires the correct RigVM controller and graph before adding nodes, and it now warns (without modifying the graph) if the controller/graph/entry is missing.
  KO: `Scripts/pelvisonly.py`가 노드 추가 전에 올바른 RigVM 컨트롤러와 그래프를 확보하도록 보강했으며, 컨트롤러/그래프/엔트리가 없으면 그래프를 건드리지 않고 경고만 출력하도록 변경했습니다.

## What Changed / 변경 사항
- EN: Controller acquisition order: `get_controller()` → `get_controller_by_name(default entry)` → `model.get_controller()`.
  KO: 컨트롤러 확보 순서: `get_controller()` → `get_controller_by_name(기본 엔트리)` → `model.get_controller()`.
- EN: Guarded `_add_unit_any(...)` to avoid dereferencing `None` and to try both class-based and struct-path node creation.
  KO: `_add_unit_any(...)`에 가드를 추가해 `None` 역참조를 방지하고 클래스/경로 기반 노드 생성을 모두 시도합니다.
- EN: If graph or entry node is missing, the script prints `[WARN]` and skips wiring rather than corrupting the graph.
  KO: 그래프나 엔트리 노드가 없으면 `[WARN]`을 출력하고 연결을 건너뛰어 그래프 손상을 방지합니다.

## Next Steps / 다음 단계
- EN: Open the created Control Rig asset once in the editor so the model/graph initialize, then run `pelvisonly.py` again. Ensure Source Control checkout.
  KO: 에디터에서 생성된 컨트롤 릭 자산을 한 번 열어 모델/그래프가 초기화되도록 한 뒤 `pelvisonly.py`를 다시 실행하세요. 소스 컨트롤 체크아웃을 확인하세요.
- EN: If `[WARN] Entry node not found` still appears, tell me; I can add a one-off repair that recreates the default Forward Solve function for your UE 5.5 build.
  KO: `[WARN] 엔트리 노드를 찾지 못했습니다`가 계속 보이면 알려주세요. UE 5.5 환경에 맞춘 엔트리 복구 스크립트를 추가해 드리겠습니다.

