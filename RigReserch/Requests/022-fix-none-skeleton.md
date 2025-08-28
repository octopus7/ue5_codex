# Fix NoneType Skeleton in pelvisonly.py / pelvisonly.py 스켈레톤 NoneType 수정

## Request / 요청
- EN: Error: `AttributeError: 'NoneType' object has no attribute 'skeleton'` when running `pelvisonly.py`.
  KO: `pelvisonly.py` 실행 시 `AttributeError: 'NoneType' object has no attribute 'skeleton'` 오류가 발생합니다.

## Cause / 원인
- EN: `unreal.load_asset(SKELETAL_MESH)` returned `None` because it was given a package path without the object name. Accessing `.skeleton` on `None` raised the error.
  KO: 패키지 경로만 전달되어 `unreal.load_asset(SKELETAL_MESH)`가 `None`을 반환했고, 그 상태에서 `.skeleton` 접근으로 오류가 발생했습니다.

## Fix / 수정
- EN: Updated `Scripts/pelvisonly.py` to use a robust loader that tries both package and object paths, e.g. `/Game/Mannequin/Character/Mesh/SK_Mannequin` and `/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin`. Also added clear errors if the Skeletal Mesh or its Skeleton is missing.
  KO: `Scripts/pelvisonly.py`에 패키지·오브젝트 경로 모두 시도하는 로더를 추가했습니다. 예: `/Game/Mannequin/Character/Mesh/SK_Mannequin`와 `/Game/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin` 모두 시도합니다. Skeletal Mesh나 Skeleton이 없을 경우 명확한 오류를 출력합니다.

## What To Do / 사용 방법
- EN: Run `pelvisonly.py` again. If further errors occur (e.g., compile utility missing), tell me and I will reapply the safe compile path.
  KO: `pelvisonly.py`를 다시 실행해 보세요. 추가 오류(예: 컴파일 유틸리티 누락)가 발생하면 알려주시면 안전한 컴파일 경로를 재적용하겠습니다.

