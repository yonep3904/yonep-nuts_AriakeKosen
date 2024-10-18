#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <vector>
#include <map>
#include <string>
#include <limits>

#include "common.hpp"

#include "json.hpp"

namespace checker {
    // 解答が本当に有効な解答か確認する(返り値0が有効な回答)
    int legal_check(const BoardTemplate& board_template, const nlohmann::json& answer);
    int legal_check(const BoardTemplate& board_template, const std::string& answer);

    // legal_checkによる返り値のエラーコード
    extern std::map<int, std::string> legal_check_map;
}

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


#endif