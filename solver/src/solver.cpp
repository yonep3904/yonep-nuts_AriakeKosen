#include "solver.hpp"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <queue>

#include "common.hpp"
#include "board_template.hpp"
#include "board.hpp"
#include "operation.hpp"
#include "point_xy.hpp"
#include "beam_node.hpp"
#include "state.hpp"

#include "font.hpp"

using namespace std;

namespace {
    // vectorを連結する.連結後のvectorのサイズ上限を指定可能(上限を超えた分は破棄される).vec2は空になる
    template<class T>
    inline void marge_vector(vector<T>& vec1, vector<T>& vec2, int lim) {
        int remaining_space = lim - vec1.size();
        if (remaining_space > 0) {
            if (remaining_space < vec2.size()) {
                vec1.reserve(lim);
                vec1.insert(vec1.end(), make_move_iterator(vec2.begin()), make_move_iterator(vec2.begin() + remaining_space));
            } else {
                vec1.reserve(vec1.size() + vec2.size());
                vec1.insert(vec1.end(), make_move_iterator(vec2.begin()), make_move_iterator(vec2.end()));
            }
        }
        vec2.clear();
    }

    template<class T>
    inline void marge_vector(vector<T>& vec1, vector<T>& vec2) {
        static int inf = numeric_limits<int>::max();
        marge_vector(vec1, vec2, inf);
    }
}

namespace checker {

    int legal_check(const BoardTemplate& board_template, const nlohmann::json& answer) {
        // 解答のフォーマットチェック
        if (!(answer.size() == 2 && answer.contains("n") && answer.contains("ops") && answer["ops"].is_array())) {
            return 1;
        }

        // 配列のフォーマットチェック(0番目の要素のみチェック)
        auto ops_elem = answer["ops"][0];
        if (!(ops_elem.size() == 4 && ops_elem.contains("p") && ops_elem.contains("x") && ops_elem.contains("y") && ops_elem.contains("s"))) {
            return 2;
        }

        // nの値と配列サイズのチェック
        if (!(answer["n"] == answer["ops"].size())) {
            return 3;
        }

        // 無効な型抜き操作がないかチェック
        Board board(board_template);
        for (const auto& operation : answer["ops"]) {
            int p = operation["p"];
            int x = operation["x"];
            int y = operation["y"];
            int s = operation["s"];

            auto [type, size] = BoardTemplate::to_type_size(p);
            Direction direction = static_cast<Direction>(s);
            bool is_legal = board.operate(type, size, x, y, direction);
            // board.print_color();
            // cout << Operation(type, size, x, y, direction) << endl;
            if (!is_legal) {
                return 4;
            }
        }
        
        // 最終盤面と一致しているかチェック
        if (!board.is_goal()) {
            return 5;
        }

        return 0;
    }

    int legal_check(const BoardTemplate& board_template, const string& answer) {
        return legal_check(board_template, nlohmann::json::parse(answer));
    }

    map<int, string> legal_check_map = {
        {0, u8"有効な回答です"},
        {1, u8"JSONのフォーマットが間違えています"},
        {2, u8"\"opt\"キー内の要素のフォーマットが間違えています"},
        {3, u8"nの値と配列サイズが一致しません"},
        {4, u8"無効な型抜き操作が含まれています"},
        {5, u8"ピースが最終盤面の配列と一致していません"},
    };
}

namespace solver {

    void greedy(Board& board, const PointXY& destination, int num) {
        // cout << destination << endl;
        PointXY best_action = {0, 0};
        evaluater::ScoreType best_score = -evaluater::INF;
        int start = board.get_count();

        auto source_candidate = finder::line_priority(board, destination, num);
        for (const auto& source: source_candidate) {
            mover::use_all(board, source, destination);
            evaluater::ScoreType score =  evaluater::count_connected(board, destination);
            if (best_score < score) {
                best_score = score;
                best_action = source;
            }
            board.back_to(start);
        }
        mover::use_all(board, best_action, destination);
    }

    void greedy_twice(Board& board, const PointXY& destination, int lim) {
        // cout << destination << endl;
        evaluater::ScoreType best_score = -evaluater::INF;
        PointXY best_action = {0, 0};
        int init1 = board.get_count();

        auto sources1 = finder::line_priority(board, destination, lim);
        for (const auto& source1 : sources1) {
            int init2 = board.get_count();
            solver::mover::use_all(board, source1, destination);
            PointXY next_destination = board.next(destination);
            if (next_destination.x == -1 && next_destination.y == -1) {
                return;
            }
            auto sources2 = finder::line_priority(board, next_destination, lim);
            for (const auto& source2 : sources2) {
                mover::use_all(board, source2, next_destination);
                auto score = solver::evaluater::count_connected(board, destination);
                if (best_score < score) {
                    best_score = score;
                    best_action = source1;
                }
                board.back_to(init2);
            }
            board.back_to(init1);
        }
        solver::mover::use_all(board, best_action, destination);
     }        

