# Fix Forward Solve Controller / Forward Solve 컨트롤러 수정

## Request / 요청
- EN: Error: `AttributeError: 'NoneType' object has no attribute 'add_unit_node_from_struct_path'` when wiring Forward Solve.
  KO: Forward Solve 연결 중 `AttributeError: 'NoneType' object has no attribute 'add_unit_node_from_struct_path'` 오류가 발생합니다.

## Cause / 원인
- EN: The RigVM controller (`vmc`) was `None` for the target graph, so node-add calls dereferenced a null controller.
  KO: 대상 그래프의 RigVM 컨트롤러(`vmc`)가 `None`이어서 노드 추가 호출이 널 컨트롤러에 대해 수행됐습니다.

## Fix / 수정
- EN: In `Scripts/pelvisonly.py`:
  KO: `Scripts/pelvisonly.py`에서 다음을 수정했습니다:
- EN: • Acquire controller robustly: try `get_controller()` first, then `get_controller_by_name(default entry)`, then from `model`.
  KO: • 컨트롤러 획득을 견고화: `get_controller()` → 기본 엔트리 이름으로 `get_controller_by_name()` → `model` 순으로 시도.
- EN: • If controller is still `None`, print a clear warning and skip wiring (prevents crash).
  KO: • 여전히 `None`이면 경고를 출력하고 연결을 건너뜁니다(크래시 방지).
- EN: • Added `_add_unit_any(...)` that tries `add_unit_node` with struct classes and falls back to `add_unit_node_from_struct_path`/`add_struct_node`.
  KO: • 구조체 클래스로 `add_unit_node`를 우선 시도하고, 경로 기반 메서드로 폴백하는 `_add_unit_any(...)`를 추가.

## How to Use / 사용 방법
- EN: Run `pelvisonly.py` again. If you see `[WARN] Could not acquire RigVM controller`, open the created Control Rig once in the editor, then run again (ensures model is initialized). Ensure the asset is checked out if Source Control is enabled.
  KO: `pelvisonly.py`를 다시 실행하세요. `[WARN] Could not acquire RigVM controller`가 보이면 에디터에서 생성된 컨트롤 릭을 한 번 열고 다시 실행하세요(모델 초기화). 소스 컨트롤 사용 시 자산 체크아웃을 확인하세요.

