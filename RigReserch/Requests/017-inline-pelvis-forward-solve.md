# Inline Pelvis Forward Solve / 골반 Forward Solve 내장

## Request / 요청
- EN: Make the pelvis control in `pelvisonly.py` actually drive the pelvis bone (modify the same file; do not create a new one).
  KO: `pelvisonly.py`의 골반 컨트롤이 실제로 골반 본을 구동하도록 같은 파일을 수정해 주세요(새 파일 생성 금지).

## Response / 응답
- EN: Updated `Scripts/pelvisonly.py` to add the Forward Solve wiring inside the script: `GetControlTransform(Pelvis_CTRL) -> SetBoneTransform(pelvis)` with execution linked from Entry. Includes fallbacks for node/graph/pin names across UE 5.x.
  KO: `Scripts/pelvisonly.py`에 Forward Solve 연결을 내장하여 `GetControlTransform(Pelvis_CTRL) -> SetBoneTransform(pelvis)`와 엔트리 실행 연결을 추가했습니다. UE 5.x 간 노드/그래프/핀 이름 폴백을 포함합니다.

## Details / 상세
- EN: Added helpers to access the Forward Solve (Rig Graph) controller, add unit nodes by class or struct path, set pin default values, and create links.
  KO: Forward Solve(리그 그래프) 컨트롤러 접근, 클래스/구조체 경로 기반 노드 추가, 핀 기본값 설정, 링크 생성을 위한 헬퍼를 추가했습니다.
- EN: Wrapped edits in `ScopedEditorTransaction` and recompiled/saved via the existing `_safe_compile_and_save`.
  KO: 변경을 `ScopedEditorTransaction`으로 감싸고 기존 `_safe_compile_and_save`를 통해 리컴파일/저장합니다.

## How to Use / 사용 방법
- EN: Run `pelvisonly.py` as before. With Source Control enabled, check out the CR asset first so changes persist.
  KO: 기존과 동일하게 `pelvisonly.py`를 실행하세요. 소스 컨트롤 사용 시 변경 사항이 저장되도록 먼저 CR 자산을 체크아웃하세요.

