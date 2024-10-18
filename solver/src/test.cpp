#include "test.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <functional>
#include <utility>

#include "common.hpp"
#include "board_template.hpp"
#include "board.hpp"
#include "operation.hpp"
#include "inherited_handlers.hpp"
#include "solver.hpp"

#include "timer.hpp"

using namespace std;

namespace {
    template<class T>
    vector<vector<T>> reshape_2D(const vector<T>& flatten, int rows, int cols) {
        if (flatten.size() != rows*cols) {
            throw invalid_argument("flatten vector elements do not match reshape dimensions.");
        }

        vector<vector<T>> result;
        result.reserve(cols);
        for (int i = 0; i < cols; i++) {
            vector<T> tmp;
            tmp.reserve(rows);
            for (int j = 0; j < rows; j++) {
                tmp.push_back(flatten[i*cols + j]);
            }
            result.push_back(move(tmp));
        }
        return result;
    }

    vector<tuple<int, int>> random_size(int min, int max, int num, int seed) {
        mt19937 rng(seed);
        vector<int> width;
        vector<int> height;
        width.reserve(max - min + 1);
        height.reserve(max - min + 1);
        for (int i = min; i <= max; i++) {
            width.push_back(i);
            height.push_back(i);
        }
        shuffle(width.begin(), width.end(), rng);
        shuffle(height.begin(), height.end(), rng);

        vector<tuple<int, int>> result;
        result.reserve(num);
        for (int i = 0; i < num; i++) {
            result.emplace_back(width[i], height[0]);
        }
        return result;
    }

    vector<tuple<int, int>> square_size_random(int min, int max, int num, int seed) {
        mt19937 rng(seed);
        int range = (max - min + 1) / num;
        uniform_int_distribution<PieceType> dist(0, range - 1);
        
        vector<tuple<int, int>> result;
        result.reserve(num);
        for (int i = 0; i < num; i++) {
            int size = range*i + dist(rng);
            result.emplace_back(size, size); 
        }
        return result;
    }

    vector<tuple<int, int>> square_size(int min, int max, int step) {
        vector<tuple<int, int>> result;
        if ((max - min)%step == 0) {
            result.reserve((max - min)/step + 1);
            for (int size = min; size <= max; size += step) {
                result.emplace_back(size, size); 
            }
        } else {
            result.reserve((max - min)/step + 1 + 1);
            for (int size = min; size <= max; size += step) {
                result.emplace_back(size, size); 
            }
            result.emplace_back(max, max);
        }
        return result;
    }
}

namespace test::problem {

    int random_seed() {
        static random_device rd;
        return rd();
    }

    int randint(int min, int max, int seed) {
        mt19937 gen(seed); 
        uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

    Vector2D vertical_board(int width, int height) {
        Vector2D result(height, vector<PieceType>(width));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                result[i][j] = j % PIECE_VARIANT_NUM;
            }
        }
        return result;
    }

    Vector2D horizontal_board(int width, int height) {
        Vector2D result(height, vector<PieceType>(width));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                result[i][j] = i % PIECE_VARIANT_NUM;
            }
        }
        return result;
    }

    Vector2D diagonal_board(int width, int height) {
        Vector2D result(height, vector<PieceType>(width));
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                result[i][j] = (i+j) % PIECE_VARIANT_NUM;
            }
        }
        return result;
    }

    Vector2D random_board(int width, int height, int seed) {
        mt19937 rng(seed);
        int fix_piece_num = width*height / 10 + 1;
        int random_piece_num = width*height - PIECE_VARIANT_NUM*fix_piece_num;

        // 1次元(flatten化)で盤面を生成
        vector<PieceType> board_flatten;
        board_flatten.reserve(width * height);
        for (int i = 0; i < fix_piece_num; i++) {
            for (PieceType j = 0; j < PIECE_VARIANT_NUM; j++) {
                board_flatten.push_back(j);
            }
        }
        uniform_int_distribution<PieceType> dist(0, PIECE_VARIANT_NUM - 1);
        for (int i = 0; i < random_piece_num; i++) {
            board_flatten.push_back(dist(rng));
        }
        shuffle(board_flatten.begin(), board_flatten.end(), rng);

        // 1次元の盤面を2次元配列に変換
        Vector2D board = reshape_2D(board_flatten, width, height);
        return board;
    }

    Vector2D shuffle_board(const Vector2D& board, int seed) {
        mt19937 rng(seed);
        int width = board[0].size();
        int height = board.size();

        vector<pair<int, int>> indices;
        indices.reserve(width*height);
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                indices.emplace_back(x, y);
            }
        }
        shuffle(indices.begin(), indices.end(), rng);

        Vector2D result(height, vector<PieceType>(width));
        int cnt = 0;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                result[y][x] = board[indices[cnt].second][indices[cnt].first];
                cnt++;
            }
        }
        return result;
    }

    tuple<Vector2D, Vector2D> generate_vertical_boards(int width, int height, int seed) {
        Vector2D goal = vertical_board(width, height);
        Vector2D start = shuffle_board(goal, seed);
        return make_tuple(start, goal);
    }

    tuple<Vector2D, Vector2D> generate_horizontal_boards(int width, int height, int seed) {
        Vector2D goal = horizontal_board(width, height);
        Vector2D start = shuffle_board(goal, seed);
        return make_tuple(start, goal);
    }

    tuple<Vector2D, Vector2D> generate_diagonal_boards(int width, int height, int seed) {
        Vector2D goal = diagonal_board(width, height);
        Vector2D start = shuffle_board(goal, seed);
        return make_tuple(start, goal);
    }

    tuple<Vector2D, Vector2D> generate_random_boards(int width, int height, int seed) {
        Vector2D goal = random_board(width, height, seed);
        Vector2D start = shuffle_board(goal, seed);
        return make_tuple(start, goal);
    }

    vector<Vector2D> generate_random_general(int num, int seed, int size_lim) {
        mt19937 rng(seed);
        uniform_int_distribution<int> size_dist(1, size_lim);
        uniform_int_distribution<PieceType> cell_dist(0, 1);
        vector<Vector2D> result;
        result.reserve(num);
        for (int i = 0; i < num; i++) {
            int width = size_dist(rng);
            int height = size_dist(rng);
            Vector2D tmp2d;
            tmp2d.reserve(height);
            for (int j = 0; j < height; j++) {
                vector<PieceType> tmp1d;
                tmp1d.reserve(width);
                for (int k = 0; k < width; k++) {
                    tmp1d.push_back(cell_dist(rng));
                }
                tmp2d.push_back(move(tmp1d));
            }
            result.push_back(move(tmp2d));
        }
        return result;
    }

}

