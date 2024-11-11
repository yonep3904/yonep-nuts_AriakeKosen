#ifndef BEAM_NODE_HPP
#define BEAM_NODE_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <utility>
#include <functional>

#include "board.hpp"

class BeamNode {
public:
    Board board;
    int score;
    BeamNode* parent;
    std::vector<std::unique_ptr<BeamNode>> children;

    // コンストラクタ
    BeamNode(const Board& b, int s);

    // scoreの比較(等号, 否定等号, 不等号)が可能
    bool operator==(const BeamNode& other) const;
    bool operator!=(const BeamNode& other) const;
    bool operator<(const BeamNode& other) const;
    bool operator<=(const BeamNode& other) const;
    bool operator>(const BeamNode& other) const;
    bool operator>=(const BeamNode& other) const;

    // 親ノードの情報を使って子ノードを追加する（コピー）
    void add_child_by_copy();
    // 親ノードの情報を使って子ノードを追加する（ムーブ）
    void add_child_by_move();
    // nの数の子ノードが追加される。ひとつのみ移動を行う
    std::vector<BeamNode*> add_children(int n, bool all_copy=false);

    // 自分の先祖かつ根ノード直下のノードのポインタを返す
    BeamNode* get_root_direct_child();
    // 葉ノードのポインタを返す
    std::vector<BeamNode*> get_leaf_nodes();

    // 子ノードを削除する
    void delete_children();
    // 自身と子ノードを削除する
    void delete_self();
};

inline bool BeamNode::operator==(const BeamNode& other) const {
    return score == other.score;
}

inline bool BeamNode::operator!=(const BeamNode& other) const {
    return score != other.score;
}

inline bool BeamNode::operator<(const BeamNode& other) const {
    return score < other.score;
}

inline bool BeamNode::operator<=(const BeamNode& other) const {
    return score <= other.score;
}

inline bool BeamNode::operator>(const BeamNode& other) const {
    return score > other.score;
}

inline bool BeamNode::operator>=(const BeamNode& other) const {
    return score >= other.score;
}

inline void BeamNode::delete_children() {
    children.clear();
}

#endif



// struct BeamNode{
//     // コンストラクタ
//     BeamNode(Board state, PointXY& origin_source, );
//     BeamNode();
//     BeamNode(int score, const PointXY& source, const PointXY& destination, int count, int depth);
//     BeamNode(int score, const PointXY& source, const PointXY& destination, int count, const BeamNode& parent);

//     // scoreの比較(等号, 否定等号, 不等号)が可能
//     bool operator==(const BeamNode& other) const;
//     bool operator!=(const BeamNode& other) const;
//     bool operator<(const BeamNode& other) const;
//     bool operator<=(const BeamNode& other) const;
//     bool operator>(const BeamNode& other) const;
//     bool operator>=(const BeamNode& other) const;

//     // 標準出力時に利用
//     friend std::ostream& operator<<(std::ostream& os, const PointXY& p);
// };


// inline BeamNode::BeamNode():
//     score(-1),
//     depth(-1),
//     count(-1),
//     source({-1, -1}),
//     destination({-1, -1}),
//     origin_source({-1, -1})
// {

// }

// inline BeamNode::BeamNode(int score, const PointXY& source, const PointXY& destination, int count, int depth):
//     score(score),
//     depth(depth),
//     count(count),
//     source(source),
//     destination(destination),
//     origin_source(source)
// {

// }

// inline BeamNode::BeamNode(int score, const PointXY& source, const PointXY& destination, int count, const BeamNode& parent):
//     score(score),
//     depth(parent.depth + 1),
//     count(count),
//     source(source),
//     destination(destination),
//     origin_source(parent.origin_source)
// {

// }


// inline bool BeamNode::operator==(const BeamNode& other) const {
//     return score == other.score;
// }

// inline bool BeamNode::operator!=(const BeamNode& other) const {
//     return score != other.score;
// }

// inline bool BeamNode::operator<(const BeamNode& other) const {
//     return score < other.score;
// }

// inline bool BeamNode::operator<=(const BeamNode& other) const {
//     return score <= other.score;
// }

// inline bool BeamNode::operator>(const BeamNode& other) const {
//     return score > other.score;
// }

// inline bool BeamNode::operator>=(const BeamNode& other) const {
//     return score >= other.score;
// }

// inline std::ostream& operator<<(std::ostream& os, const BeamNode& beam_node) {
//     os << "{" << beam_node.source << ", " << beam_node.score << "}";
//     return os;
// }

// struct BeamNode {
//     // 評価値
//     int score;
//     // 直前の手数
//     int count;
//     // 移動元座標
//     PointXY source;
//     // 親ノード
//     BeamNode* parent;

//     // コンストラクタ
//     BeamNode();
//     BeamNode(int score, int count, const PointXY& source, BeamNode* parent = nullptr);

//     // scoreの比較(等号, 否定等号, 不等号)が可能
//     bool operator==(const BeamNode& other) const;
//     bool operator!=(const BeamNode& other) const;
//     bool operator<(const BeamNode& other) const;
//     bool operator<=(const BeamNode& other) const;
//     bool operator>(const BeamNode& other) const;
//     bool operator>=(const BeamNode& other) const;

//     // 標準出力時に利用
//     friend std::ostream& operator<<(std::ostream& os, const PointXY& p);

//     std::vector<PointXY> source_path();
// };

// inline BeamNode::BeamNode():
//     score(-1),
//     count(-1),
//     source({-1, -1}),
//     parent(nullptr)
// {

// }

// inline BeamNode::BeamNode(int score, int count, const PointXY& source, BeamNode* parent):
//     score(score),
//     count(count),
//     source(source),
//     parent(parent)
// {

// }


// inline bool BeamNode::operator==(const BeamNode& other) const {
//     return score == other.score;
// }

// inline bool BeamNode::operator!=(const BeamNode& other) const {
//     return score != other.score;
// }

// inline bool BeamNode::operator<(const BeamNode& other) const {
//     return score < other.score;
// }

// inline bool BeamNode::operator<=(const BeamNode& other) const {
//     return score <= other.score;
// }

// inline bool BeamNode::operator>(const BeamNode& other) const {
//     return score > other.score;
// }

// inline bool BeamNode::operator>=(const BeamNode& other) const {
//     return score >= other.score;
// }

// inline std::ostream& operator<<(std::ostream& os, const BeamNode& beam_node) {
//     os << "{" << beam_node.source << ", " << beam_node.score << "}";
//     return os;
// }
