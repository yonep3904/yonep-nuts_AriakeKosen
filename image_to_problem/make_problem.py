# 画像をもとに問題を生成(k-means)するスクリプト

import json
import shutil
import subprocess
import atexit
from pathlib import Path
from PIL import Image

import numpy as np
from sklearn.cluster import KMeans

######################################
image_dir = 'image'
data_dir = 'data'

sizes = [(32, 32), (64, 64), (128, 128)]
colors_n = 4 # 固定
seed = 42 # 固定

visualizer_file_text = """\
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
    general_info = '\\n'.join([f'- 番号:{i},(横,縦) = ({size[0]}, {size[1]})' for i, size in enumerate(general_size, 25)])
    st.markdown('## 一般抜型サイズ')
    st.markdown(general_info)
"""

######################################

dir = Path(__file__).parent

# 画像を指定されたサイズにリサイズし、指定された色数に減色する
def reduce_colors(img: Image, size: tuple[int, int], colors: int = 4) -> tuple[np.ndarray, np.ndarray]:
    # サイズの変更
    img = img.resize(size)

    # 色によるピクセルのクラスタリング
    img.convert('RGB')
    #imgをndarrayの(HW, C)の形に変換
    img_array = np.array(img).reshape(-1, 3)
    kmeans_model = KMeans(n_clusters = colors).fit(img_array)

    quantized_image = kmeans_model.labels_.reshape(size)
    quantized_colors = kmeans_model.cluster_centers_.astype(np.uint8)
    return quantized_image, quantized_colors

# 問題を生成する
def make_problem(quantized_image: np.ndarray, colors: np.ndarray, seed: int|None = None):
    rng = np.random.default_rng(seed)
    height, width = quantized_image.shape

    start = rng.permutation(quantized_image.flatten()).reshape((height, width)).tolist()
    goal = quantized_image.tolist()

    general = [
        [
            [1, 0],
            [0, 1],
        ],
    ]
    # color_RGB = {i: colors[i].tolist() for i in range(colors.shape[0])}
    color_RGB = colors.tolist()

    to_string = lambda list2D : [''.join(i) for i in map(lambda col: map(str, col), list2D)]

    j_dict = {
        'board': {
            'width': width,
            'height': height,
            'start': to_string(start),
            'goal': to_string(goal),
        },
        'general': {
            'n': len(general),
            'patterns': [],
        },
        'colors': color_RGB
    }

    for p, pattern in enumerate(general):
        tmp_dict = {
            'p': 25 + p,
            'width': len(pattern[0]),
            'height': len(pattern),
            'cells' : to_string(pattern)
        }
        j_dict['general']['patterns'].append(tmp_dict)

    return j_dict

# 画像を読み込んで問題を生成する
def make_problem_with_image(img_path: Path, size: tuple[int, int], colors: int = 4, seed: int|None = None):
    img = Image.open(img_path)
    quantized_image, quantized_colors = reduce_colors(img, size, colors)
    return make_problem(quantized_image, quantized_colors, seed)

# 問題情報をもとに画像を生成する
def make_fig(board: list[list[int]], size: int, colors: list[tuple[int, int, int]]) -> Image:
    board = [[int(char) for char in string] for string in board]

    width = len(board[0])
    height = len(board)
    img = Image.new('RGB', (width, height))
    pixels = img.load()
    for y in range(height):
        for x in range(width):
            pixels[x, y] = colors[board[y][x]]

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


# main
for image in (dir/image_dir).iterdir():
    if image.name == '.gitkeep':
        continue
    
    process = []
    for size in sizes:
        data = dir/data_dir/f'{image.stem}_{size[0]}-{size[1]}'
        if data.exists():
            continue
        data.mkdir()

        # 画像のコピー
        shutil.copy(image, data / ('source' + image.suffix))

        # 問題データ(.json)の作成
        problem = make_problem_with_image(image, size, colors_n, seed)
        with open(data/'problem.json', 'w') as f:
            json.dump(problem, f, indent=4)
        print(f'saved : {data}-problem')

        # 問題(.png)の画像を作成
        colors = [tuple(color) for color in problem['colors']]
        start_image = make_fig(problem['board']['start'], 1000, colors)
        start_image.save(data/'start.png')
        goal_image = make_fig(problem['board']['goal'], 1000, colors)
        goal_image.save(data/'goal.png')
        print(f'saved : {data}-image.png')

        # visualizer.pyの作成
        with open(data/'visualizer.py', 'w', encoding='utf-8') as f:
            f.write(visualizer_file_text)

        # 解答(.gif)の作成
        process.append((subprocess.Popen(['make_gif.exe', str(data.resolve()), '1', str(512//size[0]), data]), data))
        print(f'making... : {data}-solution')
    for p in process:
        p[0].wait()
        print(f'finished : {p[1]}-solution')

@atexit.register
def kill_process():
    try:
        process
    except NameError:
        pass
    else:
        for p in process:
            p[0].kill()
