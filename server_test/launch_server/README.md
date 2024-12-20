# Procon35 server
jsonファイルを生成して簡易サーバーを立ち上げるプログラムです。  
プログラム終了(`Ctrl + C`などによる終了も含む)の際に自動で簡易サーバープロセスも終了させます。

## 簡易サーバーを動かす方法
~~Windows以外では利用できません。~~ どのOS環境でも利用できます。  
WIndowsの場合、コマンドライン上での操作は Powershell 上で行ってください。

### 事前準備
利用にはPythonとそのライブラリの NumPy が必要です。

Python のインストール  
実行する場所は関係ないのでどこでもいいです

```PowerShell
>>> winget install -e --id Python.Python.3.12
```

Pythonのライブラリのインストール  
実行する場所は関係ないのでどこでもいいです
```PowerShell
>>> pip install numpy
```

このフォルダのダウンロード & 解凍  
フォルダ内のファイルは消したり名前を変えたりしないでください。
```
レポジトリ右上 Code から zip でダウンロードしてどこかに展開してください
```

### Run
PowerShell 上でこのディレクトリ`.\launch_sever`に移動する
```
Linuxと同じくcdコマンドとlsコマンドで移動できます。
```

`python .\launch_server.py [コマンドライン引数]`または`.\launch_server.bat　[コマンドライン引数]`を実行する(どちらで実行してもOK)

コマンドライン引数の説明
1. width : ボードの横ピース数(整数値で指定)
1. height :  ボードの縦ピース数(整数値で指定)
1. method : 初期盤面の生成方法(以下の文字列から指定)
    - fill : 全てのピースが0
    - vertical : 縦縞模様(列ごとにピースの数値が0,1,2,3,0...の順に変わる)
    - horizontal : 横縞模様(行ごとにピースの数値が0,1,2,3,0...の順に変わる)
    - diagonal : 斜めの縞模様
    - random : ランダム(各数値のピースが全体ピースの10％以上あることは保証されている)
1. seed  : シード値(整数値で指定)\[デフォルト値 : None\]
1. teams : 参加チーム数(整数値で指定)\[デフォルト値 : 3\]
1. duration : 試合時間\[s\](整数値で指定)\[デフォルト値 : 300\]
- \-\-swap : 指定すると初期盤面と最終盤面を入れ替わる
- \-\-general_off : 一般抜き型を生成しない

シード値に`None`と入力した場合、シード値がランダムに決定されます。  
初期盤面を指定した方法で生成後、最終盤面は初期盤面をシャッフルしたものになります。(デフォルトの場合)

```PowerShell
>>> .\launch_server.bat 32 32 random 42 3 300
-----------------------------------------
ポート番号は3000, 試合は04:37:33に開始されます
-
試合情報 :
width : 10
height : 10
ボードの生成方法 : random (seed=42)
試合時間 : 300 (サーバーの立ち上げ開始時刻 : 04:37:33)
-
トークンは以下
ariake90ac96700a7e0f8208caad8bbcc7a8efebf4703ec4a1b9560be92f14b4
token1
token2
token3
-
サーバーを終了させる場合は[Ctrl + C]を押してください

試合待機中...

## 試合開始後 ##
試合が開始しました

## 試合中に解答がPOSTされると　                               ##
## [解答受理時刻] : [解答が最終盤面と一致しているか]  が出力される ## 
04:59:23 : True
04:59:41 : False

## 試合終了後 ##
試合が終了しました
サーバーを終了しました
```

途中で簡易サーバーを終了させたい場合、`[Ctrl + C]`を入力してください。

正常に終了すると`サーバーを終了しました`という出力が行われます。
この出力が行われなかった場合、簡易サーバーのプロセスが残り続け、もう一度簡易サーバーを立てたときにエラーが出てしまう可能性があります。
簡易サーバー立ち上げにエラーが出た場合は、一度PowerShellを閉じてから実行してみてください。

### 使い方がわからない/何かエラーが出るとき

エラーが出る場合、コマンドライン引数が間違ってないかなどを確認してみてください。

コマンドライン引数がわからなくなった場合、引数に`-h`または`--help`と指定してプログラムを実行してください。
コマンドライン引数に関する説明が出てきます。
```PowerShell
>>> .\launch_server.bat -h

## コマンドライン引数の説明が表示される ##

```

## 簡易サーバーとの通信

簡易サーバーを立ち上げると、同じネットワーク内のコンピュータであれば(簡易サーバを立ち上げたコンピュータ自身でも)簡易サーバーにアクセスすることができます。  
簡易サーバーとは以下のようなURLで通信します。
```
http://[IPアドレス]:[ポート番号]/[API名]

//　同一ネットワーク内のコンピュータが簡易サーバーと通信する場合
http://[簡易サーバを立ち上げたコンピュータのIPアドレス]:3000/[API名]

// 簡易サーバーを立ち上げたコンピュータ自身が簡易サーバーと通信する場合(以下の4つどれでも可)
http://[自身のIPアドレス]:3000/[API名]
http://localhost:3000/[API名]
http://127.0.0.1:3000/[API名]
http://::1:3000/[API名]
```

なお、IPアドレスを確認したいときは`ipconfig`コマンドで確認できます。

### 問題情報取得

`/problem` にtokenをつけてGETリクエストを送ることで試合情報が取得できます。
試合時間内であればproblemがjson形式で返却されます。

```
>>> curl -v localhost:3000/problem -H "Procon-Token: token1"
```

### 回答提出方法

`/answer` にjsonをPOSTすることで回答を送信できます。
回答の形式については募集要項を参照。

```json
{
  "n":3,
  "ops":[
    {
      "p":4,
      "y":1,
      "s":2
    },
    {
      "p":11,
      "x":3,
      "y":-12,
      "s":3
    },
    {
      "p":25,
      "x":3,
      "y":3,
      "s":0
    }
  ]
}
```

回答が受理されるとrevisionという回答を受け付けた番号が返却されます。

```
>>> curl -X POST -H "Content-Type: application/json" -H "Procon-Token: token1" localhost:3000/answer -d @solution.json
```
