#ifndef INCLUDE_FONT_HPP_
#define INCLUDE_FONT_HPP_

#include <iostream>

namespace font {
    enum Color{black, red, green, yellow, blue, magenta, cyan, white, color_reset};

    // 指定リセット(演算子オーバーロード)
    inline std::ostream& operator<<(std::ostream& os, const Color& color) {
        if (color == color_reset) {
            return os << "\033[0m";
        } else {
            return os << "\033[3" << static_cast<int>(color) << "m";
        }
    }

    // 指定リセット
    inline void reset() {
        std::cout << "\033[0m";
    }
    
    // 太字
    inline void bold() {
        std::cout << "\033[1m";
    }

    // 文字を薄く表示
    inline void dim() {
        std::cout << "\033[2m";
    }

    // イタリック
    inline void italicize() {
        std::cout << "\033[3m";
    }

    // 下線
    inline void underline() {
        std::cout << "\033[4m";
    }

    // 点滅
    inline void blink() {
        std::cout << "\033[5m";
    }

    // 点滅(高速)
    inline void blink_fast() {
        std::cout << "\033[6m";
    }

    // 取り消し線
    inline void strike_through() {
        std::cout << "\033[9m";
    }

    // 文字色の指定
    inline void set_font_color(int color) {
        std::cout << "\033[3" << static_cast<int>(color) << "m";
    }

    // 背景色の指定
    inline void set_back_color(int color) {
        std::cout << "\033[4" << static_cast<int>(color) << "m";
    }

    
}

#endif