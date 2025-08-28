# Create Control Rig via Script / 스크립트로 컨트롤 릭 생성

## Request / 요청
- EN: Can you create a Control Rig using a script?
  KO: 스크립트를 사용해서 컨트롤 릭을 생성해줄 수 있어?

## Response / 응답
- EN: Added a UE 5.5 Python script `Scripts/create_control_rig.py` that creates a Control Rig Blueprint and optionally assigns a preview Skeletal Mesh. It handles multiple Control Rig factory variants for robustness.
  KO: 컨트롤 릭 블루프린트를 생성하고 선택적으로 프리뷰 스켈레탈 메시를 지정하는 UE 5.5 파이썬 스크립트 `Scripts/create_control_rig.py`를 추가했습니다. 여러 버전의 Control Rig 팩토리를 지원하도록 견고하게 작성했습니다.

### How to Use / 사용 방법
- EN: In Unreal Editor Python Console:
  KO: 언리얼 에디터 파이썬 콘솔에서:

```
import Scripts.create_control_rig as ccr
ccr.create_control_rig(
    rig_name="CR_MyCharacter",
    package_path="/Game/Rigs",
    skeletal_mesh_path="/Game/Characters/My/SK_My.SK_My"  # optional / 선택 사항
)
```

- EN: Ensure the Control Rig plugin is enabled; restart the editor after enabling.
  KO: Control Rig 플러그인이 활성화되어 있어야 하며, 활성화 후 에디터를 재시작하세요.

### What It Does / 동작 내용
- EN: Creates a Control Rig Blueprint asset under `package_path` with name `rig_name`.
  KO: `package_path` 경로에 `rig_name` 이름의 컨트롤 릭 블루프린트를 생성합니다.
- EN: If `skeletal_mesh_path` is provided, sets it as the preview mesh.
  KO: `skeletal_mesh_path`가 제공되면 프리뷰 메시로 설정합니다.
- EN: Uses `ControlRigBlueprintFactory` (or `ControlRigBlueprintFactoryNew`) via Asset Tools.
  KO: Asset Tools를 통해 `ControlRigBlueprintFactory`(또는 `ControlRigBlueprintFactoryNew`)를 사용합니다.

### Notes / 참고 사항
- EN: This script creates the asset but does not auto-build a full rig graph. If you want auto-generated controls (e.g., basic FK/IK), tell me the desired setup and bone naming.
  KO: 본 스크립트는 자산만 생성하며 전체 리그 그래프를 자동 구성하진 않습니다. 기본 FK/IK 등 자동 컨트롤 구성이 필요하면 원하는 구성과 본 네이밍 규칙을 알려주세요.
- EN: If `Build/` resources must be tracked, we can whitelist them in `.gitignore`.
  KO: `Build/` 내 특정 리소스를 추적해야 한다면 `.gitignore`에서 화이트리스트로 조정할 수 있습니다.

## Files / 파일
- EN: Added `Scripts/create_control_rig.py`.
  KO: `Scripts/create_control_rig.py`를 추가했습니다.

## Next Steps / 다음 단계
- EN: Provide the Skeletal Mesh asset path and target folder, or specify an auto-rigging template (FK/IK requirements), and I’ll extend the script.
  KO: 스켈레탈 메시 자산 경로와 생성 대상 폴더를 알려주시거나, 자동 리깅 템플릿(FK/IK 요구사항)을 지정해 주시면 스크립트를 확장하겠습니다.

