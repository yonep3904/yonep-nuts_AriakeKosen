# solver

## フォルダ/ファイル構成
- `include/` : ヘッダファイル(関数/クラスの宣言のみ)
- `lib/` : 外部ライブラリ
- `obj/` : のオブジェクトファイル(コンパイル済みのファイル)
- `src/` : ソースファイル
- `test_data/` : 時間の計測などで使用したファイルや記録
- `yonep-nuts.exe` : 実行ファイル
- `Makefile` : コマンド`make`を実行した際の処理が記述されている

## ヘッダ/ソースファイル構成
- `board_template.hpp/cpp` : `BoardTemplate`クラスを記述
- `board.hpp/cpp` : `Board`クラスを記述
- `common.hpp/cpp` : クラス/構造体の前方宣言と全てのファイルで使用する変数や関数を記述
- `inherited_handlers.hpp/cpp` : `MatchServerHandler`クラスと`AnswerManagerHandlear`クラスを記述
- `operation.hpp` : `Operation`構造体を記述
- `point_xy.hpp` : `PointXY`構造体を記述
- `request_handler.hpp/cpp` : `RequestHandler`クラスと`Responce`クラスを記述
- `solver.hpp/cpp` : 問題を解くための関数が記述
- `beam_node.hpp` : `beam_node`構造体を記述
- `test.hpp/cpp` : プログラムのテストや時間/手数の計測などを行う関数を記述

## 共通のエイリアス/列挙体
```cpp
// ピースとセルの型名のエイリアス
// 本番時はint -> std::int_fast8_t に変更すること
using PieceType = int;
// ボードと抜き型に用いる2次元配列のエイリアス
using Vector2D = std::vector<std::vector<PieceType>>;

// 抜き型のタイプ
enum Type {GENERAL, I, II, III};
// 型抜き時に指定する方向
enum Direction {UP, DOWN, LEFT, RIGHT};
```
## クラス/構造体の説明
### `BoardTemplate`クラス
問題情報を管理する。
そのほか、盤面を回転して新しい`BoardTemplate`オブジェクトを作成することが可能。

- `BoardTemplate(int width, int height, const Vector2D start, const Vector2D goal, const std::vector<Vector2D> general)` : コンストラクタ
    - width : 横幅
    - height : 縦幅
    - start : 初期盤面
    - goal : 最終盤面
    - general : 一般抜き型
- `BoardTemplate transform(int code)` : 盤面を回転・反転した新しい`BoardTemplate`オブジェクトを返す
    - `code` : `0b000` - `0b111`までの3ビット整数値を入力.上位1ビットが反転のフラグ,下位2ビット*90°回転。
- `print_color()` : 基本的な盤面情報の表示(標準出力)

```cpp
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
```

### `Board`クラス
BoardTemplateクラスから生成する。
現在の盤面情報を管理する。
- `Board(const BoardTemplate& board_template)` : コンストラクタ
    - `board_template` : 生成したい問題情報を持つ`BoardTemplate`オブジェクト
- `operator()(int x, int y)` : 現在盤面の座標(x, y)のピースを取得
- `bool is_match(int x, int y)` : 座標(x, y)が揃っているか判定
- `bool is_goal()` : 全てのピースが揃っているか判定
- `bool operate(Type type, int size, int x0, int y0, Direction direction)` : 型抜き操作を行う
    - `type` : 抜き型のタイプ(`I`, `II`, `III`, `GENERAL`)
    - `size` : 定型抜き型の場合は型のサイズ.一般抜き型の場合は1から始まる一般抜き型通し番号.有効な型抜きが行われればtrueを返す
    - `bool back(int n)` : **n手**戻る.nが不適切な値の場合(負数など)falseを返す
    - `bool back_to(int n)` : **n手目まで**戻る.nが不適切な値の場合(負数など)falseを返す
    - `nlohmann::json dump(bool inverse = true)` : 今までの操作をJSONオブジェクトとして返す.
        - `inverse` : `BoardTemplate::translate()`関数によって生成された`BoardTemplate`オブジェクトから生成された`Board`オブジェクトは`transform_code` = 0 に対応する型抜き操作に変換して出力する.

```cpp
class Board {
private:
    // 問題情報を保持するBoardTemplateオブジェクトの参照
    const BoardTemplate& board_template;
    // ボードの横ピース数
    const int width;
    // ボードの縦ピース数
    const int height;

    // 現在の盤面の状態
    Vector2D beam_node;
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
```

### `Operation`構造体
型抜き操作の構造体。
抜き型のタイプ/定型抜き型のサイズ(または一般抜き型の通し番号)/適用x座標/適用y座標/方向の情報を持つ
`std::cout` への標準出力が可能
```cpp
struct Operation {
    // 抜き型のタイプ
    Type type;
    // 定型抜き型の場合は抜き型のサイズ(一般抜き型の場合は通し番号)
    int size;
    // 適用x座標
    int x;
    / 適用y座標
    int y;
    // 寄席方向
    Direction direction;

    // コンストラクタ
    Operation(Type type, int size, int x, int y, Direction direction);
    Operation(int type, int size, int x, int y, int direction);

    // 演算子オーバーロード(標準出力時に利用)
    friend std::ostream& operator<<(std::ostream& os, const Operation& ope);
};
```

