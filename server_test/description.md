# Procon35 OpenAPI

- version : 1.0
- 第35回高専プロコン競技部門で使用するAPIサーバーの仕様です。
- APIは/probremと/answerの2種類があります。
- 認証にはチームごとに事前配布したトークンを用います。
- トークンをクエリパラメータ(token)かヘッダー(procon-token)に指定してリクエストしてください。

## サーバーとの通信について

### URL

URLはネットワーク上でのある**ファイルの場所**を示すものです。\
一般に、URLは次のようになります。`[通信プロトコル]://[サーバーのIPアドレス]:[ポート番号]/[ファイルのパス]?[クエリ]`

- 通信プロトコル : http, https など
- IPアドレス(ドメイン名でも可) : www.ariake-nct.ac.jp, 202.220.124.40 など
- ポート番号 : 80, 3000(ウェルノウンポート番号の場合通常は省略、httpのウェルノウンポート番号は80)
- ファイルのパス : other-info/2024/07/26507.html など
- クエリ : リクエストする際の引数。token=123456 のようにXX=YYの形で記述。

例として、`https://www.ariake-nct.ac.jp/other-info/2024/07/26507.html`というURLでリクエストした場合、ドメイン名www.ariake-nct.ac.jpのサーバーの中の絶対パス/other-info/2024/07/26507.htmlのファイルに対して、(ポート番号80で)httpsプロトコルで通信するという意味になります。

今回の場合、URL:`http://[サーバーIPアドレス]/[API名]?token=[トークン]`でサーバーと通信することになります(トークンはヘッダーで指定するかもしれないので、その場合、URL中にトークンのクエリは書かずに `http://[サーバーIPアドレス]/[API名]`になります)。

例(多分本番はこんなかんじ) : `http://http://172.28.0.1:8080/probrem?token=ariakee5d5af0c7ad9401b6449eda7ee0e8730f24f77d5b6da2ac615aca3c1f4`

### httpリクエスト

httpリクエスト中には以下の情報が含まれています。

- リクエスト行
- ヘッダーフィールド
- ボディ

httpリクエストには、具体的にどんなこと(メソッド)をしてほしいのかを指定します。\
今回使用するメソッドは以下の二つです。

- GET : サーバー側にデータの送信を要求する(\probremはこれ)
- POST : サーバーにデータを送信する(\answerはこれ)

メソッドはリクエスト行に含まれています。\
また、いずれのリクエストもサーバーから返答(response)が返ってきます。

### httpレスポンス

レスポンスには以下の情報が含まれています。

- ステータス行
- ヘッダーフィールド
- ボディ

ステータス行には3桁の数字(ステータスコード)で構成されています。\
大まかに、以下のように分類されます。

- 100番台「情報」
- 200番台「成功」
- 300番台「リダイレクト」
- 400番台「クライアントエラー」(リクエストした側に問題がある)
- 500番台「サーバエラー」(サーバー側に問題がある)

なお、POSTやレスポンスで送信されるjsonデータはボディに含まれています。

### どんなプログラムで通信する?

多くの言語では専用のライブラリ等を用います。\
以下はPythonのプログラムです。
```request.py
import request as rq

url = "http://127.0.0.1:3000"
api_token = "ariakee5d5af0c7ad9401b6449eda7ee0e8730f24f77d5b6da2ac615aca3c1f4"

header = {"Content-Type" : "application/json",
          "procon-token" : api_token,
          }

# GET(問題情報の取得)  
response = rq.get(url + "/problem", headers = header)
status_code = response.status_code
problem_data = response.json()

# POST(回答の送信)
response = rq.post(url + "/answer", headers = header, data = "<jsonデータ>")
status_code = response.status_code
problem_data = response.json()
  
```
## /probrem (http://[サーバー名]/probrem)

### GET

- operation ID : GetProbrem
- 問題を取得するAPIです
- パラメータは以下を指定
  - トークン(string) : クエリまたはヘッダーで指定

### responses

- 200
  - 正常
  - 内容として以下が返される

```問題情報(json)
{
    "startsAt": 123456, //試合開始時刻です(integer, 0~)
    "board": {         // ボードの情報です
        "width": 6,    // 横のピース数です(integer, 32~256)
        "height": 4,   // 縦のピース数です(integer, 32~256)
        "start": [     // ボードの初期状態です(array[string, "^[0123]+$"])
            "220103",
            "213033",
            "022103",
            "322033"
        ],
        "goal": [     // ボードの最終状態です(array[string(pattern : "^[0123]+$")])
            "000000",
            "111222",
            "222233",
            "333333"
        ]
    },
    "general": {              // 一般抜き型の情報です
        "n": 2,               // 一般抜き型の個数です(integer, 0~256)
        "patterns": [
            {
                "p": 25,     // 抜き型番号です。一般抜き型は25番から始まります(integer, 25~)
                "width": 4,  // 横のセル数です(integer, 1~256)
                "height": 2, // 縦のセル数です(integer, 1~256)
                "cells": [   // セルの状態です(array[string, "^[01]+$"])
                    "0111",
                    "1001"
                ]
            },
            {
                "p": 26,
                "width": 2,
                "height": 2,
                "cells": [
                    "10",
                    "01"
                ]
            }
        ]
    }
}

```

- 400
  - リクエストの内容が不正である場合にエラーになります
- 401
  -トークンが指定されていない、または不正である場合にエラーになります
- 402
  - 競技時間外にアクセスした場合にエラーになります
  - 内容として以下が返される
    - 問題情報(text(utf-8), string, enum("AccessTimeError"))
- default(その他)
  - 予期しないエラーです

## /answer (http://[サーバー名]/answer)

### POST

- operation ID : SubmitAnswer
- 回答を提出するAPIです
- パラメータは以下を指定
  - トークン(string) : クエリまたはヘッダーで指定
- 内容として以下を送信

```回答情報(json)
{
    "n":3,         // 手数です(integer, 0~)
    "ops":[
        {
            "p":4, // 抜き型番号です(integer, 0~)
            "x":5, // 適用X座標です(integer, -256~256)
            "y":6, // 適用Y座標です(integer, -256~256)
            "s":2  // 寄せ方向です。0が上,1が下,2が左,3が右です
                   // (integer, enum(0,1,2,3))
        },
        {
            "p":11,
            "x":3,
            "y":-12,
            "s":3
        },
        {
            "p":25,
            "x":6,
            "y":0,
            "s":0
        }
    ]
}

```

### responses

- 200
  - 正常
  - 内容として以下が返される

```問題情報(json)
{
    revision : 0  // 受理番号です。この値が最も大きいものが最終的に受理されています
                  // (integer, 0~)
}

```

- 400
  - リクエストの内容が不正である場合にエラーになります
- 401
  -トークンが指定されていない、または不正である場合にエラーになります
- 402
  - 競技時間外にアクセスした場合にエラーになります
  - 内容として以下が返される
    - 問題情報(text, utf-8, string, "AccessTimeError")
- default(その他)
  - 予期しないエラーです
