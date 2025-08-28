# Fix pelvisonly.py Transform Enum / pelvisonly.py 변형 타입 수정

## Request / 요청
- EN: Modify `pelvisonly.py` itself instead of creating a new file. Error: `AttributeError: type object 'RigControlType' has no attribute 'TRANSFORM'`.
  KO: 새 파일을 만들지 말고 `pelvisonly.py` 자체를 수정해 주세요. 오류: `AttributeError: type object 'RigControlType' has no attribute 'TRANSFORM'`.

## Change / 변경 내용
- EN: Replaced the hard-coded `RigControlType.TRANSFORM` with a robust selector that chooses the first available transform-like enum in this order: `EULER_TRANSFORM`, `TRANSFORM`, `TRANSFORM_NO_SCALE`. Raises a clear error if none exist.
  KO: 고정된 `RigControlType.TRANSFORM` 참조를 제거하고, `EULER_TRANSFORM` → `TRANSFORM` → `TRANSFORM_NO_SCALE` 순으로 사용 가능한 변형 계열 enum을 선택하도록 변경했습니다. 존재하지 않으면 명확한 오류를 발생시킵니다.

## Reason / 이유
- EN: UE 5.x variants differ in available `RigControlType` members; some builds don’t expose `TRANSFORM` but do expose `EULER_TRANSFORM`.
  KO: UE 5.x 빌드마다 `RigControlType` 멤버 구성이 달라 일부 환경에서는 `TRANSFORM`가 없고 `EULER_TRANSFORM`만 제공됩니다.

## File / 파일
- EN: Updated `Scripts/pelvisonly.py`.
  KO: `Scripts/pelvisonly.py`를 수정했습니다.

## Note / 참고
- EN: This script still doesn’t add Forward Solve nodes (by design). If you want the pelvis control to drive the pelvis bone, run `Scripts/pelvis_forward_solve.py` or ask me to inline that logic into `pelvisonly.py`.
  KO: 본 스크립트는 설계상 Forward Solve 노드를 추가하지 않습니다. 골반 컨트롤이 본을 구동하도록 하려면 `Scripts/pelvis_forward_solve.py`를 실행하거나 해당 로직을 `pelvisonly.py`에 통합하도록 요청해 주세요.