### `PointXY`構造体
2次元座標の構造体。
- `std::cout` への標準出力が可能
- `PointXY`同士の比較(`==`, `!=`)が可能
- `PointXY`同士の減算(`-`)で2つの座標の差(絶対値)を持つ`PointXY`になる

```cpp
struct PointXY {
    int x;
    int y;

    // コンストラクタ
    PointXY();
    PointXY(int x, int y);

    // 比較(等号, 否定等号)が可能
    bool operator==(const PointXY& other) const;
    bool operator!=(const PointXY& other) const;
    // PointXY同士の引き算 -> 距離を出力
    PointXY operator-(const PointXY& other) const;
    // 標準出力時に利用
    friend std::ostream& operator<<(std::ostream& os, const PointXY& p);
};
```

### `beam_node`構造体
評価値と移動元座標の構造体。
ビームサーチ等で使用。
- `std::cout` への標準出力が可能
- `beam_node`同士の比較(`==`, `!=`, `<`, `<=`, `>`, `>=`)で評価値の比較が可能

```cpp
struct beam_node {
    // 評価値
    int score;
    // 移動元座標
    PointXY source;

    // コンストラクタ
    beam_node();
    beam_node(int score, PointXY source);
    beam_node(int score, PointXY&& source);
    beam_node(int score, int x, int y);

    // scoreの比較(等号, 否定等号, 不等号)が可能
    bool operator==(const beam_node& other) const;
    bool operator!=(const beam_node& other) const;
    bool operator<(const beam_node& other) const;
    bool operator<=(const beam_node& other) const;
    bool operator>(const beam_node& other) const;
    bool operator>=(const beam_node& other) const;

    // 標準出力時に利用
    friend std::ostream& operator<<(std::ostream& os, const PointXY& p);
};
```

## 関数の説明
### `bit`名前空間
2のべき乗判定など、2進数関連の処理を高速に行う関数を持つ
入力の範囲(括弧内に記述)が適切でないと未定義動作を起こすことに注意
```cpp
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
```

### `checker`名前空間
解答(JSON形式)が有効な回答か、最終盤面と一致しているか確認するための関数を持つ
```cpp
namespace checker {
    // 解答が本当に有効な解答か確認する(返り値0が有効な回答)
    int legal_check(const BoardTemplate& board_template, const nlohmann::json& answer);
    int legal_check(const BoardTemplate& board_template, const std::string& answer);

    // legal_checkによる返り値のエラーコード
    extern std::map<int, std::string> legal_check_map;
}
```

### `solver`名前空間
ピースを揃えるための関数を持つ。
- `finder`名前空間 : ピースを見つける関数
- `mover`名前空間 : 見つけたピースを型抜き操作により揃える
- `evaluater`名前空間 : 評価関数

```cpp
namespace solver {
    // 指定した座標のピースを揃える
    void simple(Board& board, const PointXY& destination);
    void greedy(Board& board, const PointXY& destination, int lim);
    void beam_serch(Board& board, const PointXY& destination, int lim, int width, int depth);
    void chokudai_serch(Board& board, const PointXY& destination, int lim, int width, int depth);
    
    // 指定した座標source上の値のと同じ値を持つピースを見つける
    namespace finder {
        // 優先順位 : 同列-2べき(1) > 同列-その他(2) > 同行-2べき(1) > 同行-その他(2) > その他(3)
        std::vector<PointXY> line_priority(const Board& board, const PointXY& finding, int lim);
        std::vector<PointXY> line_priority(const Board& board, const PointXY& finding);

        // 優先順位 : y軸方向の距離のビットカウントカウントが少ない行から(1+p(dy)) > 同行2べき(1) > 同行その他(p(dx))
        std::vector<PointXY> bit_count_priority(const Board& board, const PointXY& finding, int lim);
        std::vector<PointXY> bit_count_priority(const Board& board, const PointXY& finding);
    }

    // 指定した座標sourceから指定した座標destinationまでピースを移動させる
    namespace mover {
        // タイプIの型を使用 -> p(dy) + 1手で行うことができる
        void use_typeI(Board& board, const PointXY& source, const PointXY& destination);
        // タイプI, II, IIIの型を使用 -> 全ての移動が3手以内に行うことができる
        void use_all(Board& board, const PointXY& source, const PointXY& destination);
    }

    // 評価関数
    namespace evaluater {
        // 評価値の型
        using ScoreType = int;
        // 評価値の上限(無限大)
        const ScoreType INF = std::numeric_limits<PieceType>::max();

        // 評価関数(左上からの一致ピースの連結成分数).座標を指定することによる高速化
        ScoreType count_connected(const Board& board, const PointXY& start);
    }
}
```

