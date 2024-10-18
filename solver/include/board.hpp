#ifndef BOARD_HPP
#define BOARD_HPP

#include <string>
#include <vector>

#include "common.hpp"
#include "board_template.hpp"

#include "json.hpp"

class Board {
private:
    // 問題情報を保持するBoardTemplateオブジェクトの参照
    const BoardTemplate& board_template;
    // ボードの横ピース数
    const int width;
    // ボードの縦ピース数
    const int height;

    // 現在の盤面の状態
    Vector2D state;
    // 現在までに行った型抜き操作の回数
    int count;
    //　現在までに行った型抜き操作の記録
    std::vector<Operation> history;

public:
    // コンストラクタ(BoardTemplateオブジェクトをもとに生成)
    Board(const BoardTemplate& board_template);

    // ここからメンバ変数のゲッター

    // 問題情報を保持するBoardTemplateオブジェクトの取得(参照)
    const BoardTemplate& get_template() const;
    // ボードの横ピース数を取得
    const int get_width() const;
    // ボードの縦ピース数を取得
    const int get_height() const;
    // 現在の盤面の状態の取得
    const Vector2D& get_state() const;
    // 現在の盤面の座標(x, y)上のピースを取得
    const PieceType get_state_at(int x, int y) const;
    // 現在までに行った型抜き操作の回数の取得
    const int get_count() const;
    // 現在までに行った型抜き操作の記録の取得(参照)
    const std::vector<Operation>& get_history() const;
    // ()演算子オーバーロード -> get_state_at(int x, int y)と同じ
    const PieceType operator()(int x, int y) const;
    // ゲッターここまで

    // 現在の盤面情報の標準出力
    void print();
    // 現在の盤面情報の標準出力(カラー付き, Windowsのみ対応)
    void print_color();

    // 座標(x, y)上のピースが揃っているか(最終盤面のピースの数字と一致しているか)判定する
    const bool is_match(int x, int y) const;
    // 全てのピースが揃っているか判定する
    const bool is_goal() const;
    // type-sizeの抜き型を座標(x0, y0)に適用させてdirectionの向きに型抜き操作を行い,盤面状態を遷移させる
    const bool operate(Type type, int size, int x0, int y0, Direction direction);
    // 型抜きの逆操作をn回行うことにより,n回前の盤面の状態まで盤面状態を戻す
    const bool back(int n);
    // 型抜きの逆操作を行うことにより,count=nの盤面の状態まで盤面状態を戻す
    const bool back_to(int n);

    // 現在までに行った型抜き操作の記録をもとに解答用のJSONオブジェクトを生成し返す
    nlohmann::json dump(bool inverse = true) const;

private:
    bool _operate_up(Type type, int size, int x0, int y0);
    bool _operate_down(Type type, int size, int x0, int y0);
    bool _operate_left(Type type, int size, int x0, int y0);
    bool _operate_right(Type type, int size, int x0, int y0);

    void _back_up(Type type, int size, int x0, int y0);
    void _back_down(Type type, int size, int x0, int y0);
    void _back_left(Type type, int size, int x0, int y0);
    void _back_right(Type type, int size, int x0, int y0);  
};

inline const BoardTemplate& Board::get_template() const {
    return board_template;
}

inline const int Board::get_width() const {
    return width;
}

inline const int Board::get_height() const {
    return height;
}

inline const Vector2D& Board::get_state() const {
    return state;
}

inline const PieceType Board::get_state_at(int x, int y) const {
    return state[y][x];
}

inline const int Board::get_count() const {
    return count;
}

inline const std::vector<Operation>& Board::get_history() const {
    return history;
}

inline const PieceType Board::operator()(int x, int y) const {
    return state[y][x];
}

inline const bool Board::is_match(int x, int y) const {
    return state[y][x] == board_template.get_goal()[y][x];
}

inline const bool Board::is_goal() const {
    return state == board_template.get_goal();
}

#endif
