from pathlib import Path
import chardet

dir = Path(__file__).parent

ignore = [
    ".vscode/",
    "__MACOSX/"
    "html/",
    "procon-server/",
    "openapi.json",
    "lib/",
    "trash/",
    "init.js",
]

def is_directory_in(path: Path, base_directory: Path, target_directory: Path):
    relative_path = path.resolve().relative_to(base_directory.resolve())
    return target_directory in [part for part in relative_path.parts]

def is_igunore(path: Path, base_directory: Path, ignore_list: list[str]):
    for i in ignore_list:
        if i.endswith('/'):
            if is_directory_in(path, base_directory, i[:-1]):
                return True
        else:
            if path.name == i:
                return True
    else:
        return False

def is_text_file(path: Path):
    """ファイルがテキストファイルかどうかを判定する。"""
    try:
        with path.open('rb') as file:
            raw_data = file.read(1024)
            result = chardet.detect(raw_data)
            return result['encoding'] is not None and result['encoding'].lower() not in ['utf-16', 'utf-32']
    except Exception as e:
        print(f"Error checking file {path}")
        print(f"{e}")
        return None

def count_lines_and_characters(path: Path):
    """ファイルの行数と文字数をカウントする"""
    try:
        with path.open('r', encoding='utf-8') as file:
            lines = file.readlines()
            line_count = len(lines)
            char_count = sum(len(line) for line in lines)
        return line_count, char_count
    except Exception as e:
        # print(f"Error reading file {path}: {e}")
        return None, None

def process_directory(directory_path: Path):
    """ディレクトリ内のテキストファイルの行数と文字数をカウントする。"""

    total_line_count = 0
    total_char_count = 0
    
    for file in directory_path.rglob('*'):
        if not is_igunore(file, dir, ignore):
            line_count, char_count = count_lines_and_characters(file)
            if line_count is None:
                pass
            else:
                total_line_count += line_count
                total_char_count += char_count
                print(f"{file.relative_to(directory_path)}")
                print(f"行数 : {line_count}")
                print(f"文字数 : {char_count}")
                print()
        else:
            # print(f'ignore : {file}')
            pass

    print("Total:")
    print(f"行数 : {total_line_count}")
    print(f"文字数 : {total_char_count}")

process_directory(dir)