### `test`名前空間
プログラムのテストや時間/手数の計測などを行う関数を持つ。
- `problem`名前空間 : テスト用の問題の生成を行う関数
- `operation`名前空間 : `Board`オブジェクトに対してランダムな型抜き操作を行う関数
- `evaluat`名前空間 : 手数/時間の計測を行いファイル等に記録する関数
- `comm`名前空間 : 通信のテストを行う関数
```cpp
namespace test {

    // 問題をランダムに生成する
    namespace problem {
        // ピースの種類
        const int PIECE_VARIANT_NUM = 4;

        // 非決定的な乱数のシード値を生成
        int random_seed();
        // ランダムな整数を返す
        int randint(int min, int max, int seed);

        // 縦縞模様のボードを生成
        Vector2D vertical_board(int width, int height);
        // 横縞模様のボードを生成
        Vector2D horizontal_board(int width, int height);
        // 斜め縞模様のボードを生成
        Vector2D diagonal_board(int width, int height);
        // ランダムにボードを生成
        Vector2D random_board(int width, int height, int seed);
        // ボードをシャッフル
        Vector2D shuffle_board(const Vector2D& board, int seed);

        // 縦縞模様の初期盤面と最終盤面のペアを生成する
        std::tuple<Vector2D, Vector2D> generate_vertical_boards(int width, int height, int seed);
        // 横縞模様の初期盤面と最終盤面のペアを生成する
        std::tuple<Vector2D, Vector2D> generate_horizontal_boards(int width, int height, int seed);
        // 斜め縞模様の初期盤面と最終盤面のペアを生成する
        std::tuple<Vector2D, Vector2D> generate_diagonal_boards(int width, int height, int seed);
        // ランダムな初期盤面と最終盤面のペアを生成する
        std::tuple<Vector2D, Vector2D> generate_random_boards(int width, int height, int seed);

        // ランダムに一般抜き型を生成
        std::vector<Vector2D> generate_random_general(int num, int seed, int size_lim = 256);
    }

    // 型抜き操作をランダムに生成する
    namespace operation {
        // ランダムに型抜き操作の引数を生成する
        std::vector<Operation> random_operations(int len, int width, int height, int num, int seed);
        std::vector<Operation> random_operations(int len, BoardTemplate& board_template, int seed);
    }

    // ソルバーの評価を行う
    namespace evaluate {
        std::tuple<int, double> measure(int width, int height, int num, int seed, std::function<Board(BoardTemplate&)> solver);
        void evaluate(std::function<Board(BoardTemplate&)> solver, std::string solver_name, int seed, int one_step = 5);
    }

    // 通信を用いたテスト
    namespace comm {
        void direct_send(std::function<Board(BoardTemplate&)> solver);
    }
    
}
```

## 使用例
```cpp

#include <iostream>

using namespace std;

int main() {
    // シード値
    int seed = 42;
    // 盤面サイズ
    int width = 100;
    int height = 120;
    // 初期盤面・最終盤面を生成
    auto [start, goal] = test::problem::generate_diagonal_boards(width, height, seed);
    // 一般抜き型をランダムに生成
    auto general = test::problem::generate_random_general(10, seed, 100);
    
    // インスタンス化
    BoardTemplate board_template(width, height, start, goal, general);
    Board board(board_template);

    // 型抜き操作を行い、盤面を完成させる
    board.operate(I, 2, 1, 0, LEFT);
    board.operate(II, 32, 52, 33, UP);

    // 盤面が完成した場合、解答(JSON)を標準出力
    if (board.is_goal()) {
        auto solution = board.dump();
        cout << solution << endl;
    }

}
```

## 命名規約
### 基本的な命名規約
- **関数名・変数名・ファイル名** : スネークケース -> `my_func`, `solve`...
- **グローバル/ローカル定数名(列挙体含む)** : 大文字のスネークケース -> `MY_CONST`, `TOKEN`...
- **クラス名・型名エイリアス** : パスカルケース(`MyClass`, `BoardTemplate`...)
- **名前空間** : スネークケース(できるだけ短く) -> `my_name_space`, `problem`...
- **インクルードガードのためのマクロ** : ヘッダーファイル名を大文字のスネークケースで+`_HPP` -> `MY_HEADER_HPP`(ファイル名`my_header.hpp`の場合)
- **その他(マクロなど)** : 基本的には大文字のスネークケース

### メンバ関数等
- privateな関数は先頭にアンダースコア`_`をつける -> `_my_func`, `_to_board`...
- publicな関数は通常の関数と同じように記述
- ゲッター関数は`get_xxxx`の形にする(原則xxxxには取得するメンバ変数名) -> get_width(メンバ変数widthを取得する関数)
- セッター関数は`set_xxxx`形にする(原則xxxxには取得するメンバ変数名) -> get_width(メンバ変数widthを変更する関数)

### その他
- 何かを判定する(返り値がbool)の関数は`is_xxxx`(xxxxであるか判定)や`can_xxxx`(xxxxができるか判定), `has_xxxx`(xxxxを持つか判定)などがよく用いられる
