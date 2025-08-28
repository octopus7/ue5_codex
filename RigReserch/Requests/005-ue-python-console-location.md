# Where is the UE Python Console? / 언리얼 파이썬 콘솔 위치

## Request / 요청
- EN: Where is the Unreal Python Console?
  KO: 언리얼 파이썬 콘솔이 어디에 있어?

## Response / 응답
- EN: In UE 5.5, enable the Python plugins, then open the console via Output Log or the Python menu.
  KO: UE 5.5에서는 파이썬 플러그인을 활성화한 뒤, 출력 로그 또는 Python 메뉴로 콘솔을 열 수 있습니다.

### Enable Plugins / 플러그인 활성화
- EN: Go to `Edit > Plugins`, search "Python" and enable:
  KO: `Edit > Plugins`에서 "Python"을 검색해 다음을 활성화하세요:
- EN: • Python Editor Script Plugin (required)
  KO: • Python Editor Script Plugin (필수)
- EN: • Editor Scripting Utilities (recommended)
  KO: • Editor Scripting Utilities (권장)
- EN: Restart the editor when prompted.
  KO: 재시작 안내가 나오면 에디터를 재시작하세요.

### Open Console / 콘솔 열기
- EN: `Window > Developer Tools > Output Log`, then switch the input mode at the bottom from `Cmd` to `Python` and type Python code.
  KO: `Window > Developer Tools > Output Log`를 열고, 하단 입력창 모드를 `Cmd`에서 `Python`으로 바꾼 뒤 파이썬 코드를 입력합니다.
- EN: Or use `Tools > Python > Open Python Console` (visible after enabling the plugin).
  KO: 또는 `Tools > Python > Open Python Console`을 사용합니다(플러그인 활성화 후 표시됨).
- EN: For editing scripts, open `Window > Developer Tools > Python Editor` and press Run.
  KO: 스크립트 편집은 `Window > Developer Tools > Python Editor`에서 열어 실행(Run)하면 됩니다.

### Run Our Script / 우리 스크립트 실행
- EN: This repo's script is at `Scripts/create_control_rig.py`. Either:
  KO: 이 저장소의 스크립트는 `Scripts/create_control_rig.py`에 있습니다. 다음 중 한 방법을 사용하세요:
- EN: • Add project root to `sys.path` in the Python Console:
  KO: • 파이썬 콘솔에서 프로젝트 루트를 `sys.path`에 추가:

```
import sys, unreal
sys.path.append(unreal.Paths.project_dir())
import Scripts.create_control_rig as ccr
ccr.create_control_rig("CR_MyCharacter", "/Game/Rigs", "/Game/Characters/My/SK_My.SK_My")
```

- EN: • Or move/copy the script under `Content/Python/` (auto-discovered), then `import create_control_rig as ccr`.
  KO: • 또는 스크립트를 `Content/Python/` 아래로 옮기거나 복사하면(자동 인식), `import create_control_rig as ccr`로 불러올 수 있습니다.
- EN: • Or run via `Tools > Python > Execute Python Script...` and pick the file.
  KO: • 또는 `Tools > Python > Execute Python Script...`에서 파일을 직접 실행합니다.

## Notes / 참고
- EN: The console input mode selector appears as a dropdown or buttons near the input field; choose `Python`.
  KO: 콘솔 입력 모드 선택기는 입력창 근처 드롭다운/버튼으로 표시되며, `Python`을 선택하세요.
- EN: If `Tools > Python` is missing, the plugin is not enabled or the editor needs a restart.
  KO: `Tools > Python` 메뉴가 보이지 않으면 플러그인이 비활성 상태이거나 에디터 재시작이 필요합니다.

