# Agent Specification / 에이전트 명세

## Overview / 개요
- EN: This repository uses a bilingual, request-logged workflow targeting Unreal Engine 5.5. Builds are executed by the user.
  KO: 이 저장소는 언리얼 엔진 5.5를 대상으로 하며, 모든 응답과 기록은 영어/한국어 병기이고 빌드는 사용자가 수행합니다.

## Response Style / 응답 스타일
- EN: Default tone is concise, direct, and friendly.
  KO: 기본 톤은 간결하고 직설적이며 친근합니다.
- EN: Console replies and files must be bilingual (EN/KO).
  KO: 콘솔과 파일의 모든 답변은 영어/한국어 병기입니다.
- EN: Use short preambles before tool calls summarizing next actions.
  KO: 툴 호출 전, 다음 작업을 1–2문장으로 간단히 예고합니다.
- EN: Structure answers with short headers and bullet points when it helps.
  KO: 필요 시 짧은 헤더와 불릿으로 구조화합니다.

## Request Logging / 요청 기록
- EN: For every user request, create `Requests/NNN-<english-summary>.md`.
  KO: 모든 사용자 요청은 `Requests/NNN-<english-summary>.md`로 기록합니다.
- EN: Numbering uses zero-padded 3 digits (`001`, `002`, ...).
  KO: 순번은 3자리 0 패딩(`001`, `002`, ...)을 사용합니다.
- EN: Each file contains the Request and the Response sections, both bilingual.
  KO: 각 파일은 요청과 응답 섹션을 포함하며 둘 다 병기합니다.

## Unreal 5.5 / 언리얼 5.5
- EN: Target Unreal Engine 5.5 in examples, scripts, and settings.
  KO: 예시, 스크립트, 설정은 언리얼 5.5 기준으로 작성합니다.
- EN: Do not run builds unless explicitly asked; user builds locally.
  KO: 명시 요청이 없는 한 빌드를 실행하지 않으며, 사용자가 직접 빌드합니다.

## Coding Guidelines / 코딩 가이드라인
- EN: Fix root causes; avoid unrelated changes; keep edits minimal and consistent with repo style.
  KO: 근본 원인 해결을 우선하며 불필요한 변경을 피하고 저장소 스타일을 따릅니다.
- EN: Update docs as needed; avoid license headers unless asked.
  KO: 필요한 문서는 갱신하되, 요청 없이는 라이선스 헤더를 추가하지 않습니다.
- EN: Use `apply_patch` to add/modify files; do not commit or branch unless asked.
  KO: 파일 추가/수정은 `apply_patch`를 사용하며, 요청 없이는 커밋/브랜치 생성하지 않습니다.
- EN: Prefer targeted formatting; test changes when possible without unrelated fixes.
  KO: 포맷팅은 범위를 좁게 적용하고, 가능한 범위에서 테스트하되 관련 없는 문제는 수정하지 않습니다.
- EN: Avoid one-letter variable names and inline citations; keep code readable.
  KO: 한 글자 변수명과 인라인 인용을 피하고 가독성을 유지합니다.

## Plans & Preambles / 계획과 예고
- EN: Use a short plan when tasks have multiple steps or ambiguity; keep exactly one in-progress step.
  KO: 다단계/모호한 작업에는 짧은 계획을 사용하며, 항상 하나의 진행 중 단계를 유지합니다.
- EN: Group related tool actions into one concise preamble.
  KO: 관련된 툴 작업은 하나의 간결한 예고로 묶습니다.

## Approvals & Sandbox / 승인과 샌드박스
- EN: Request escalated permissions only when necessary (e.g., writes outside workspace or network access) and explain why.
  KO: 작업 공간 밖 쓰기나 네트워크 접근 등 필요한 경우에만 권한 상승을 요청하고 이유를 명시합니다.

## Deliverables / 결과물
- EN: Provide paths to created/updated files and concise next steps.
  KO: 생성/수정된 파일 경로와 간단한 다음 단계를 제공합니다.

## Files Created by This Setup / 이번 설정으로 생성된 파일
- EN: `agent.md` (this file) and `Requests/001-setup-preferences.md`.
  KO: `agent.md`(본 파일)과 `Requests/001-setup-preferences.md`.
- EN: Future requests continue at `Requests/NNN-...`.
  KO: 이후 요청도 `Requests/NNN-...` 형식으로 계속 기록합니다.

