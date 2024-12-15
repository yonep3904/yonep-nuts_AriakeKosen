// make_gif.exeのメイン関数です
// コンパイルする際はsolverのmain.cppをこれに置き換えてmakeしてください

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>

#include "common.hpp"
#include "board_template.hpp"
#include "board.hpp"
#include "operation.hpp"
#include "point_xy.hpp"
#include "beam_node.hpp"
#include "solver.hpp"

#include "json.hpp"
#include "font.hpp"
#include "gif.h"

using namespace std;

int main(int argc, char* argv[]) {
    const string input_file = "\\problem.json";
    const string output_file = "\\solution.gif";

    // コマンドライン引数の確認
    if (argc < 4) {
        cerr << "Usage: [problem.json]\n" << endl;
        return 1;
    }
    const auto dir_path = string(argv[1]);
    const int delay = atoi(argv[2]);
    const int rate = atoi(argv[3]);
    // ファイルの読み込み
    ifstream ifs(dir_path + input_file);
    if (!ifs) {
        cerr << "File not found : " << (dir_path + input_file) << endl;
        return 1;
    }

    BoardTemplate board_template(ifs);
    Board board(board_template);
    bool success;
    GifWriter gif_writer;

    success = GifBegin(&gif_writer, (dir_path + output_file).c_str(), board_template.get_width()*rate, board_template.get_height()*rate, delay);
    if (!success) {
        std::cerr << "Failed to create GIF file.\n" << std::endl;
        return 1;
    }

    auto image = board.make_image(rate);
    success = GifWriteFrame(&gif_writer, image.data(), board_template.get_width()*rate, board_template.get_height()*rate, delay);
    if (!success) {
        std::cerr << "Failed to write GIF frame.\n" << std::endl;
        return 1;
    }
    for (int y = 0; y < board_template.get_height(); y++) {
        for (int x = 0; x < board_template.get_width(); x++) {
            if (board.is_match(x, y)) {
                continue;
            } else {
                solver::greedy(board, {x, y}, 10);
                auto image = board.make_image(rate);
                success = GifWriteFrame(&gif_writer, image.data(), board_template.get_width()*rate, board_template.get_height()*rate, delay);
                if (!success) {
                    std::cerr << "Failed to write GIF frame.\n" << std::endl;
                    return 1;
                }
            }
        }
    }

    success = GifEnd(&gif_writer);
    if (!success) {
        std::cerr << "Failed to close GIF file.\n" << std::endl;
        return 1;
    }
    return 0;

}