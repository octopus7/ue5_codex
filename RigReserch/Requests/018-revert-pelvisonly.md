# Revert pelvisonly.py / pelvisonly.py 되돌리기

## Request / 요청
- EN: It doesn’t work. Revert the changes to the original state for `pelvisonly.py`; I will re-check.
  KO: 동작하지 않습니다. `pelvisonly.py`를 원래 상태로 되돌려 주세요. 다시 검토하겠습니다.

## Response / 응답
- EN: Reverted only our edits to `Scripts/pelvisonly.py` while keeping your current config values (mesh, folder, rig name) intact.
  KO: `Scripts/pelvisonly.py`에서 우리가 추가/수정했던 부분만 되돌리고, 사용자의 현재 설정값(메시, 폴더, 리그 이름)은 그대로 유지했습니다.

### Reverted Items / 되돌린 항목
- EN: Control type selection back to `RigControlType.TRANSFORM` (removed dynamic selection code).
  KO: 컨트롤 타입을 `RigControlType.TRANSFORM`로 원복(동적 선택 코드 제거).
- EN: Removed inline Forward Solve wiring (GetControlTransform → SetBoneTransform).
  KO: 인라인 Forward Solve 연결(컨트롤→본) 제거.
- EN: Restored original compile path: `ControlRigBlueprintLibrary.request_control_rig_init` + `KismetEditorUtilities.compile_blueprint` + save.
  KO: 원래 컴파일 경로로 복구: `ControlRigBlueprintLibrary.request_control_rig_init` + `KismetEditorUtilities.compile_blueprint` + 저장.

## Note / 참고
- EN: Since Source Control and environment can block `KismetEditorUtilities`, you may see the prior error again; this revert matches the original script semantics as requested.
  KO: 소스 컨트롤/환경에 따라 `KismetEditorUtilities`가 차단될 수 있어 이전 오류가 다시 나타날 수 있습니다. 요청대로 원래 스크립트 의미를 유지한 복구입니다.

