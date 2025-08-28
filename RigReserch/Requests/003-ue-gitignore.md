# Create Unreal .gitignore / 언리얼 .gitignore 생성

## Request / 요청
- EN: Generate a `.gitignore` suitable for Unreal Engine (UE 5.5).
  KO: 언리얼 엔진(UE 5.5)에 적합한 `.gitignore`를 생성해 주세요.

## Response / 응답
- EN: Added a UE5.5-optimized `.gitignore` at repo root, targeting common build artifacts and IDE clutter while keeping source, content, and config files tracked.
  KO: 일반적인 빌드 산출물과 IDE 관련 파일을 제외하고 소스/콘텐츠/설정 파일은 추적되도록, UE5.5 최적화 `.gitignore`를 저장소 루트에 추가했습니다.

### Highlights / 주요 항목
- EN: Ignore `Binaries/`, `DerivedDataCache/`, `Intermediate/`, `Saved/` recursively across project and plugins.
  KO: 프로젝트 및 플러그인 전역에서 `Binaries/`, `DerivedDataCache/`, `Intermediate/`, `Saved/`를 재귀적으로 무시합니다.
- EN: Ignore IDE folders: `.vs/`, `.idea/`, `.vscode/` and common user files.
  KO: IDE 폴더인 `.vs/`, `.idea/`, `.vscode/` 및 사용자별 파일을 무시합니다.
- EN: Optionally ignore `Build/` everywhere; remove this rule if you keep packaging resources under `Build/`.
  KO: 선택적으로 전역 `Build/`를 무시합니다. `Build/` 아래에 패키징 리소스를 보관한다면 이 규칙을 제거하세요.
- EN: Ignore macOS/Windows OS clutter and common logs.
  KO: macOS/Windows 생성 파일과 일반 로그 파일을 무시합니다.

## Files / 파일
- EN: `.gitignore` created at repository root.
  KO: 저장소 루트에 `.gitignore`를 생성했습니다.

## Next Steps / 다음 단계
- EN: If you need to keep specific `Build/` resources, I can whitelist them.
  KO: `Build/` 내 특정 리소스를 보관해야 한다면 화이트리스트로 조정해 드리겠습니다.

