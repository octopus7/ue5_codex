# Fix Missing KismetEditorUtilities / KismetEditorUtilities 미노출 수정

## Request / 요청
- EN: Error: `AttributeError: module 'unreal' has no attribute 'KismetEditorUtilities'` when running `pelvisonly.py`. Keep Forward Solve and `Pelvis` bone; make compilation robust.
  KO: `pelvisonly.py` 실행 시 `AttributeError: module 'unreal' has no attribute 'KismetEditorUtilities'` 오류가 발생합니다. Forward Solve와 `Pelvis` 본은 유지하고, 컴파일을 견고하게 만들어 주세요.

## Response / 응답
- EN: Updated `Scripts/pelvisonly.py` to use a safe `_safe_compile_and_save(crb)` function that:
  KO: `Scripts/pelvisonly.py`에 안전한 `_safe_compile_and_save(crb)` 함수를 추가하여 다음을 수행합니다:
- EN: • Requests Control Rig init via `ControlRigBlueprintLibrary.request_control_rig_init`.
  KO: • `ControlRigBlueprintLibrary.request_control_rig_init`로 초기화 요청.
- EN: • Tries `KismetEditorUtilities.compile_blueprint` if available.
  KO: • 가능하면 `KismetEditorUtilities.compile_blueprint` 시도.
- EN: • Tries `BlueprintEditorLibrary.compile_blueprint` if available.
  KO: • 가능하면 `BlueprintEditorLibrary.compile_blueprint` 시도.
- EN: • Falls back to `crb.recompile_vm()` and then saves the asset.
  KO: • `crb.recompile_vm()`로 폴백 후 자산 저장.

## Notes / 참고
- EN: Source Control can still block saving if the asset isn’t checked out; ensure the CR asset is checked out before running.
  KO: 자산이 체크아웃되지 않으면 소스 컨트롤이 저장을 막을 수 있으니, 실행 전에 CR 자산을 체크아웃하세요.

## Files / 파일
- EN: Updated `Scripts/pelvisonly.py`.
  KO: `Scripts/pelvisonly.py`를 업데이트했습니다.

