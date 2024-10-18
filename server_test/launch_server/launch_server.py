import re
import time
import datetime
import json
import os
import sys
import atexit
import subprocess
import platform
import socket
import threading
from pathlib import Path
from argparse import ArgumentParser
from color import colored
from problem import Problem, make_problem

##########################################################################
# 初期化ファイル名
PATH = 'initial.json'
# 実行ファイルの指定(Noneで自動選択)
run_file = None

# ポート番号
PORT = 3000
# 試合開始までの時間
READY_TIME = 10
# トークン
TOKEN = "ariake90ac96700a7e0f8208caad8bbcc7a8efebf4703ec4a1b9560be92f14b4"

##########################################################################

reg = re.compile(r'\d{4}/\d{2}/\d{2} (\d{2}:\d{2}:\d{2}) ボードの最終状態:')
dir = Path(__file__).parent
os_name = platform.system()

# 簡易サーバープロセス標準出力参考
'''\
2024/08/26 21:48:28 ボードの最終状態:
3213031002
0020301230
3220021311
0101220203
1132332002
2031301213
1023311321
3221113321
3100303231
3133202320
'''

# OS環境によって実行ファイルを選択
if run_file is None:
    if os_name == 'Linux':
        run_file = "procon-server/procon-server_linux"
    elif os_name == 'Windows':
        run_file = "procon-server/procon-server_win.exe"
    elif os_name == 'Darwin':
        run_file = "procon-server/procon-server_darwin_amd"
    # armの場合に対応していない
    # run_file = "procon-server/procon-server_darwin_arm"

# ipアドレスの取得
try:
    hostname = socket.gethostname()
    ip_address = socket.gethostbyname(hostname)
except Exception as e:
    ip_address = "Error"

# プログラム終了時に行う処理を指定
@atexit.register
def kill():
    try:
        process
    except NameError:
        pass
    else:
        # process.stdout.close()
        process.terminate()
        process.wait()
        print("サーバーを終了しました")

# initial.jsonファイルの作成
def save_initial(path: Path|str, tokens: list[str], duration: int, problem: Problem) -> None:
    initial_json = {
        "teams": tokens,
        "duration": duration,
        "problem": problem.to_json_dict(),
    }

    with open(path, 'w') as f:
        json.dump(initial_json, f, indent=4)

# 簡易サーバープロセスの標準出力を監視し、それをもとに最終盤面が揃っているか判定して結果を標準出力する
def read_thread(process: subprocess.Popen, goal: list[str]):
    buffer = []
    while True:
        output = process.stdout.readline()
        if output:
            if hit := reg.match(output):
                buffer = [hit.group(1)]
            else:
                buffer.append(output.strip('\n'))
            
            if len(buffer) == height + 1:
                print(f"{buffer[0]} : {buffer[1:] == goal}")

# コマンドライン引数の解析
parser = ArgumentParser(description="簡易サーバーの立ち上げ")
parser.add_argument("width", type=int, help="ボードの横ピース数")
parser.add_argument("height", type=int, help="ボードの縦ピース数")
parser.add_argument("method", help="ボードの生成方法[fill, vertical, horizontal, diagonal, random]")
parser.add_argument("seed", type=int, default=None, help="シード値")
parser.add_argument("teams", type=int, default=3, help="チーム数")
parser.add_argument("duration", type=int,default=300, help="試合時間")
parser.add_argument("--swap", action='store_true', help="初期盤面と最終盤面を入れ替える")
parser.add_argument("--general_off", action='store_true', help="一般抜き型を生成しない(最低限のみ)")

args = parser.parse_args()
width, height, method, seed, teams, duration, swap, general_off = \
    args.width, args.height, args.method, args.seed, args.teams, args.duration, args.swap, args.general_off
    
# 問題データの生成
problem = make_problem(width, height, method, seed, swap, general_off)
tokens = [TOKEN] + [f"token{i+1}" for i in range(teams)]
save_initial(dir/PATH, tokens, duration, problem)

# 簡易サーバーの立ち上げ
process = subprocess.Popen(f"{run_file} -c {PATH} -l :{PORT} -start {READY_TIME}s",
                           stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT,
                           encoding='utf-8',
                           )

# 現在時刻(簡易サーバー立ち上げ時刻)の取得
start = datetime.datetime.now()

# 情報の表示

colored_ready_time = colored(f"{READY_TIME}秒後", "blue")

colored_width = colored(f"{width}", "green")
colored_height = colored(f"{height}", "green")
colored_method = colored(f"{method}", "green")
colored_seed = colored(f"{seed}", "green")
colored_duration = colored(f"{duration}", "green")
colored_start_time = colored(start.strftime('%H:%M:%S'), "green")

colored_ip_address = colored(f"{ip_address}", "blue")
colored_port = colored(f"{PORT}", "blue")
colored_url = colored(f"http://{ip_address}:{PORT}", "blue")
colored_tokens = colored("\n".join(tokens), "light_blue")

print("""\
-----------------------------------------
試合は{}に開始されます
---
試合情報 :
width : {}
height : {}
ボードの生成方法 : {} (seed={})
試合時間 : {} (サーバーの立ち上げ開始時刻 : {})
---
IPアドレス : {}
ポート番号 : {}
(URL : {})
トークン : 
{}
---
サーバーを終了させる場合は{}を押してください
""".format(colored_ready_time, 
           colored_width, colored_height, colored_method, colored_seed, colored_duration, colored_start_time,
           colored_ip_address, colored_port, colored_url, colored_tokens,
           colored("[Ctrl + C]", "light_yellow"),
           )
)

print("試合待機中...")
time.sleep(READY_TIME)
print(colored("試合が開始しました", "light_cyan"))

# 簡易サーバープロセスの標準出力監視スレッドを立てる
worker = threading.Thread(target=read_thread, args=(process, problem.get_goal()), daemon=True)
worker.start()

time.sleep(duration)
print(colored("試合が終了しました", "light_cyan"))
time.sleep(5)

# 簡易サーバープロセスの終了と親プロセスの強制終了
# sys.exit(0)だとうまく動かなかった(メインプロセスが停止しない)
kill()
os._exit(0)