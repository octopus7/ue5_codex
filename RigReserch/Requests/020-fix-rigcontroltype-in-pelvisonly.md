# Fix RigControlType in pelvisonly.py / pelvisonly.py 컨트롤 타입 수정

## Request / 요청
- EN: Error: `AttributeError: type object 'RigControlType' has no attribute 'TRANSFORM'`. Update `pelvisonly.py` so it runs on UE 5.5 while keeping Forward Solve wiring and bone name `Pelvis`.
  KO: 오류: `AttributeError: type object 'RigControlType' has no attribute 'TRANSFORM'`. `pelvisonly.py`가 UE 5.5에서 동작하도록 수정하되, Forward Solve 연결과 본 이름 `Pelvis`는 유지해 주세요.

## Change / 변경 내용
- EN: Replaced the hard-coded `RigControlType.TRANSFORM` with a safe selector that picks the first available among `EULER_TRANSFORM`, `TRANSFORM`, `TRANSFORM_NO_SCALE`.
  KO: 고정된 `RigControlType.TRANSFORM` 대신 `EULER_TRANSFORM`, `TRANSFORM`, `TRANSFORM_NO_SCALE` 중 사용 가능한 항목을 우선 선택하도록 변경했습니다.

## Context / 배경
- EN: Different UE 5.x builds expose different `RigControlType` members; some omit `TRANSFORM` but include `EULER_TRANSFORM`.
  KO: UE 5.x 빌드마다 `RigControlType` 멤버가 달라 일부 환경에서는 `TRANSFORM`가 없고 `EULER_TRANSFORM`만 제공됩니다.

## Files / 파일
- EN: Updated `Scripts/pelvisonly.py` (kept `PELVIS_BONE = "Pelvis"` and Forward Solve wiring intact).
  KO: `Scripts/pelvisonly.py`를 업데이트했습니다(`PELVIS_BONE = "Pelvis"`와 Forward Solve 연결 유지).

