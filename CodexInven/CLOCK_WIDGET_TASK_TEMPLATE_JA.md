# Clock Widget Task Template

別の会話で同じ種類の非 MVVM 時計ウィジェット実装をもう一度依頼したいときに使うテンプレートです。

## 変数

- `<EnginePath>`: Unreal Engine のインストールパス
- `<ProjectRoot>`: プロジェクトのルートパス
- `<ProjectName>`: プロジェクト名
  - 例: `CodexInven`
- `<ProjectEditorModuleName>`: そのプロジェクトのエディターモジュール名
  - 通常は `<ProjectName>Editor`
  - 例: `CodexInvenEditor`
- `<ProjectUprojectPath>`: `.uproject` のフルパス
- `<CommandletName>`: 一回限りのアセット生成用 commandlet 名

## 依頼テンプレート

```text
これは Unreal Engine 5.7.4 のプロジェクトです。エンジンパスは <EnginePath> で、ビルドと実行は必ずそのパスのエンジンを使ってください。
まず AGENTS.md、<ProjectName>.uproject、Source/*.Target.cs を確認して、エンジンバージョンを再確認してください。

この文書では:
- <ProjectName> はプロジェクト名です。例: CodexInven
- <ProjectEditorModuleName> はそのプロジェクトのエディターモジュール名です。通常は <ProjectName>Editor です。例: CodexInvenEditor

既存ウィジェットの移行はしないでください。通常方式の時計ウィジェットを完全に新規で実装してください。

要件:
- UUserWidget を継承したランタイム C++ ウィジェットクラスを 1 つ作成すること
- ウィジェットツリーやレイアウト生成コードをランタイムクラスに入れないこと
- ランタイムクラスは必要なウィジェットだけを BindWidget で受け取り、時刻テキストと時針/分針/秒針の角度だけを更新すること
- デジタル時計とアナログ針時計を同時に表示すること
- アナログ時計は円形の文字盤にすること
- 目盛りは 12 個にし、3 時/6 時/9 時/12 時方向は少し強く見せること
- 画面上部中央に配置すること
- 矩形コンテナ背景は暖色系にすること
- C++ 親クラスを継承した新規 WBP を作成すること
- WBP のウィジェットツリーは editor-only の一回限りコードで生成すること
- 既存 WBP のツリー構造を読む共通ユーティリティは permanent/common コードとして分離すること
- WBP 作成/更新コードは transient フォルダに分離し、後で削除できるようにすること
- 円形文字盤を直接組みにくい場合は、テクスチャを生成して Image に使ってもよい
- プレイヤーコントローラーからその WBP をロードしてビューポートに追加すること
- Python Remote Execution、外部 Python スクリプト、DefaultEngine.ini の変更は行わないこと

一時コードの実行方法:
- 一回限りの editor-only コードは commandlet または editor-only scaffold として実装すること
- commandlet を使う場合は、実行に使った実際のコマンドラインも残すこと
- 実行形式:
  "<EnginePath>\\Engine\\Binaries\\Win64\\UnrealEditor-Cmd.exe" "<ProjectUprojectPath>" -run=<CommandletName> -unattended -nop4 -nosplash -NoSound
- 例:
  "<EnginePath>\\Engine\\Binaries\\Win64\\UnrealEditor-Cmd.exe" "<ProjectUprojectPath>" -run=<ProjectName>ClockWidgetScaffold -unattended -nop4 -nosplash -NoSound

成果物:
- ランタイムウィジェットクラス
- プレイヤーコントローラー連携
- 共通 WBP ツリー参照ユーティリティ
- 一回限りの WBP/テクスチャ生成 commandlet または editor scaffold
- /Game/UI/WBP_CodexClock
- 必要なら /Game/UI/T_ClockFace_Analog

検証:
- <ProjectEditorModuleName> をビルドすること
- 上記 commandlet を実行して WBP を生成すること
- 0 error(s), 0 warning(s) を確認すること
- ビルド後にエディタが起動していなければプロジェクトを開くこと
```

## 現在のプロジェクト例

- `<EnginePath>`: `E:\WorkTemp\Epic Games\UE_5.7`
- `<ProjectRoot>`: `D:\github\ue5_codex\CodexInven`
- `<ProjectName>`: `CodexInven`
- `<ProjectEditorModuleName>`: `CodexInvenEditor`
- `<ProjectUprojectPath>`: `D:\github\ue5_codex\CodexInven\CodexInven.uproject`
- `<CommandletName>`: `CodexInvenClockWidgetScaffold`

実行例:

```powershell
& "E:\WorkTemp\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
  "D:\github\ue5_codex\CodexInven\CodexInven.uproject" `
  -run=CodexInvenClockWidgetScaffold `
  -unattended -nop4 -nosplash -NoSound
```

## 参照位置

- ランタイムウィジェット: `Source/CodexInven/CodexInvenClockWidget.h`, `Source/CodexInven/CodexInvenClockWidget.cpp`
- プレイヤーコントローラー連携: `Source/CodexInven/CodexInvenTopDownPlayerController.h`, `Source/CodexInven/CodexInvenTopDownPlayerController.cpp`
- 共通ツリーユーティリティ: `Source/CodexInvenEditor/Public/WidgetBlueprint/CodexInvenWidgetBlueprintTreeUtils.h`, `Source/CodexInvenEditor/Private/WidgetBlueprint/CodexInvenWidgetBlueprintTreeUtils.cpp`
- 一時スキャフォールド: `Source/CodexInvenEditor/Private/Transient/ClockWidgetScaffolding/CodexInvenClockWidgetScaffoldCommandlet.h`, `Source/CodexInvenEditor/Private/Transient/ClockWidgetScaffolding/CodexInvenClockWidgetScaffoldCommandlet.cpp`
