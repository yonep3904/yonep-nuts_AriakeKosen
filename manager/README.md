# manager

## フォルダ/ファイル構成

- `image_data`: `visualizer.py`実行時、取得した問題の盤面状態の画像(.png)が保存される
- `log_data`: `answer_manager.py`実行時、通信のログが保存される
- `problem_data`: `visualizer.py`実行時、取得した問題の情報(.json)が保存される
- `answer_manager`: 解答管理システム
- `visualizer`: ビジュアライザ

## 使用方法

`manager` ディレクトリに移動し、Python でビジュアライザや解答管理システムを実行します。

```bash
python -m streamlit run answer_manager.py
python visualize.py
```
