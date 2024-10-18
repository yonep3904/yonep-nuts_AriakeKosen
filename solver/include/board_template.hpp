#ifndef BOARD_TEMPLATE_HPP
#define BOARD_TEMPLATE_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>

#include "common.hpp"
#include "operation.hpp"

#include "json.hpp"

class BoardTemplate {
private:
    // 一般抜き型の抜き型番号は25番から始まる
    static const int FIXED_TYPE_NUM = 25;
    
private:
    // ボードの横ピース数
    int width;
    // ボードの縦ピース数
    int height;
    // 初期盤面の状態
    Vector2D start;
    // 最終盤面の状態
    Vector2D goal;

    // 一般抜き型の数
    int n;
    // 各一般抜き型の横セル数
    std::vector<int> general_width;
    // 各一般抜き型の縦セル数
    std::vector<int> general_height;
    // 各一般抜き型
    std::vector<Vector2D> general_cells;

    // 変形コード
    int transform_code;
    std::map<Type, Type> type_dict;
    std::map<Direction, Direction> direction_dict;

private:
    // サーバから送られる["1111", "1234", ....](文字列の配列)の形式の盤面状態をVector2D(2次元配列)の形式に整形 
    Vector2D _to_board(const std::vector<std::string>& json_obj);
    // サーバから送られる[["1111", "1010", ....], ["0000", ....], ...](文字列の2次元配列)の形式の盤面状態をvector<Vector2D>(3次元配列)の形式に整形
    std::vector<Vector2D> _to_general_cells(const std::vector<nlohmann::json>& json_obj);
    // コンストラクタで行われる処理
    void init();

public:
    // コンストラクタ(空コンストラクタ) 
    BoardTemplate();
    // コンストラクタ(JSON形式のデータをもとに生成) -> 本番はこっちを使う
    BoardTemplate(const nlohmann::json& json_text);
    // コンストラクタ(数値や2次元配列などをもとに生成) -> デバッグ・練習の時はこっちが扱いやすいからこっちを使う
    BoardTemplate(int width, int height, const Vector2D start, const Vector2D goal, const std::vector<Vector2D> general);
    // transformメソッドによる生成の際に用いる。
    BoardTemplate(int width, int height, const Vector2D&& start, const Vector2D&& goal, const std::vector<Vector2D>&& general, int transform_code);
    // ボードを回転または転置させたBoardTemplateを出力
    BoardTemplate transform(int code) const;

    // ここからメンバ変数のゲッター
    // ボードの横ピース数を取得
    const int get_width() const;
    // ボードの縦ピース数を取得
    const int get_height() const;
    // 初期盤面の状態を取得(参照)
    const Vector2D& get_start() const;
    // 最終盤面の状態を取得(参照)
    const Vector2D& get_goal() const ;
    // 最終盤面の座標(x, y)上のピースを取得
    const PieceType get_goal_at(int x, int y) const;

    // 一般抜き型の数を取得
    const int get_n() const;
    // 一般抜き型pの横セル数を取得
    const int get_general_width(int p) const;
    // 一般抜き型pの縦セル数を取得
    const int get_general_height(int p) const;
    // 一般抜き型pを取得(参照)
    const Vector2D& get_general_cells(int p) const;
    // 一般抜き型pの座標(x, y)上のセルを取得
    const PieceType get_general_cells_at(int p, int x, int y) const;
    // ()演算子オーバーロード -> get_goal_at(int x, int y)と同じ
    const PieceType operator()(int x, int y) const;

    // 変形コードを取得
    const int get_transform_code() const;
    // ゲッターここまで

    // 問題情報の標準出力
    void print() const;
    // 問題情報の標準出力(カラー付き, Windowsのみ対応)
    void print_color() const;

    // 以下 type(Type型)は抜き型のタイプ(I, II, III, 一般のどれか), size(int型)は定型抜き型の場合は抜き型のサイズ(1,2,4...),一般抜き型の場合は一般抜き型番号
    //　このプログラム上では一般抜き型番号は0から始まる数字で管理している.25でないことに注意

    // type-sizeの抜き型を座標(x0, y0)に適用させた場合の型抜き対象ピースの左端を取得
    const int left_limit(Type type, int size, int x0, int y0) const;
    // type-sizeの抜き型を座標(x0, y0)に適用させた場合の型抜き対象ピースの右端を取得
    const int right_limit(Type type, int size, int x0, int y0) const;
    // type-sizeの抜き型を座標(x0, y0)に適用させた場合の型抜き対象ピースの上端を取得
    const int up_limit(Type type, int size, int x0, int y0) const;
    // type-sizeの抜き型を座標(x0, y0)に適用させた場合の型抜き対象ピースの下端を取得
    const int down_limit(Type type, int size, int x0, int y0) const;

    // 型抜き操作に必要な値(type, size, x, y)をもとに関数を生成する(関数を生成する関数であることに注意)
    //  -> どんな関数? : 座標(x, y)のピースはその型抜きによって抜かれるか判定(true/false)
    const std::function<bool(int, int)> judge_func(Type type, int size, int x0, int y0) const;
    const std::function<int(int)> line_count(Type type, int size, int x0, int y0) const;
    const std::function<int(int)> row_count(Type type, int size, int x0, int y0) const;
    
    // Operationオブジェクトをtransform_code = 0 に対応するOperationオブジェクトに変換する
    const Operation inverse(const Operation& operation) const;

    // 抜き型のタイプとサイズ(または0始まりの一般抜き型番号)から抜き型番号を取得する
    static int to_p_number(Type type, int size);
    // 抜き型番号から抜き型のタイプとサイズ(または0始まりの一般抜き型番号)を取得する
    static std::pair<Type, int> to_type_size(int p);
};

inline const int BoardTemplate::get_width() const {
    return width;
}

inline const int BoardTemplate::get_height() const {
    return height;
}

inline const Vector2D& BoardTemplate::get_start() const {
    return start;
}

inline const Vector2D& BoardTemplate::get_goal() const {
    return goal;
}

inline const PieceType BoardTemplate::get_goal_at(int x, int y) const {
    return goal[y][x];
}

inline const int BoardTemplate::get_n() const {
    return n;
}

inline const int BoardTemplate::get_general_width(int p) const {
    return general_width[p];
}

inline const int BoardTemplate::get_general_height(int p) const {
    return general_height[p];
}

inline const Vector2D& BoardTemplate::get_general_cells(int p) const {
    return general_cells[p];
}

inline const PieceType BoardTemplate::get_general_cells_at(int p, int x, int y) const {
    return general_cells[p][y][x];
}

inline const PieceType BoardTemplate::operator()(int x, int y) const {
    return goal[y][x];
}

inline const int BoardTemplate::get_transform_code() const {
    return transform_code;
}

inline const int BoardTemplate::left_limit(Type type, int size, int x0, int y0) const {
    return std::max(0, x0);
}

inline const int BoardTemplate::right_limit(Type type, int size, int x0, int y0) const {
    return std::min(width, x0 + (type == GENERAL ? general_width[size] : size)) - 1;
}

inline const int BoardTemplate::up_limit(Type type, int size, int x0, int y0) const {
    return std::max(0, y0);
}

inline const int BoardTemplate::down_limit(Type type, int size, int x0, int y0) const {
    return std::min(height, y0 + (type == GENERAL ? general_height[size] : size)) - 1;
}

#endif
