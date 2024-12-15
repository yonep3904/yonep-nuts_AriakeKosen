# dataディレクトリ上にあるデータを削除するスクリプト

import shutil
from pathlib import Path

dir = Path(__file__).parent
data_dir = 'data'

for data in (dir/data_dir).iterdir():
    if data.is_file():
        continue
    shutil.rmtree(data)

print('data removed')