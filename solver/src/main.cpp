#include <iostream>
#include <string>
#include <limits>

#include "common.hpp"
#include "inherited_handlers.hpp"
#include "board_template.hpp"
#include "board.hpp"
#include "operation.hpp"
#include "point_xy.hpp"
#include "beam_node.hpp"
#include "solver.hpp"
#include "test.hpp"
#include "communication.hpp"

#include "json.hpp"
#include "font.hpp"

using namespace std;


// 通信用定数
namespace config {
    // 通信のインターバル[ms]
    const int INTERVAL_TIME = 500;
    // 通信の最高リクエスト回数
    const int MAX_REQEST = 200;

    // 大会サーバのIP
    const string MATCH_SERVER_URL = "http://192.168.1.140:3000";
    // 大会サーバと通信する際のトークン
    const string MATCH_SERVER_TOKEN = "ariake90ac96700a7e0f8208caad8bbcc7a8efebf4703ec4a1b9560be92f14b4";

    // 解答管理サーバーのIP
    const string ANSWER_MANAGER_URL = "http://192.168.1.140:8080";
    // 解答管理サーバーと通信する際のトークン
    const string ANSWER_MANAGER_TOKEN = "keita0723";
}

// Board solve_beam(const BoardTemplate& board_template) {
//     Board board(board_template);
//     for (int y = 0; y < board_template.get_height(); y++) {
//         for (int x = 0; x < board_template.get_width(); x++) {
//             if (board.is_match(x, y)) {
//                 continue;
//             } else {
//                 solver::beam_serch(board, {x, y}, 5, 10, 10);
//             }
//         }
//     }
//     return board;
// }

Board solve_greedy(const BoardTemplate& board_template) {
    Board board(board_template);
    for (int y = 0; y < board_template.get_height(); y++) {
        for (int x = 0; x < board_template.get_width(); x++) {
            if (board.is_match(x, y)) {
                continue;
            } else {
                solver::greedy(board, {x, y}, 25);
            }
        }
    }
    return board;
}

// int main() {
//     int seed = 42;
//     int width = 100;
//     int height = 100;
//     // auto [start, goal]= test::problem::generate_diagonal_boards(width, height, seed);
//     // auto general = test::problem::generate_random_general(1, seed, 10);

//     // BoardTemplate board_template(width, height, start, goal, general);
//     // for (int i = 0; i < 8; i++) {
//     //     auto transformed = board_template.transform(i);
//     //     auto solution = solve_greedy(transformed);
//     //     cout << transformed.get_transform_code() << " : " << solution.get_count() << endl;
//     // }
//     auto [count_beam, time_beam] = test::evaluate::measure(width, height, 1, 42, solve_beam);
//     cout << "Beam : " << count_beam << ", " << time_beam << endl;
//     auto [count_greedy, time_greedy] = test::evaluate::measure(width, height, 1, 42, solve_greedy);
//     cout << "Greedy : " << count_greedy << ", " << time_greedy << endl;
//     return 0;
// }

// red : エラー, 異常
// brue : 解答を求める部分での成功など
// green : 解答を求める部分での成功など
// 通信関係はスペース4つのインデント

int main() {
    // 問題の取得
    auto problem = comm::get_problem(config::MATCH_SERVER_URL, config::MATCH_SERVER_TOKEN, config::INTERVAL_TIME, config::MAX_REQEST);
    BoardTemplate board_template(problem);
    
    // 最小の手数を無限大で初期化
    int min_moves = numeric_limits<int>::max();
    
    for (int i = 0; i < 8; i++) {
        // ボードを変形して解く
        cout << "Tranform -> " << i << " ... ";
        auto transformed = board_template.transform(i);
        auto solution = solve_greedy(transformed);
        auto solution_json = solution.dump();
        auto solution_count = solution.get_count();

        // 解答のチェック
        if (int legal_code = checker::legal_check(board_template, solution_json)) {
            cout << font::red << "Illegal actions (code " << legal_code  << ")" << font::reset << endl;
        // 解答の手数が最小値を更新しているかチェック
        } else if (min_moves <= solution_count) {
            cout << font::blue << "Moves not updated" << font::color_reset << endl;
        // 最小手数を更新した場合 -> 解答の送信へ
        } else {
            cout << font::blue << "success !!" << font::color_reset << endl;
            font::underline();
            cout << font::blue << "Moves updated with " << solution_count << font::color_reset << endl;
            font::reset();
            min_moves = solution_count;
            // 解答の送信
            int success = comm::post_answer_to_server(config::MATCH_SERVER_URL, config::MATCH_SERVER_TOKEN, solution_json, config::INTERVAL_TIME, config::MAX_REQEST);
            // int success = comm::post_answer_to_manager(config::ANSWER_MANAGER_URL, config::ANSWER_MANAGER_TOKEN, solution_json, config::INTERVAL_TIME, config::MAX_REQEST);
        
        }
    }
    return 0;
}