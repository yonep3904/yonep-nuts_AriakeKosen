#ifndef POINT_XY_HPP
#define POINT_XY_HPP

#include <iostream>
#include <cmath>

#include "common.hpp"

struct PointXY {
    int x;
    int y;

    // コンストラクタ
    PointXY();
    PointXY(int x, int y);

    // 比較(等号, 負等号)が可能
    bool operator==(const PointXY& other) const;
    bool operator!=(const PointXY& other) const;
    // PointXY同士の引き算 -> 距離を出力
    PointXY operator-(const PointXY& other) const;

    // 標準出力時に利用
    friend std::ostream& operator<<(std::ostream& os, const PointXY& p);

    // adderの分だけ座標を右または下にずらす
    PointXY displace(int adder, int width, int height) const;
};

inline PointXY::PointXY() :
    x(0),
    y(0)
{

}

inline PointXY::PointXY(int x, int y) :
    x(x),
    y(y)
{

}


inline bool PointXY::operator==(const PointXY& other) const {
    return x == other.x && y == other.y;
}

inline bool PointXY::operator!=(const PointXY& other) const {
    return !operator==(other);
}

inline PointXY PointXY::operator-(const PointXY& other) const {
    return {std::abs(x - other.x), std::abs(y - other.y)};
}

inline PointXY PointXY::displace(int adder, int width, int height) const {
    return {(x + adder)%width, y + (x + adder)/width};
}

inline std::ostream& operator<<(std::ostream& os, const PointXY& p) {
    os << "(" << p.x << " ," << p.y << ")";
    return os;
}

#endif