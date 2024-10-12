from PIL import Image
from pathlib import Path
import streamlit as st
import time
import datetime
import requests as rq
from problem import make_problem

# 定数の設定
IP = '127.000.000.001'
TOKEN = '"ariake90ac96700a7e0f8208caad8bbcc7a8efebf4703ec4a1b9560be92f14b4"'
SIZE = 1000

# 保存先ディレクトリの設定
dir = Path(__file__).parent

# 色の辞書
color_dict = {
    0: (255, 0, 0),      # red
    1: (255, 255, 0),    # yellow
    2: (0, 255, 0),      # green
    3: (0, 0, 255),      # blue
    4: (0, 255, 255),    # cyan
    5: (255, 0, 255),    # magenta
}

# 問題を取得する関数
def get_problem(ip: str, token: str) -> dict:
    url = f"http://{ip}/problem"
    headers = {
        'Content-Type': 'application/json',
        'Procan-Token': token
    }
    response = rq.get(url, headers=headers)
    response.raise_for_status()
    return response.json()

# 文字列のリストを整数のリストに変換する関数
def str_to_int(array: list[str]) -> list[list[int]]:
    return [[int(char) for char in string] for string in array]

# ボード画像を作成する関数
def make_fig(board: list[list[int]], size: int) -> Image:
    """二次元配列からボード画像を作成・保存し、画像オブジェクトを返す"""
    width = len(board[0])
    height = len(board)
    
    # 新しい画像を作成
    img = Image.new('RGB', (width, height))
    pixels = img.load()
    
    # ピクセルに色を設定
    for y in range(height):
        for x in range(width):
            pixels[x, y] = color_dict[board[y][x]]
    
    # 指定サイズに拡大
    img = img.resize((size, size), resample=Image.NEAREST)
    return img

# ダミーの問題を生成する（テスト用）
problem = make_problem(10, 10, 'diagonal', 42, False, False).to_json_dict()

# 時間の計測開始
program_start = time.time()

# ボードの幅と高さを取得
board_width = problem["board"]["width"]
board_height = problem["board"]["height"]

# 初期盤面と最終盤面の画像を生成
start = make_fig(str_to_int(problem["board"]["start"]), SIZE)
goal = make_fig(str_to_int(problem["board"]["goal"]), SIZE)

# 一般抜型のパターンサイズと画像を取得
n = problem["general"]["n"]
general_size = [(pattern["width"], pattern["height"]) for pattern in problem["general"]["patterns"]]
general_cells = [make_fig(str_to_int(pattern["cells"]), SIZE) for pattern in problem["general"]["patterns"]]

# 開始時間と試合時間、終了時間を取得
start_at = datetime.datetime.now()
match_time = datetime.timedelta(seconds=300)
stop_at = start_at + match_time

# Streamlitの設定
st.title("Board Image ")

# カラム分割
col1, col2 = st.columns(2)

# 初期盤面と最終盤面を表示
with col1:
    st.image(start, caption="初期盤面", use_column_width=True)

with col2:
    st.image(goal, caption="最終盤面", use_column_width=True)

st.markdown(f'''
# ボード情報
- 横: **{board_width}**
- 縦: **{board_height}**

# 時間
- 開始時間: **{start_at}**
- 終了時間: **{stop_at}**

## 手数: {n}
'''.format(board_width=board_width, board_height=board_height, start_at=start_at, stop_at=stop_at, general_size=general_size, n=n))



# 保存先ディレクトリの作成
output_dir = dir / "output"
output_dir.mkdir(exist_ok=True)

# 画像を保存
path = output_dir / "sample.png"
goal.save(path, format="PNG")


#st.write(f"画像が保存されました: {path}")
#t.write(f"処理時間: {time.time() - program_start:.2f} 秒") 実行時間の表示

# 一般抜型の画像を表示
#st.markdown("## 一般抜型の画像")
#for i, general_cell in enumerate(general_cells):
 #   width, height = general_size[i]
  #  st.image(general_cell, caption=f"手数 {i+1},横 {width} , 縦 {height}", use_column_width=True)



# 一般抜型の画像を表示（アコーディオン形式）

st.markdown("## 一般抜型の画像")
with st.expander("一般抜型の画像一覧を表示"):
    for i, general_cell in enumerate(general_cells):
        width, height = general_size[i]
        st.image(general_cell, caption=f"手数 {i+1}, 横 {width} , 縦 {height}", use_column_width=True)



# 一般抜型サイズの情報を文字列に整形して表示（アコーディオン形式）

with st.expander("一般抜型サイズの詳細を表示"):
    general_info = "\n".join([f"- 手数:{i+1},(横,縦) = ({size[0]}, {size[1]})" for i, size in enumerate(general_size)])
    st.markdown(f'''
    ## 一般抜型サイズ
    {general_info}
    ''')


# 一般抜型サイズの情報を文字列に整形して表示
#general_info = "\n".join([f"- 手数:{i+1}, (横,縦) = ({size[0]}, {size[1]})" for i,size in enumerate(general_size)])


