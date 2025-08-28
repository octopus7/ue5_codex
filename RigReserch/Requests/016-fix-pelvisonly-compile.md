# Fix pelvisonly.py Compile Fallback / pelvisonly.py 컴파일 폴백 수정

## Request / 요청
- EN: Error: `AttributeError: module 'unreal' has no attribute 'KismetEditorUtilities'` when running `pelvisonly.py`. Modify the existing file; do not create a new one.
  KO: `pelvisonly.py` 실행 시 `AttributeError: module 'unreal' has no attribute 'KismetEditorUtilities'` 오류가 납니다. 새 파일이 아닌 기존 파일을 수정해 주세요.

## Change / 변경 내용
- EN: Replaced the direct `KismetEditorUtilities.compile_blueprint` call with a robust `_safe_compile_and_save(crb)` function that:
  KO: 직접 호출하던 `KismetEditorUtilities.compile_blueprint`를 제거하고, 다음을 시도하는 견고한 `_safe_compile_and_save(crb)` 함수로 교체했습니다:
- EN: • Requests Control Rig init via `ControlRigBlueprintLibrary.request_control_rig_init`.
  KO: • `ControlRigBlueprintLibrary.request_control_rig_init`로 CR 초기화를 요청.
- EN: • Tries `KismetEditorUtilities.compile_blueprint` if available.
  KO: • 사용 가능하면 `KismetEditorUtilities.compile_blueprint` 시도.
- EN: • Tries `BlueprintEditorLibrary.compile_blueprint` if available.
  KO: • 사용 가능하면 `BlueprintEditorLibrary.compile_blueprint` 시도.
- EN: • Falls back to `crb.recompile_vm()` on the Control Rig Blueprint.
  KO: • 컨트롤 릭 블루프린트의 `crb.recompile_vm()`로 폴백.
- EN: • Saves via `EditorAssetLibrary.save_loaded_asset` regardless, so changes persist.
  KO: • `EditorAssetLibrary.save_loaded_asset`로 저장을 보장.

## Reason / 이유
- EN: Some UE 5.5 environments do not expose `KismetEditorUtilities` to Python; the fallback path ensures compilation or safe saving across variants.
  KO: 일부 UE 5.5 환경은 파이썬에서 `KismetEditorUtilities`를 노출하지 않습니다. 폴백 경로로 다양한 환경에서 컴파일 또는 안전 저장을 보장합니다.

## File / 파일
- EN: Updated `Scripts/pelvisonly.py`.
  KO: `Scripts/pelvisonly.py`를 수정했습니다.

## Note / 참고
- EN: With Source Control enabled, remember to Check Out the asset so hierarchy/control edits persist.
  KO: 소스 컨트롤 사용 시 자산을 체크아웃해야 하이어라키/컨트롤 편집이 저장됩니다.

