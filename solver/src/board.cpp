#include "board.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <utility>
#include <algorithm>

#include "common.hpp"
#include "board_template.hpp"
#include "operation.hpp"

#include "json.hpp"
#include "font.hpp"

using namespace std;

Board::Board(const BoardTemplate& board_template):
    board_template(board_template),
    width(board_template.get_width()),
    height(board_template.get_height())
{
    state = board_template.get_start();
    count = 0;
}

void Board::print() {
    cout << "Count : " << count << endl;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cout << state[y][x] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

void Board::print_color() {
    cout << "Count : " << count << endl;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            font::set_back_color(state[y][x] + 1);
            cout << "  ";
        }
        font::reset();
        cout << endl;
    }
    cout << endl;
}

const bool Board::operate(Type type, int size, int x0, int y0, Direction direction) {
    bool is_legal = (type == GENERAL) ? (0 <= size && size < board_template.get_n()) : (bit::is_power_of_2(size) && 1<= size && size <= 256);

    switch (direction) {
    case UP:
        is_legal = is_legal && _operate_up(type, size, x0, y0);
        break;
    case DOWN:
        is_legal = is_legal && _operate_down(type, size, x0, y0);
        break;
    case LEFT:
        is_legal = is_legal && _operate_left(type, size, x0, y0);
        break;
    case RIGHT:
        is_legal = is_legal && _operate_right(type, size, x0, y0);
        break;
    }

    count++;
    history.emplace_back(type, size, x0, y0, direction);
    return is_legal;
}

const bool Board::back(int n) {
    if (!(0 <= n && n <= count)) {
        return false;
    }

    for (int i = 0; i < n; i++) {
        auto [type, size, x0, y0, direction] = history.back();
        switch (direction) {
        case UP:
            _back_up(type, size, x0, y0);
            break;
        case DOWN:
            _back_down(type, size, x0, y0);
            break;
        case LEFT:
            _back_left(type, size, x0, y0);
            break;
        case RIGHT:
            _back_right(type, size, x0, y0);
            break;
        }
        count--;
        history.pop_back();
    }
    return true;
}

const bool Board::back_to(int n) {
    if (!(0 <= n && n <= count)) {
        return false;
    }

    while (n < count) {
        auto [type, size, x0, y0, direction] = history.back();
        switch (direction) {
        case UP:
            _back_up(type, size, x0, y0);
            break;
        case DOWN:
            _back_down(type, size, x0, y0);
            break;
        case LEFT:
            _back_left(type, size, x0, y0);
            break;
        case RIGHT:
            _back_right(type, size, x0, y0);
            break;
        }
        count--;
        history.pop_back();
    }
    return true;
}
nlohmann::json Board::dump(bool inverse) const {
    nlohmann::json result = {
        {"n", history.size()},
        {"ops", nlohmann::json::array()},
    };
    
    for (const auto &e : history) {
        auto [type, size, x0, y0, direction] = inverse ? board_template.inverse(e) : e;
        nlohmann::json ops = {
            {"p", BoardTemplate::to_p_number(type, size)},
            {"x", x0},
            {"y", y0},
            {"s", static_cast<int>(direction)},
        };
        result["ops"].push_back(move(ops));
    }
    return result;
}

bool Board::_operate_up(Type type, int size, int x0, int y0) {
    bool is_moved = false;
    auto condition = board_template.judge_func(type, size, x0, y0);
    auto row_counter = board_template.row_count(type, size, x0, y0);
    int left_lim = board_template.left_limit(type, size, x0, y0);
    int right_lim = board_template.right_limit(type, size, x0, y0);
    int up_lim = board_template.up_limit(type, size, x0, y0);

    int cnt;
    vector<PieceType> picked;
    for (int x = left_lim; x <= right_lim; x++) {
        if (!row_counter(x)) {
            continue;
        }

        cnt = 0;
        picked.clear();
        for (int y = up_lim; y < height; y++) {
            if (condition(x, y)) {
                picked.push_back(state[y][x]);
                cnt++;
            } else {
                state[y - cnt][x] = state[y][x];
            }
        }
        is_moved = is_moved || static_cast<bool>(cnt);

        cnt = height - cnt;
        for (const auto& e : picked) {
            state[cnt][x] = e;
            cnt++;
        }
    }
    return is_moved;
}

bool Board::_operate_down(Type type, int size, int x0, int y0) {
    bool is_moved = false;
    auto condition = board_template.judge_func(type, size, x0, y0);
    auto row_counter = board_template.row_count(type, size, x0, y0);
    int left_lim = board_template.left_limit(type, size, x0, y0);
    int right_lim = board_template.right_limit(type, size, x0, y0);
    int down_lim = board_template.down_limit(type, size, x0, y0);
    
    int cnt;
    vector<PieceType> picked;
    for (int x = left_lim; x <= right_lim ; x++) {        
        if (!row_counter(x)) {
            continue;
        }

        cnt = 0;
        picked.clear();
        for (int y = down_lim; y >= 0; y--) {
            if (condition(x, y)) {
                picked.push_back(state[y][x]);
                cnt++;
            } else {
                state[y + cnt][x] = state[y][x];
            }
        }
        is_moved = is_moved || static_cast<bool>(cnt);
        
        cnt = cnt - 1;
        for (const auto& e : picked) {
            state[cnt][x] = e;
            cnt--;
        }
    }
    return is_moved;
}

