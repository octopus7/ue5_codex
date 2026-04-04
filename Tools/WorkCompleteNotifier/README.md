# WorkCompleteNotifier

작업 완료 시 사용자에게 반드시 보이도록 화면 중앙 상단 계층으로 띄우는 .NET 10 WPF 알림 앱입니다.

- 중앙 고정 크기 TopMost 팝업으로 실행됩니다.
- 창 바깥 화면은 가리지 않고 그대로 보입니다.
- 중앙 카드에 완료 시각, 수행 시간, 작업 경로를 표시합니다.
- 아무 곳이나 클릭하면 바로 닫힙니다.

## 실행 예시

```powershell
dotnet run --project .\Tools\WorkCompleteNotifier -- `
  --title "CodexPlanet 작업 완료" `
  --completed-at "2026-04-04 18:35:00" `
  --duration "00:12:48" `
  --path "D:\github\ue5_codex\CodexPlanet"
```

`--duration` 대신 `--duration-seconds 768`도 지원합니다.

`--started-at`을 넘기면 `--completed-at`과의 차이로 수행 시간을 계산합니다.

## 빌드

```powershell
dotnet build .\Tools\WorkCompleteNotifier\WorkCompleteNotifier.csproj
```
