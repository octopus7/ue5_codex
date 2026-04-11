# Monitor Sample

정적 웹페이지로 다중 플랜 배치 모니터를 확인하는 샘플입니다.

## 실행
1. VS Code `Live Server`로 [index.html](./index.html)을 엽니다.
2. 기본 데이터는 `./data/monitor-data.current.json`을 읽습니다.
3. 다른 JSON을 붙이려면 쿼리스트링으로 경로를 넘깁니다.

```text
http://127.0.0.1:5500/Docs/Monitor/index.html?data=./data/monitor-data.current.json
```

## 구조
- `index.html`: 모니터 대시보드
- `styles.css`: 정적 스타일
- `app.js`: JSON 로드, 폴링, 경과 시간 계산
- `data/monitor-data.current.json`: 모니터링 전용 작업본
- `data/monitor-data.sample.json`: 초기화/예시용 샘플 데이터

## JSON 필드
- `meta`: 타이틀, 설명, 시간대, 생성 시각, 폴링 주기
- `board`: Track/Phase 상태판
- `locks`: 공용 리소스 점유 현황
- `timeline`: 시계열 이벤트 로그

## 동작 방식
- 페이지는 JSON 파일을 `fetch()`로 읽습니다.
- 기본은 `5초` 주기 폴링입니다.
- `진행중/대기/차단/검증중` 경과 시간은 저장된 타임스탬프를 기준으로 브라우저가 매초 갱신합니다.
- 정적 페이지이므로 브라우저가 데이터를 직접 수정하지는 않습니다. 다른 스크립트나 작업자가 JSON을 갱신하면 화면이 따라옵니다.
