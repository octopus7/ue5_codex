# Run Script Without Parameters / 파라미터 없이 스크립트 실행

## Request / 요청
- EN: Paths are already set; I don’t want to pass parameters. Is there a way to run the file directly?
  KO: 이미 경로가 지정되어 있어 파라미터를 넣고 싶지 않습니다. 파일을 바로 실행하는 방법이 있나요?

## Response / 응답
- EN: Added a no-parameter entry point to `Scripts/setup_fk_spine_ik_legs.py` and a small runner `Scripts/run_fk_spine_ik_legs.py` so you can execute directly from the menu.
  KO: `Scripts/setup_fk_spine_ik_legs.py`에 무파라미터 엔트리포인트를 추가하고, 간단한 러너 `Scripts/run_fk_spine_ik_legs.py`를 만들어 메뉴에서 바로 실행할 수 있게 했습니다.

### Option A: Execute the runner / 방법 A: 러너 실행
- EN: In UE: `Tools > Python > Execute Python Script...` → select `Scripts/run_fk_spine_ik_legs.py`.
  KO: 언리얼에서 `Tools > Python > Execute Python Script...` → `Scripts/run_fk_spine_ik_legs.py` 선택.

### Option B: Execute the setup file directly / 방법 B: 설정 스크립트 직접 실행
- EN: Same menu, select `Scripts/setup_fk_spine_ik_legs.py`; it runs `main()` using defaults (`CR_Man`, `/Game/Rigs`, `/Character/SK_Man`).
  KO: 동일 메뉴에서 `Scripts/setup_fk_spine_ik_legs.py`를 선택하면 기본값(`CR_Man`, `/Game/Rigs`, `/Character/SK_Man`)으로 `main()`이 실행됩니다.

### Option C: Auto-discovery (optional) / 방법 C: 자동 인식(선택)
- EN: Move/copy the files under `Content/Python/` and run from `Window > Developer Tools > Python Editor`, or make `Content/Python/init_unreal.py` import and call `rig.main()`.
  KO: 파일을 `Content/Python/` 아래로 옮기거나 복사해 `Window > Developer Tools > Python Editor`에서 실행하거나, `Content/Python/init_unreal.py`에서 `rig.main()`을 호출하도록 구성할 수 있습니다.

## Notes / 참고
- EN: Both scripts add the project root to `sys.path` so imports work when run directly.
  KO: 두 스크립트 모두 프로젝트 루트를 `sys.path`에 추가해 직접 실행 시 임포트가 정상 동작합니다.

## Files / 파일
- EN: Updated `Scripts/setup_fk_spine_ik_legs.py` and added `Scripts/run_fk_spine_ik_legs.py`.
  KO: `Scripts/setup_fk_spine_ik_legs.py`를 수정하고 `Scripts/run_fk_spine_ik_legs.py`를 추가했습니다.

