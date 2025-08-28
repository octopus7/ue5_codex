# Wire Pelvis Forward Solve / 골반 Forward Solve 연결

## Request / 요청
- EN: The pelvis bone name is `Pelvis`. Modify `pelvisonly.py` so the pelvis control actually drives the bone in Forward Solve.
  KO: 골반 본 이름은 `Pelvis`입니다. `pelvisonly.py`에서 골반 컨트롤이 Forward Solve에서 본을 실제로 구동하도록 수정해 주세요.

## Response / 응답
- EN: Updated `Scripts/pelvisonly.py` to set `PELVIS_BONE = "Pelvis"` and to add Forward Solve wiring: `GetControlTransform(Pelvis_CTRL) -> SetBoneTransform(Pelvis)` with execution linked from the Entry node. Includes fallbacks for controller/struct/pin names.
  KO: `Scripts/pelvisonly.py`에 `PELVIS_BONE = "Pelvis"`로 변경하고, Forward Solve에 `GetControlTransform(Pelvis_CTRL) -> SetBoneTransform(Pelvis)`를 추가했습니다. 엔트리에서 실행 핀을 연결하며, 컨트롤러/구조체/핀 이름 폴백을 포함합니다.

## How to Use / 사용 방법
- EN: Run `pelvisonly.py` as usual. If Source Control is enabled, check out the CR asset first so graph edits persist.
  KO: 기존처럼 `pelvisonly.py`를 실행하세요. 소스 컨트롤 사용 시 그래프 편집이 저장되도록 먼저 CR 자산을 체크아웃하세요.

## Note / 참고
- EN: If nodes still don’t appear, share the Output Log; I’ll align exact graph/controller/pin names to your UE 5.5 build.
  KO: 그래프에 노드가 보이지 않으면 출력 로그를 공유해 주세요. 사용 중인 UE 5.5 빌드에 맞춰 이름을 조정하겠습니다.

