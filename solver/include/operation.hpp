#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <iostream>

#include "common.hpp"

struct Operation {
    // 抜き型のタイプ
    Type type;
    // 定型抜き型の場合は抜き型のサイズ(一般抜き型の場合は通し番号)
    int size;
    // 適用x座標
    int x;
    // 適用y座標
    int y;
    // 寄席方向
    Direction direction;

    // コンストラクタ
    Operation(Type type, int size, int x, int y, Direction direction);
    Operation(int type, int size, int x, int y, int direction);

    // 演算子オーバーロード(標準出力時に利用)
    friend std::ostream& operator<<(std::ostream& os, const Operation& ope);
};

inline std::ostream& operator<<(std::ostream& os, const Operation& ope) {
    os << "{" << ope.type << "-" << ope.size << ", (" << ope.x << ", " << ope.y << "), " << ope.direction << "}";
    return os;
}

#endif
