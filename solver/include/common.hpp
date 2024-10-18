// __   _____  _   _ _____ ____       _   _ _   _ _____ ____
// \ \ / / _ \| \ | | ____|  _ \     | \ | | | | |_   _/ ___|
//  \ V / | | |  \| |  _| | |_) |____|  \| | | | | | | \___ \ 
//   | || |_| | |\  | |___|  __/_____| |\  | |_| | | |  ___) |
//   |_| \___/|_| \_|_____|_|        |_| \_|\___/  |_| |____/

/*
やることりすと
通信の時の例外処理
make_move_iteratorによる高速化が可能?(rightとleftのみ)
splver::greedy::solverに最適化の余地あり
-------
やったことりすと
ボードの回転と転置(BoardTemplate) -> II, III, GENERALのテスト
型抜き(operate) 最適化の余地あり
時間の計測
legal_chack()関数
参照渡しの確認
historyをどうするか
json形式での入力の実装
boardのprint/operateメソッドの実装
Board::back関数

*/

#ifndef COMMON_HPP
#define COMMON_HPP

// Display を定義 -> デバック用の標準出力が行われる
// 本番時にはここをコメントアウトすること
// #define DEBUG

#include <iostream>
#include <cstdint>
#include <vector>


// ピースとセルの型名のエイリアス
// 本番時はint -> std::int_fast8_t に変更すること
using PieceType = std::int_fast8_t;
// ボードと抜き型に用いる2次元配列のエイリアス
using Vector2D = std::vector<std::vector<PieceType>>;

// 座標(x-y)の構造体
struct PointXY;
// 型抜き操作の構造体.「抜き型タイプ, 抜き型サイズ, 適用座標, 方向」などの情報が含まれる
struct Operation;
// PointXY構造体とscore(評価)を持つ構造体.ビームサーチ等で使用する
struct State;
// 問題情報(初期盤面, 最終顔面, 一般抜き型など)を保持しており問題情報とそれに関する機能の提供を行う
class BoardTemplate;
// 現在盤面を保持しており型抜きによる状態の遷移などを行う
class Board;

// RequestHandlerクラスがhttpリクエストを行った際に返される値の型
class Response;
// hpptリクエストを行うための基底クラス
class RequstHandler;
// RequstHandlerの派生クラス. 大会サーバとの通信に利用する
class MatchServerHandler;
// RequstHandlerの派生クラス. 解答管理サーバとの通信に利用する
class AnswerManagerHandlear;

// 抜き型のタイプ
enum Type {GENERAL, I, II, III};
// 型抜き時に指定する方向
enum Direction {UP, DOWN, LEFT, RIGHT};

// 列挙隊の標準出力用の演算子オーバーロード
std::ostream& operator<<(std::ostream& os, const Type& type);
std::ostream& operator<<(std::ostream& os, const Direction& direction);

//  ビット演算に関する関数
namespace bit {
    // ビットカウント関数
    int bits_count(int n);
    //　2^nの値を返す(nは必ず非負整数にする)
    int power2_bitwise(int n);
    // |log_2(n)| (||は切り捨て)の値を返す(nは必ず正整数にする)
    int log2_bitwise(int n);
    // 2のべき乗数または0の場合にtrueを返す(nは必ず非負整数にする)
    bool is_power_of_2(int n);
    // n以上で最小の2のべき乗数を返す(nは必ず非負整数にする)
    int upper_next_power_of_2(int n);
    // n以下で最大の2のべき乗数を返す(nは必ず非負整数にする)
    int downer_next_power_of_2(int n);
}

//////////////////////////////////////////////////////

// インライン関数の定義
namespace bit {
    inline int power2_bitwise(int n) {
        [[assume(n >= 0)]];
        return 1 << n;
    }

    inline bool is_power_of_2(int n) {
        [[assume(n >= 0)]];
        return (n & (n - 1)) == 0;
    }
}

// テンプレート関数の定義

#endif