    void beam_serch(Board& board, const PointXY& destination, int lim, int beam_width, int max_depth) {
        BeamNode root(board, 0);
        
        // 各深さに対して
        for (int depth = 0; depth < max_depth; depth++) {
            // 各葉要素に対して
            auto reaf_nodes = root.get_leaf_nodes();
            for (const auto& reaf_node : reaf_nodes) {
                // 試行と評価
                priority_queue<State> beam;
                int init = reaf_node->board.get_count();
                auto next_destination = reaf_node->board.next({0, 0});
                auto source_candidate = finder::line_priority(board, next_destination);
                for (const auto& source : source_candidate) {
                    mover::use_all(reaf_node->board, source, next_destination);
                    auto score = evaluater::count_connected(reaf_node->board, destination);
                    beam.emplace(score, source);
                    reaf_node->board.back_to(init);
                }
                // 幅以内でノードの作成
                auto child_nodes = root.add_children(min(beam_width, int(beam.size())), depth==0);
                for (const auto& child_node : child_nodes) {
                    mover::use_all(child_node->board, beam.top().source, destination);
                    child_node->score = evaluater::count_connected(child_node->board, destination);
                    beam.pop();
                }
            }
        }

        int best_score = -evaluater::INF;
        BeamNode* best_board;
        auto reaf_nodes = root.get_leaf_nodes();
        for (const auto& reaf_node : reaf_nodes) {
            int score = evaluater::count_connected(reaf_node->board, destination);
            if (best_score < score) {
                best_score = score;
                best_board = reaf_node;
            }
        }
        board = best_board->get_root_direct_child()->board;
    }
}
    
namespace solver::finder {

 vector<PointXY> line_priority(const Board& board, const PointXY& finding, int lim) {
        const PieceType finding_value = board.get_template()(finding.x, finding.y);
        vector<PointXY> result, tmp;

        // 優先順位(最下段) : 同行-2べき(1) > 同行-その他(2)
        if (finding.y == board.get_height() - 1) {
            for (int x = finding.x + 1; x < board.get_width(); x++) {
                if (board(x, finding.y) == finding_value) {
                    if (bit::is_power_of_2(x - finding.x)) {
                        result.emplace_back(x, finding.y);
                        if (result.size() >= lim) {
                            return result;
                        }
                    } else {
                        tmp.emplace_back(x, finding.y);
                    }
                }
            }
            marge_vector(result, tmp, lim);

        // 優先順位 : 同列-2べき(1) > 同列-その他(2) > 同行-2べき(1) > 同行-その他(2) > その他(3)
        } else {
            for (int y = finding.y + 1; y < board.get_height(); y++) {
                if (board(finding.x, y) == finding_value) {
                    if (bit::is_power_of_2(y - finding.y)) {
                        result.emplace_back(finding.x, y);
                        if (result.size() >= lim) {
                            return result;
                        }
                    } else {
                        tmp.emplace_back(finding.x, y);
                    }
                }
            }
            marge_vector(result, tmp, lim);
            if (result.size() >= lim) {
                return result;
            }

            for (int x = finding.x + 1; x < board.get_width(); x++) {
                if (board(x, finding.y) == finding_value) {
                    if (bit::is_power_of_2(x - finding.x)) {
                        result.emplace_back(x, finding.y);
                        if (result.size() >= lim) {
                            return result;
                        }
                    } else {
                        tmp.emplace_back(x, finding.y);
                    }
                }
            }
            marge_vector(result, tmp, lim);
            if (result.size() >= lim) {
                return result;
            }

            for (int y = finding.y + 1; y < board.get_height(); y++) {
                for (int x = 0; x < board.get_width(); x++) {
                    if (x != finding.x && board(x, y) == finding_value) {
                        result.emplace_back(x, y);
                        if (result.size() >= lim) {
                            return result;
                        }
                    }
                }
            }
        }

        if (result.empty()) {
            throw logic_error("not found piece");
        } else {
            return result;
        }
    }

    vector<PointXY> line_priority(const Board& board, const PointXY& finding) {
        static int inf = numeric_limits<int>::max();
        return line_priority(board, finding, inf);
    }

