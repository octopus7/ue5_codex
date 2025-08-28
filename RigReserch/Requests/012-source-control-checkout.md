# Source Control Checkout / 소스 컨트롤 체크아웃

## Request / 요청
- EN: Error shows "Unable to Check Out From Revision Control", and node_count remains 0. How to proceed?
  KO: "리비전 컨트롤에서 체크아웃 불가" 오류가 발생하고 node_count가 0입니다. 어떻게 진행하나요?

## Response / 응답
- EN: Source control is blocking writes, so graph/hierarchy changes cannot be saved. Either check out the asset or temporarily disable source control, then run the builder again.
  KO: 소스 컨트롤이 쓰기를 막아 그래프/하이어라키 변경이 저장되지 않습니다. 자산을 체크아웃하거나 일시적으로 소스 컨트롤을 비활성화한 뒤 빌더를 다시 실행하세요.

### Option A: Checkout via script / 방법 A: 스크립트로 체크아웃
- EN: Run `Scripts/scc_checkout_leg_ik.py` to connect and check out `/Game/Rigs/leg_ik` (falls back to disk path) and then run `Scripts/leg_ik_build_forward.py`.
  KO: `Scripts/scc_checkout_leg_ik.py`를 실행해 `/Game/Rigs/leg_ik`을 체크아웃(디스크 경로 폴백 포함)한 뒤 `Scripts/leg_ik_build_forward.py`를 실행하세요.

### Option B: Checkout in UI / 방법 B: 에디터 UI로 체크아웃
- EN: In Content Browser, right-click `/Game/Rigs/leg_ik` → `Source Control > Check Out`. If missing, let the builder create it, then right-click the new asset and `Mark for Add`.
  KO: 콘텐츠 브라우저에서 `/Game/Rigs/leg_ik` 우클릭 → `Source Control > Check Out`. 자산이 없다면 빌더로 생성 후 우클릭하여 `Mark for Add`를 선택하세요.

### Option C: Temporarily disable / 방법 C: 일시 비활성화
- EN: Toolbar `Source Control` → `Change Settings` → Provider: `None`. Run the builder, verify results, then re-enable and `Mark for Add`.
  KO: 툴바 `Source Control` → `Change Settings` → Provider: `None`로 설정. 빌더 실행 후 결과를 확인하고, 다시 활성화하여 `Mark for Add` 하세요.

## Why it happened / 원인
- EN: With source control enabled, `.uasset` files are read-only until checked out; Python graph/hierarchy edits fail to save and appear as no-op.
  KO: 소스 컨트롤이 켜져 있으면 `.uasset` 파일이 체크아웃 전까지 읽기 전용이므로, 파이썬을 통한 그래프/하이어라키 수정이 저장되지 않아 무효처럼 보입니다.

