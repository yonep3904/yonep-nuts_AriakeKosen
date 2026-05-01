# image_to_problem

## 概要

画像データ(`.png`, `.jpeg`/`.jpg`, `.webp`...)をもとに問題データ(`.json`)や盤面の画像データを作成します。

## フォルダ/ファイル構成

- `data`: `make_problem.py`実行時、作成したデータの保存先となる。
- `image`: 作成したいデータの元となる画像データを保存する。
- `clear.py`: `data`ディレクトリ上にあるデータを削除するスクリプト
- `main.cpp`: `make_gif.exe`のメイン関数
- `make_gif.exe`: 解答(gif画像)を作成する
- `make_problem`: 画像データをもとに問題データや盤面の画像データを作成する。
- `replace.py`: `data/xxxxx/visualizer.py`内の`use_column_width`と`use_container_width`を入れ替える(streamlitのバージョンの互換性によって実行ができない場合に使用)

## 使用方法

`image`ディレクトリに作成したいデータの元となる画像データを保存した後、以下のコマンドを実行します:
```bash
python make_problem.py
```

環境によっては警告(主に「環境変数からコア数が取得できない」など)が出ますが、問題ないことが多いです。

デフォルトでは、32*32, 64*64, 128*128の盤面の問題を作成します。
他のサイズで作成したい場合、`make_problem.py`内の変数`sizes`を直接更してください。
また、ピースの種類を変更する場合、`make_problem.py`内の変数`colors_n`を直接更してください。

`python make_problem.py`を実行後、dataディレクトリ直下に**生成元の画像ファイル名_W_H**の名前でディレクトリが作成され、その中に以下のようなファイルが作成されます。

- `goal.png`: 最終盤面の画像
- `problem.json`: 問題データ
- `source.xxx`: 元の画像
- `solution_gif`: ピースをそろえる様子を示したgif画像
- `start.png`: 初期盤面の画像
- `visualizer.py`: ビジュアライザ起動のためのスクリプト

