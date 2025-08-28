# Fix Transform Enum and Compile / 변형 타입·컴파일 폴백 수정

## Request / 요청
- EN: `pelvisonly.py` throws `AttributeError: type object 'RigControlType' has no attribute 'TRANSFORM'`. Make it work on UE 5.5.
  KO: `pelvisonly.py` 실행 시 `AttributeError: type object 'RigControlType' has no attribute 'TRANSFORM'` 오류가 납니다. UE 5.5에서 동작하도록 수정해 주세요.

## Response / 응답
- EN: Updated `Scripts/pelvisonly.py` to:
  KO: `Scripts/pelvisonly.py`를 다음과 같이 수정했습니다:
- EN: • Pick the first available transform-like control type among `EULER_TRANSFORM`, `TRANSFORM`, `TRANSFORM_NO_SCALE`.
  KO: • `EULER_TRANSFORM`, `TRANSFORM`, `TRANSFORM_NO_SCALE` 중 사용 가능한 변형 계열 타입을 선택.
- EN: • Compile via a safe fallback that tries `KismetEditorUtilities`, then `BlueprintEditorLibrary`, then `recompile_vm()`, and always saves.
  KO: • `KismetEditorUtilities` → `BlueprintEditorLibrary` → `recompile_vm()` 순으로 컴파일을 시도하고 항상 저장.
- EN: • Keep robust SkeletalMesh loading and Skeleton checks.
  KO: • SkeletalMesh 로딩 및 Skeleton 검사를 유지.

## Notes / 참고
- EN: If Source Control is enabled, check out the asset so hierarchy/graph edits persist.
  KO: 소스 컨트롤 사용 시 자산을 체크아웃해야 하이어라키/그래프 변경이 저장됩니다.

## File / 파일
- EN: Updated `Scripts/pelvisonly.py`.
  KO: `Scripts/pelvisonly.py`를 업데이트했습니다.

