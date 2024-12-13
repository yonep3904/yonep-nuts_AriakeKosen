import time
import datetime
import json
import yaml
import requests as rq
import streamlit as st
from pathlib import Path
from PIL import Image
from problem import make_problem

#############################################################
# 設定ファイル
config_file = 'config.yml'
# 盤面画像記録ファイル
image_dir = 'image_data'
# 問題情報記録ファイル
problem_dir = 'problem_data'
# 画像サイズ
image_size = 1000

#############################################################

# パスを取得
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
def get_problem(url: str, token: str) -> dict:
    header = {
        'Content-Type': 'application/json',
        'Procon-Token': token,
    }
    while True:
        response = rq.get(url + '/problem', headers = header)
        print(f"Status Code: {response.status_code}")
        if response.status_code == 200:
            break
        elif response.status_code == 403:
            time.sleep(0.5)
            continue
        else:
            response.raise_for_status()
    return response.json()

# 文字列のリストを整数のリストに変換する関数
def str_to_int(array: list[str], adder: int = 0) -> list[list[int]]:
    return [[int(char) + adder for char in string] for string in array]

def make_fig(board: list[list[int]], size: int) -> Image:
    width = len(board[0])
    height = len(board)
    img = Image.new('RGB', (width, height))
    pixels = img.load()
    for y in range(height):
        for x in range(width):
            pixels[x, y] = color_dict[board[y][x]]

    if width > height:
        new_width = size
        new_height = int(size * height / width)
        img = img.resize((new_width, new_height), resample=Image.NEAREST)
        back = Image.new('RGBA', (size, size), (0, 0, 0, 0))
        back.paste(img, (0, size//2 - new_height//2))
        return back
    elif width < height:
        new_width = int(size * width / height)
        new_height = size
        img = img.resize((new_width, new_height), resample=Image.NEAREST)
        back = Image.new('RGBA', (size, size), (0, 0, 0, 0))
        back.paste(img, (size//2 - new_width//2, 0))
        return back
    else:
        new_width = size
        new_height = size
        return img.resize((new_width, new_height), resample=Image.NEAREST)


# configファイルの読み込み
with open(dir.parent / config_file, 'r', encoding='utf-8') as f:
    config = yaml.safe_load(f)

url = config['match_server']["url"]
token = config['match_server']["token"]
match_time = datetime.timedelta(seconds=config['match_server']['match_time'])

# 問題情報を取得
problem = get_problem(url, token)
# 開始時間と試合時間、終了時間を取得
start_at = datetime.datetime.now()
stop_at = start_at + match_time

# ボードの幅と高さを取得
board_width = problem["board"]["width"]
board_height = problem["board"]["height"]

# 初期盤面と最終盤面の画像を生成
start = make_fig(str_to_int(problem["board"]["start"]), image_size)
goal = make_fig(str_to_int(problem["board"]["goal"]), image_size)

# 一般抜型のパターンサイズと画像を取得
n = problem["general"]["n"]
general_size = [(pattern["width"], pattern["height"]) for pattern in problem["general"]["patterns"]]
general_cells = [make_fig(str_to_int(pattern["cells"], adder=4), image_size) for pattern in problem["general"]["patterns"]]

# Streamlitの設定
st.markdown("# Board Image ")

# 初期盤面と最終盤面を表示
col1, col2 = st.columns(2)
with col1:
    st.image(start, caption="初期盤面", use_container_width=True)
with col2:
    st.image(goal, caption="最終盤面", use_container_width=True)

formatted_start_at = start_at.strftime('%Y-%m-%d %H:%M:%S')
formatted_stop_at = stop_at.strftime('%Y-%m-%d %H:%M:%S')
st.markdown(f'''
## ボード情報
- 横\: **{board_width}**
- 縦\: **{board_height}**

## 時間
- start time\: **{formatted_start_at}**
- end time\: **{formatted_stop_at}**

## 一般抜き型数: {n}
'''.format(board_width=board_width, board_height=board_height, start_at=start_at, stop_at=stop_at, general_size=general_size, n=n))

# 一般抜型の画像を表示（アコーディオン形式）
st.markdown("## 一般抜型の画像")
with st.expander("### 一般抜型の画像一覧を表示"):
    col3, col4 = st.columns(2)  # 2列に分割する
    for i, [size, image] in enumerate(zip(general_size, general_cells), 25):
        width, height = size
    
        # 交互にカラムに画像を表示
        if i % 2 == 1:
            with col3:
                st.image(image, caption=f"番号 {i}, 横 {width} , 縦 {height}", use_container_width=True)
        else:
            with col4:
                st.image(image, caption=f"番号 {i}, 横 {width} , 縦 {height}", use_container_width=True)

# 一般抜型サイズの情報を文字列に整形して表示（アコーディオン形式）
with st.expander("### 一般抜型サイズの詳細を表示"):
    general_info = "\n".join([f"- 番号:{i},(横,縦) = ({size[0]}, {size[1]})" for i, size in enumerate(general_size, 25)])
    st.markdown('## 一般抜型サイズ')
    st.markdown(general_info)

# 問題データの保存
filename = start_at.strftime('%y%m%d_%H%M%S')
with open(dir / problem_dir / f'{filename}.json', 'w', encoding='utf-8') as f:
    json.dump(problem, f, indent=4)

# 画像データの保存
image_dir_now = dir / image_dir / filename
(image_dir_now).mkdir()
start.save(image_dir_now / 'start.png', format = 'PNG')
goal.save(image_dir_now / 'goal.png', format = 'PNG')
for i, image in enumerate(general_cells, 25):
    image.save(image_dir_now / f'general_{i}.png', format = 'PNG')

