import datetime
import yaml
from pathlib import Path
import atexit

import requests as rq
from flask import Flask, request, jsonify

import logging
from rich.logging import RichHandler

#############################################################
# 設定ファイル
config_file = 'config.yml'
# ログ記録ファイル
log_dir = 'log_data'

#############################################################

dir = Path(__file__).parent
now = datetime.datetime.now()

# configファイルの読み込み
with open(dir.parent / config_file, 'r', encoding = 'utf-8') as f:
    config = yaml.safe_load(f)

ip = config['manager']['ip']
port = config['manager']['port']
tokens = config['manager']['tokens']

match_server_url = config['match_server']['url']
match_server_token = config['match_server']['token']

# ロガーの設定
formater = logging.Formatter('[%(asctime)s] %(levelname)-8s %(message)s', datefmt = '%m/%d/%y %H:%M:%S')

stream_handler = RichHandler(markup = True, rich_tracebacks = True)
stream_handler.setLevel(logging.INFO)

file_name = now.strftime('%y%m%d_%H%M%S') + '.log'
file_handler = logging.FileHandler(filename = dir / log_dir / file_name)
file_handler.setFormatter(formater)
file_handler.setLevel(logging.DEBUG)

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)
logger.addHandler(file_handler)
logger.addHandler(stream_handler)

flask_loger = logging.getLogger('werkzeug')
flask_loger.setLevel(logging.ERROR)

# 最短手数
min_moves = float('inf')
app = Flask(__name__)

# トークンの認証
def verify_token(token):
    global tokens
    return token in tokens

def send_answer(answer: str) -> tuple[int, int | None]:
    global match_server_url, match_server_token

    header = {'Content-Type': 'application/json', 'Procon-Token': match_server_token}
    try:
        response = rq.post(match_server_url + '/answer', headers=header, data=answer)
        response.raise_for_status()
        status = response.status_code
        if status == 200:
            revision = response.json()['revision']
            logger.debug(f'POST 200 (revision : {revision})')
            return 200, revision
        else:
            logger.debug(f'POST {status}')
            return status, None
    except:
        logger.error(f'POST -1')
        return -1, None

@app.route('/answer', methods=['POST'])
def manage_answer():
    global min_moves

    # 不正なトークン -> 401
    token = request.headers.get('Authorization')
    if not verify_token(token):
        logger.warning(f'401 : Unauthorized Access')
        return jsonify({'error': 'Unauthorized'}), 401
    
    # 不正なリクエスト内容(簡単なチェックのみ) -> 400
    data = request.get_json()
    if 'n' not in data or 'ops' not in data:
        logger.warning(f'400 : Invalid request from "{token}"')
        return jsonify({'error': 'Invalid request'}), 400
    
    if data['n'] < min_moves:
        status, revision = send_answer(request.get_data(as_text=True))
        match status:
            # 不正なリクエスト内容 -> 400
            case 400:
                logger.warning(f'400 : Invalid request from "{token}"')
                return jsonify({'error': 'Invalid request'}), 400
            # 不正なトークン(解答管理サーバー側に問題あり) -> 500
            case 401:
                logger.error(f'500 : Answer_manager Unauthorized from "{token}"')
                return jsonify({'error': 'Answer_manager Unauthorized'}), 500
            # タイムアウト -> 403
            case 403:
                logger.warning('403 : Outside access times from "{token}"')
                return jsonify({'error': 'Outside access times'}), 403
            # 解答の送信に成功 -> 200
            case 200:
                logger.info(f'200 : update and submit from "{token}" (revision : {revision})')
                tmp = min_moves
                min_moves = data['n']
                logger.info(f'Update from {tmp} to {min_moves}')
                return jsonify({'moves': -1}), 200
    else:
        logger.info(f'200 : non update from "{token}')
        return jsonify({'moves': min_moves}), 200

@atexit.register
def exit_process():
    logger.info(f'End server with min_moves : {min_moves}')

if __name__ == '__main__':
    logger.info('Run server')
    app.run(host=ip, port=port)
