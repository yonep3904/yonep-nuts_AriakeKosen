#include <iostream>
#include <vector>

#include "point_xy.hpp"
#include "board.hpp"
#include "beam_node.hpp"

using namespace std;

vector<PointXY> BeamNode::source_path() {
    vector<PointXY> path;
    BeamNode* node = this;
    while (node->parent != nullptr) {
        path.push_back(node->source);
        node = node->parent;
    }
    path.pop_back();
    reverse(path.begin(), path.end());
    return path;
}
