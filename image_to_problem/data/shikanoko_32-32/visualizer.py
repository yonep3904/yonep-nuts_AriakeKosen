import datetime
import json
from pathlib import Path
from PIL import Image
import streamlit as st

#####################################################
problem_name = 'probllem.json'
image_size = 1000
#####################################################

dir = Path(__file__).parent
problem_path = dir / 'problem.json'

with open(problem_path, 'r') as f:
    problem = json.load(f)

def str_to_int(array: list[str], adder: int = 0) -> list[list[int]]:
    return [[int(char) + adder for char in string] for string in array]

def make_fig(board: list[list[int]], size: int, color_dict) -> Image:
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


match_time = datetime.timedelta(seconds=300)

# 開始時間と試合時間、終了時間を取得
start_at = datetime.datetime.now()
stop_at = start_at + match_time

# ボードの幅と高さを取得
board_width = problem['board']['width']
board_height = problem['board']['height']

# 色の取得
color_dict = {i: tuple(color) for i, color in enumerate(problem['colors'] + [[0, 0, 0], [255, 255, 255]])}

# 初期盤面と最終盤面の画像を生成
start = make_fig(str_to_int(problem['board']['start']), image_size, color_dict=color_dict)
goal = make_fig(str_to_int(problem['board']['goal']), image_size, color_dict=color_dict)

# 一般抜型のパターンサイズと画像を取得
n = problem['general']['n']
general_size = [(pattern['width'], pattern['height']) for pattern in problem['general']['patterns']]
general_cells = [make_fig(str_to_int(pattern['cells'], adder=4), image_size, color_dict=color_dict) for pattern in problem['general']['patterns']]

# Streamlitの設定
st.markdown('# Board Image ')

# 初期盤面と最終盤面を表示
col1, col2 = st.columns(2)
with col1:
    st.image(start, caption='初期盤面', use_container_width=True)
with col2:
    st.image(goal, caption='最終盤面', use_container_width=True)

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
st.markdown('## 一般抜型の画像')
with st.expander('### 一般抜型の画像一覧を表示'):
    col3, col4 = st.columns(2)  # 2列に分割する
    for i, [size, image] in enumerate(zip(general_size, general_cells), 25):
        width, height = size
    
        # 交互にカラムに画像を表示
        if i % 2 == 1:
            with col3:
                st.image(image, caption=f'番号 {i}, 横 {width} , 縦 {height}', use_container_width=True)
        else:
            with col4:
                st.image(image, caption=f'番号 {i}, 横 {width} , 縦 {height}', use_container_width=True)

# 一般抜型サイズの情報を文字列に整形して表示（アコーディオン形式）
with st.expander('### 一般抜型サイズの詳細を表示'):
    general_info = '\n'.join([f'- 番号:{i},(横,縦) = ({size[0]}, {size[1]})' for i, size in enumerate(general_size, 25)])
    st.markdown('## 一般抜型サイズ')
    st.markdown(general_info)