    vector<PointXY> bit_count_priority(const Board& board, const PointXY& finding, int lim) {
        const PieceType finding_piece = board.get_template().get_goal_at(finding.x, finding.y);
        vector<PointXY> result;

        for (int cnt = 1; cnt < 9; cnt++) {
            for (int y = finding.y + 1; y < board.get_height(); y++) {
                if (bit::bits_count(y - finding.y) == cnt) {
                    for (int x = 0; x < board.get_width(); x++) {
                        if (board(x, y) == finding_piece) {
                            result.emplace_back(x, y);
                        }
                    }
                } else {
                    continue;
                }
            }
            if (result.empty()) {
                result.clear();
            } else {
                return result;
            }
        }

        for (int x = finding.x + 1; x < board.get_width(); x++) {
            if (board(x, finding.y) == finding_piece) {
                result.emplace_back(x, finding.y);
            }
        }
        if (result.empty()) {
            throw logic_error("not found piece");
        } else {
            return result;
        }
    }

    vector<PointXY> bit_count_priority(const Board& board, const PointXY& finding) {
        static int inf = numeric_limits<int>::max();
        return bit_count_priority(board, finding, inf);
    }
}

namespace solver::mover {
    
    void use_typeI(Board& board, const PointXY& source, const PointXY& destination) {
        if (source == destination) {
            return;
        }
        
        auto delta = source - destination;
        if (delta.y == 0) {
            // only X-axis move
            if (destination.x == 0) {
                board.operate(I, 256, -256 + (source.x - destination.x), source.y, LEFT);
            } else {
                for (int i = 0b100000000; i != 0; i >>= 1) {
                    if (i & delta.x) {
                        board.operate(I, i, destination.x, destination.y, LEFT);
                    }
                }
            }

        } else {
            // X-axis move-left
            if (source.x > destination.x) {
                if (bit::is_power_of_2(delta.x)) {
                    board.operate(I, delta.x, destination.x, source.y, LEFT);
                } else {
                    board.operate(I, 256, -256 + (source.x - destination.x), source.y, LEFT);
                }
            // X-axis move-right
            } else if (source.x < destination.x){
                if (bit::is_power_of_2(delta.x)) {
                    board.operate(I, delta.x, source.x + 1, source.y, RIGHT);
                } else {
                    board.operate(I, 256, board.get_width() - (destination.x - source.x), source.y, RIGHT);
                }
            } else {
                // NOP
            }
            
            // Y-axis move
            if (destination.y == 0) {
                board.operate(I, 256, destination.x, -256 + (source.y - destination.y), UP);
            } else {
                for (int i = 0b100000000; i != 0; i >>= 1) {
                    if (i & delta.y) {
                        board.operate(I, i, destination.x, destination.y, UP);
                    }
                }
            }
        }
    }

    void use_all(Board& board, const PointXY& source, const PointXY& destination) {
        if (source == destination) {
            return;
        }

        auto delta = destination - source;
        // 同じ行に存在
        if (delta.y == 0) {
            if (bit::is_power_of_2(delta.x)) {
                board.operate(I, delta.x, destination.x, destination.y, LEFT);
            } else {
                int size_I = bit::downer_next_power_of_2(delta.x);
                int remaining = delta.x - size_I;
                board.operate(III, bit::upper_next_power_of_2(remaining*2), source.x - remaining*2 + 1, destination.y, LEFT);
                board.operate(I, size_I, destination.x, destination.y, LEFT);
            }
            
        // その他
        } else {
            // 移動元が左に存在
            if (source.x < destination.x) {
                int size = bit::upper_next_power_of_2(delta.x);
                board.operate(I, size, board.get_width() - delta.x, source.y, RIGHT);

            // 移動元が右に存在
            } else if (destination.x < source.x) {
                int size = bit::upper_next_power_of_2(delta.x);
                board.operate(I, size, -size + delta.x, source.y, LEFT);
                
            // 同じ列に存在
            } else {
                // 何もしない
            }

            // 上に移動

            // 移動先が最上段の場合
            if (destination.y == 0) {
                int size = bit::upper_next_power_of_2(delta.y);
                board.operate(I, size, destination.x,  -size + delta.y, UP);

            // その他
            } else if (bit::is_power_of_2(delta.y)) {
                board.operate(I, delta.y, destination.x, destination.y, UP);
            } else {
                int size_I = bit::downer_next_power_of_2(delta.y);
                int remaining = delta.y - size_I;
                board.operate(II, bit::upper_next_power_of_2(remaining*2), destination.x, source.y - remaining*2 + 1, UP);
                board.operate(I, size_I, destination.x, destination.y, UP);
            }
        }
    }
}

namespace solver::evaluater {
    
    ScoreType count_connected(const Board& board, const PointXY& start) {
        ScoreType score = 0;
        for (int y = start.y + 1; y < board.get_height(); y++) {
            for (int x = start.x; x < board.get_width(); x++) {
                if (board.is_match(x, y)) {
                    score++;
                } else {
                    return score;
                }
            }
        }
        return score;
    }

}