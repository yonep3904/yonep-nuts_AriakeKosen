#include <memory>
#include <vector>
#include <utility>
#include <functional>

#include "point_xy.hpp"
#include "board.hpp"
#include "beam_node.hpp"

using namespace std;

BeamNode::BeamNode(const Board& b, int s):
    board(b),
    score(s),
    parent(nullptr)
{

}

void BeamNode::add_child_by_copy() {
    auto child = make_unique<BeamNode>(board, score);
    child->parent = this;
    children.push_back(move(child));
}

void BeamNode::add_child_by_move() {
    auto child = make_unique<BeamNode>(move(board), score);
    child->parent = this;
    children.push_back(move(child));
}

vector<BeamNode*> BeamNode::add_children(int n, bool all_copy) {
    vector<BeamNode*> added_children;

    if (all_copy) {
        for (int i = 0; i < n; i++) {
            add_child_by_copy();
        }
    } else {
        for (int i = 0; i < n - 1; i++) {
            add_child_by_copy();
        }
        add_child_by_move();
    }

    // 追加した子ノードのポインタを格納
    for (auto& child : children) {
        added_children.emplace_back(child.get());
    }
    return added_children;
}

vector<BeamNode*> BeamNode::get_leaf_nodes() {
    vector<BeamNode*> leaf_nodes;
    if (children.empty()) {
        leaf_nodes.emplace_back(this);
    } else {
        for (auto& child : children) {
            auto child_leafs = child->get_leaf_nodes();
            leaf_nodes.insert(leaf_nodes.end(), child_leafs.begin(), child_leafs.end());
        }
    }
    return leaf_nodes;
}

BeamNode* BeamNode::get_root_direct_child() {
    BeamNode* current = this;
    do {
        current = current->parent;
    } while (current && current->parent);
    // 根直下のノードがcurrentであり、それが自身の祖先であるか確認
    return (current && current != this) ? current : nullptr;
}

void BeamNode::delete_self() {
    delete_children();

    if (parent) {
        // 親ノードの子ノードリストから自身を削除
        auto& siblings = parent->children;
        siblings.erase(remove_if(siblings.begin(), siblings.end(),
            [this](const unique_ptr<BeamNode>& child) {
                return child.get() == this;
            }), siblings.end());
    }
}