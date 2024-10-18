import re
import yaml
import socket
from pathlib import Path

config_file = 'config.yml'
writen_file = 'solver/src/main.cpp'

dir = Path(__file__).parent.parent

pattern = re.compile('''\
namespace config {
    // 通信のインターバル\\[ms\\]
    const int INTERVAL_TIME = 500;
    // 通信の最高リクエスト回数
    const int MAX_REQEST = 200;

    // 大会サーバのIP
    const string MATCH_SERVER_URL = "(.*)";
    // 大会サーバと通信する際のトークン
    const string MATCH_SERVER_TOKEN = "(.*)";

    // 解答管理サーバーのIP
    const string ANSWER_MANAGER_URL = "(.*)";
    // 解答管理サーバーと通信する際のトークン
    const string ANSWER_MANAGER_TOKEN = "(.*)";
}\
''')



replacements = '''\
namespace config {{
    // 通信のインターバル[ms]
    const int INTERVAL_TIME = 500;
    // 通信の最高リクエスト回数
    const int MAX_REQEST = 200;

    // 大会サーバのIP
    const string MATCH_SERVER_URL = "{}";
    // 大会サーバと通信する際のトークン
    const string MATCH_SERVER_TOKEN = "{}";

    // 解答管理サーバーのIP
    const string ANSWER_MANAGER_URL = "{}";
    // 解答管理サーバーと通信する際のトークン
    const string ANSWER_MANAGER_TOKEN = "{}";
}}\
'''

def get_ip():
    try:
        hostname = socket.gethostname()
        return socket.gethostbyname(hostname)
    except Exception as e:
        return f'Error: {e}'

with open(dir / config_file, 'r', encoding='utf-8') as f:
    config = yaml.safe_load(f)

manager_ip = input('解答管理サーバーのIPアドレス (例: 127.0.0.1) : ')
if manager_ip == '':
    print(f'自身のIPアドレスは {get_ip()} です')
    config['manager']['ip'] = get_ip()
else:
    config['manager']['ip'] = manager_ip
print(f"解答管理サーバのIPアドレスを {config['manager']['ip']} に設定しました")
print()

server_ip = input('大会サーバーのIPアドレス(ポート番号も含める) (例: 127.0.0.1:3000) : ')
config['match_server']['url'] = f'http://{server_ip}'
print(f"大会サーバのIPアドレスを {config['match_server']['url']} に設定しました")
print()

replacements = replacements.format(
    config['match_server']['url'],
    config['match_server']['token'],
    f"http://{config['manager']['ip']}:{config['manager']['port']}",
    config['self']['id'],
)

if (input('設定ファイルを書き換えますか？ (Y/n) : ') == 'Y'):
    with open(dir / config_file, 'w', encoding='utf-8') as f:
        yaml.dump(config, f, default_flow_style=False, allow_unicode=True)

if (input('ソースファイルを書き換えますか? (Y/n) : ') == 'Y'):
    with open(dir / writen_file, 'r', encoding='utf-8') as f:
        text = f.read()
    with open(dir / writen_file, 'w', encoding='utf-8') as f:
        f.write(re.sub(pattern, replacements, text))