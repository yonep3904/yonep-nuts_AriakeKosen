# use_column_widthとuse_container_widthを入れ替える(streamlitのバージョンによって変更する)

import re
from pathlib import Path

dir = Path(__file__).parent

def main():
    if input('replace column to container? (y/n): ') == 'y':
        replace_column_to_container()
    elif input('replace container to column? (y/n): ') == 'y':
        replace_container_to_column()


def replace_column_to_container():
    for file in (dir / 'data').iterdir():
        visualizer = file / 'visualizer.py'
        with visualizer.open('r') as f:
            lines = f.readlines()

        new_lines = [line.replace('use_column_width', 'use_container_width') for line in lines]

        with visualizer.open('w') as f:
            f.writelines(new_lines)

def replace_container_to_column():
    for file in (dir / 'data').iterdir():
        visualizer = file / 'visualizer.py'
        with visualizer.open('r') as f:
            lines = f.readlines()

        new_lines = [line.replace('use_container_width', 'use_column_width') for line in lines]

        with visualizer.open('w') as f:
            f.writelines(new_lines)

if __name__ == '__main__':
    main()