namespace test::operation {
    vector<Operation> random_operations(int len, int width, int height, int num, int seed) {
        mt19937 rng(seed);
        uniform_int_distribution<int> type_dist(num == 0 ? 1 : 0, 3);
        uniform_int_distribution<int> size1_dist(0, 8);
        uniform_int_distribution<int> size23_dist(1, 8);
        uniform_int_distribution<int> size0_dist(0, num - 1);
        uniform_int_distribution<int> x_dist(0, width - 1);
        uniform_int_distribution<int> y_dist(0, height - 1);
        uniform_int_distribution<int> dirction_dist(0, 3);

        vector<Operation> result;
        result.reserve(len);

        for (int i = 0; i < len; i++) {
            int type = type_dist(rng);
            int size;
            switch (type) {
            case GENERAL:
                size = size0_dist(rng);
                break;
            case I:
                size = bit::power2_bitwise(size1_dist(rng));
                break;
            case II:
            case III:
                size = bit::power2_bitwise(size23_dist(rng));
                break;
            }
            result.emplace_back(type, size, x_dist(rng), y_dist(rng), dirction_dist(rng));
        }
        return result;
    }

    vector<Operation> random_operations(int len, BoardTemplate& board_template, int seed) {
        int width = board_template.get_width();
        int height = board_template.get_height();
        int num = board_template.get_n();
        return random_operations(len, width, height, num, seed);
    }

}

namespace test::evaluate {

    tuple<int, double> measure(int width, int height, int num, int seed, function<Board(BoardTemplate&)> solver) {
        auto [start, goal] = test::problem::generate_diagonal_boards(width, height, seed);
        auto general = test::problem::generate_random_general(num, seed, width);
        BoardTemplate board_template(width, height, start , goal, general);

        Timer timer;
        timer.start();
        auto board = solver(board_template);
        timer.stop();

        if (board.is_goal()) {
            return make_tuple(board.get_count(), timer.elapsed_milliseconds());    
        } else {
            return make_tuple(-1, timer.elapsed_milliseconds());
        }
    }

    void evaluate(function<Board(BoardTemplate&)> solver, string solver_name, int seed, int one_step) {
        // 一般抜き型の数
        static int num = 10;
        mt19937 rng(seed);
        uniform_int_distribution<int> dist(0, 1000000);

        
        stringstream filename;
        filename << "test_data/" << solver_name << "_" << seed << ".csv";
        ofstream file(filename.str());
        file << "width,height,seed,count,time" << endl;
        for (const auto e: square_size(32, 256, 10)) {
            auto [width, height] = e;
            cout << "[width, height] = [" << width << ", " << height << "]" << endl;
            for (int i = 0; i < one_step; i++) {
                int seed_ = dist(rng);
                auto [count, time] = measure(width, height, num, seed_, solver);
                file << width << "," << height << "," << seed_ << "," << count << "," << time << endl;

                cout << "   [count, time] = [" << count << ", " << time << "]" << endl;
            }
        }
    }

}

namespace test::comm {
    // void direct_send(function<Board(BoardTemplate&)> solver) {
    //     MatchServerHandler problem_server(CommConfig::PROBLEM_SERVER_URL, CommConfig::PROBLEM_SERVER_TOKEN);
    //     Response response;
    //     // 問題の取得
    //     while(true) {
    //         response = problem_server.Get_problem();
    //         cout << response.status() << endl;
    //         if (response.status() == 200) {
    //             cout << "Get problem successfull!!" << endl;
    //             break;
    //         }
    //         wait_millisec(CommConfig::INTERVAL_TIME);
    //     }

    //     // 問題の読み込みと表示
    //     BoardTemplate board_template(response.to_json());
    //     // board_template.print_color();
        
    //     // 解答を求める
    //     Board board = solver(board_template);
    //     string ans = board.dump();

    //     // 解答のチェック
    //     int legal_check_code = checker::legal_check(board_template, ans);
    //     cout << "legal_check code : " << legal_check_code << endl;
    //     // cout << "JSON : " << ans.dump(4) << endl;

    //     // 解答の提出
    //     response = problem_server.Post_answer(ans);
    //     cout << response.status() << endl;
    //     if (response.status() == 200) {
    //         cout << "Get problem successfull!!" << endl;
    //     }
    // }
}



