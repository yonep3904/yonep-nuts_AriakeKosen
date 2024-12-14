# yonep-nuts\_AriakeKosen

## 概要

このリポジトリには第35回高専プロコン競技部門で使用したプログラムが含まれています。
大会では有明高専のチーム「よねぴーなっつ」として出場しました。

## リポジトリ構成

リポジトリ直下には以下のディレクトリおよびファイルが存在します。
`solver` は問題を解決するソルバー部分、`manager` はその解答の管理および可視化します。
`server_test` は練習環境として機能します。

### ディレクトリ

- `solver`: 問題を解くプログラムが含まれる
- `manager`: 解答管理システムやビジュアライザが含まれる
- `server_test`: 練習用の簡易サーバーを立ち上げるためのプログラムが含まれる
### ファイル
- `config.yml`: 設定ファイル
- `set_config.py`: `config.yml` をもとに、各ソースファイルや設定に変更を加えるスクリプト
- `check.bat`, `get_ip.py`: 本番時のネットワーク接続確認用スクリプト

## 使用方法

以下は基本的な使用手順です。

### 1. 設定の確認と変更

リポジトリ内の `config.yml` を編集して設定を行います。
その後、`set_config.py` を実行して必要な変更を反映させてください。

```bash
python set_config.py
```

### 2. Solver のコンパイル

`solver` ディレクトリ内のプログラムを C++17 でコンパイルしてください。
コンパイルにはMakefileを使用します:

```bash
make
```

コンパイル後は、生成された `yonep-nuts.exe` 実行ファイルを以下のように実行できます:

```bash
./yonep-nuts.exe
```

### 3. Manager の起動

`manager` ディレクトリに移動し、Python でビジュアライザや解答管理システムを実行します。

```bash
python -m streamlit run answer_manager.py
python visualize.py
```

### 4. サーバーの起動 (練習用)

`server_test` ディレクトリに移動し、簡易サーバーを起動します。

```bash
# (W * H) = (32 * 64) でランダムにボードを生成(シード値は42) / チーム数(トークン数)は3 / 試合時間は300
python launch_server.py 32 64 random 42 3 300
```

## 開発環境および動作環境

- C++17 (Solver 用)
- Python 3.12 (Manager および Server Test 用)
- Windows 環境を想定して動作確認を行っています。

