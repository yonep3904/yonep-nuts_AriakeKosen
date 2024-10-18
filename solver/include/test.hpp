#ifndef TEST_HPP
#define TEST_HPP

#include <vector>
#include <tuple>
#include <random>
#include <functional>
#include <stdexcept>

#include "common.hpp"
#include "board_template.hpp"
#include "operation.hpp"

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

#endif