bool Board::_operate_left(Type type, int size, int x0, int y0) {
    bool is_moved = false;
    auto condition = board_template.judge_func(type, size, x0, y0);
    auto line_counter = board_template.line_count(type, size, x0, y0);
    int left_lim = board_template.left_limit(type, size, x0, y0);
    int up_lim = board_template.up_limit(type, size, x0, y0);
    int down_lim = board_template.down_limit(type, size, x0, y0);

    int cnt;
    vector<PieceType> picked;
    for (int y = up_lim; y <= down_lim; y++) {
        if (!line_counter(y)) {
            continue;
        }

        cnt = 0;
        picked.clear();
        for (int x = left_lim; x < width; x++) {
            if (condition(x, y)) {
                picked.push_back(state[y][x]);
                cnt++;
            } else {
                state[y][x - cnt] = state[y][x];
            }
        }
        is_moved = is_moved || static_cast<bool>(cnt);

        cnt = width - cnt;
        for (const auto& e : picked) {
            state[y][cnt] = e;
            cnt++;
        }
    }
    return is_moved;
}

bool Board::_operate_right(Type type, int size, int x0, int y0) {
    bool is_moved = false;
    auto condition = board_template.judge_func(type, size, x0, y0);
    auto line_counter = board_template.line_count(type, size, x0, y0);
    int right_lim = board_template.right_limit(type, size, x0, y0);
    int up_lim = board_template.up_limit(type, size, x0, y0);
    int down_lim = board_template.down_limit(type, size, x0, y0);

    int cnt;
    vector<PieceType> picked;
    for (int y = up_lim; y <= down_lim; y++) {
        if (!line_counter(y)) {
            continue;
        }

        cnt = 0;
        picked.clear();
        for (int x = right_lim; x >= 0 ; x--) {
            if (condition(x, y)) {
                picked.push_back(state[y][x]);
                cnt++;
            } else {
                state[y][x + cnt] = state[y][x];
            }
        }
        is_moved = is_moved || static_cast<bool>(cnt);

        cnt = cnt - 1;
        for (const auto& e : picked) {
            state[y][cnt] = e;
            cnt--;
        }
    }
    return is_moved;
}

void Board::_back_up(Type type, int size, int x0, int y0) {
    auto condition = board_template.judge_func(type, size, x0, y0);
    auto row_counter = board_template.row_count(type, size, x0, y0);
    int left_lim = board_template.left_limit(type, size, x0, y0);
    int right_lim = board_template.right_limit(type, size, x0, y0);
    int up_lim = board_template.up_limit(type, size, x0, y0);

    int cnt;
    vector<PieceType> picked;
    for (int x = left_lim; x <= right_lim; x++) {
        int row_count = row_counter(x);
        picked.clear();
        for (int y = height - row_count; y < height; y++) {
            picked.push_back(state[y][x]);
        }

        cnt = row_count;
        for (int y = height - 1; y >= up_lim; y--) {
            if (condition(x, y)) {
                state[y][x] = picked[cnt - 1];
                cnt--;
            } else {
                state[y][x] = state[y - cnt][x];
            }
        }
    }
}

void Board::_back_down(Type type, int size, int x0, int y0) {
    auto condition = board_template.judge_func(type, size, x0, y0);
    auto row_counter = board_template.row_count(type, size, x0, y0);
    int left_lim = board_template.left_limit(type, size, x0, y0);
    int right_lim = board_template.right_limit(type, size, x0, y0);
    int down_lim = board_template.down_limit(type, size, x0, y0);

    int cnt;
    vector<PieceType> picked;
    for (int x = left_lim; x <= right_lim; x++) {
        int row_count = row_counter(x);
        picked.clear();
        for (int y = 0; y < row_count; y++) {
            picked.push_back(state[y][x]);
        }
        
        cnt = 0;
        for (int y = 0; y <= down_lim; y++) {
            if (condition(x, y)) {
                state[y][x] = picked[cnt];
                cnt++;
            } else {
                state[y][x] = state[y + (row_count - cnt)][x];
            }
        }
    }
}

void Board::_back_left(Type type, int size, int x0, int y0) {
    auto condition = board_template.judge_func(type, size, x0, y0);
    auto line_counter = board_template.line_count(type, size, x0, y0);
    int left_lim = board_template.left_limit(type, size, x0, y0);
    int up_lim = board_template.up_limit(type, size, x0, y0);
    int down_lim = board_template.down_limit(type, size, x0, y0);

    int cnt;
    vector<PieceType> picked;
    for (int y = up_lim; y <= down_lim; y++) {
        int line_count = line_counter(y);
        picked.clear();
        for (int x = width - line_count; x < width; x++) {
            picked.push_back(state[y][x]);
        }

        cnt = line_count;
        for (int x = width - 1; x >= left_lim; x--) {
            if (condition(x, y)) {
                state[y][x] = picked[cnt - 1];
                cnt--;
            } else {
                state[y][x] = state[y][x - cnt];
            }
        }
    }
}


void Board::_back_right(Type type, int size, int x0, int y0) {
    auto condition = board_template.judge_func(type, size, x0, y0);
    auto line_counter = board_template.line_count(type, size, x0, y0);
    int right_lim = board_template.right_limit(type, size, x0, y0);
    int up_lim = board_template.up_limit(type, size, x0, y0);
    int down_lim = board_template.down_limit(type, size, x0, y0);

    int cnt;
    vector<PieceType> picked;
    for (int y = up_lim; y <= down_lim; y++) {
        int line_count = line_counter(y);
        picked.clear();
        for (int x = 0; x < line_count; x++) {
            picked.push_back(state[y][x]);
        }

        cnt = 0;
        for (int x = 0; x <= right_lim; x++) {
            if (condition(x, y)) {
                state[y][x] = picked[cnt];
                cnt++;
            } else {
                state[y][x] = state[y][x + (line_count - cnt)];
            }
        }

    }
}