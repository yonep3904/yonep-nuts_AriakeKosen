#include "board_template.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <bitset>
#include <utility>
#include <fstream>

#include "common.hpp"

#include "json.hpp"
#include "font.hpp"

using namespace std;

namespace {
    // 整数値を任意の桁数でスペース埋めして返す
    string sfill(int n, int digit) {
        ostringstream oss;
        oss << setw(digit) << setfill(' ') << n;
        return oss.str();
    }

    // 2次元配列をn回右に90°回転させる
    template <class T>
    std::vector<std::vector<T>> rotate_vector2D(const std::vector<std::vector<T>>& vec2D, int n) {
        static int direction_num = 4;
        int rotate_num = (n%direction_num < 0 ? n%direction_num + 4 : n%direction_num);
        
        int cols = vec2D.size();
        int rows = vec2D[0].size();
        std::vector<std::vector<T>> result;

        switch(rotate_num) {
        case 0:
            result.resize(cols);
            for (int y = 0; y < cols; y ++) {
                result[y].resize(rows);
                for (int x = 0; x < rows; x++) {
                    result[y][x] = vec2D[y][x];
                }
            }
            break;

        case 1:
            result.resize(rows);      
            for (int y = 0; y < rows; y ++) {
                result[y].resize(cols);
                for (int x = 0; x < cols; x++) {
                    result[y][x] = vec2D[(cols - 1) - x][y];
                }
            }  
            break;

        case 2:
            result.resize(cols);
            for (int y = 0; y < cols; y ++) {
                result[y].resize(rows);
                for (int x = 0; x < rows; x++) {
                    result[y][x] = vec2D[(cols - 1) - y][(rows - 1) - x];
                }
            }
            break;

        case 3:
            result.resize(rows);      
            for (int y = 0; y < rows; y ++) {
                result[y].resize(cols);
                for (int x = 0; x < cols; x++) {
                    result[y][x] = vec2D[x][(rows - 1) - y];
                }
            }
            break;
        }
        return result;
    }

    // 2次元配列を転置させる
    template <class T>
    std::vector<std::vector<T>> transpose_vector2D(const std::vector<std::vector<T>>& vec2D, int n) {
        int cols = vec2D.size();
        int rows = vec2D[0].size();
        std::vector<std::vector<T>> result;
        if (n == 0) {
            result.resize(cols);
            for (int y = 0; y < cols; y++) {
                result[y].resize(rows);
                for (int x = 0; x < rows; x++) {
                    result[y][x] = vec2D[y][x];
                }
            }
        } else {
            result.resize(rows);
            for (int y = 0; y < rows; y++) {
                result[y].resize(cols);
                for (int x = 0; x < cols; x++) {
                    result[y][x] = vec2D[x][y];
                }
            }
        }
        return result;
    }
}

Vector2D BoardTemplate::_to_board(const vector<string>& json_obj) {
    Vector2D result;
    result.reserve(json_obj.size());
    for (const auto& line : json_obj) {
        vector<PieceType> tmp;
        tmp.reserve(line.length());
        for (const char& cell : line) {
            tmp.push_back(static_cast<PieceType>(cell - '0'));
        }
        result.push_back(move(tmp));
    }
    return result;
}

vector<Vector2D> BoardTemplate::_to_general_cells(const vector<nlohmann::json>& json_obj) {
    vector<Vector2D> result;
    result.reserve(json_obj.size());
    for (const auto& pattern : json_obj) {
        result.push_back(_to_board(pattern["cells"]));
    }

    return result;
}

void BoardTemplate::init() {
    static map<int, Direction> direction_num = {
        {0, UP},
        {1, RIGHT},
        {2, DOWN},
        {3, LEFT},
    };

    general_width.reserve(n);
    general_height.reserve(n);
    for (const auto& pattern : general_cells) {
        general_width.push_back(pattern[0].size());
        general_height.push_back(pattern.size());
    }

    if ((transform_code & 0b001) ^ ((transform_code & 0b100) >> 2)) {
        type_dict = {
            {GENERAL, GENERAL},
            {I, I},
            {II, III},
            {III, II},
        };
    } else {
        type_dict = {
            {GENERAL, GENERAL},
            {I, I},
            {II, II},
            {III, III},
        };
    }

    int rotate_num = transform_code & 0b011;
    int transpose_num = transform_code & 0b100;
    direction_dict = {
        {UP, direction_num[(4 + (transpose_num ? 3 : 0) - rotate_num)%4]},
        {RIGHT, direction_num[(4 + (transpose_num ? 2 : 1) - rotate_num)%4]},
        {DOWN, direction_num[(4 + (transpose_num ? 1 : 2) - rotate_num)%4]},
        {LEFT, direction_num[(4 + (transpose_num ? 0 : 3) - rotate_num)%4]},
    };
}

BoardTemplate::BoardTemplate() {

}

BoardTemplate::BoardTemplate(ifstream& file) {
    nlohmann::json json_text;
    file >> json_text;

    width = json_text["board"]["width"];
    height = json_text["board"]["height"];
    start = _to_board(json_text["board"]["start"]);
    goal = _to_board(json_text["board"]["goal"]);
    n = json_text["general"]["n"];
    general_cells = _to_general_cells(json_text["general"]["patterns"]);
    transform_code = 0;
    
    colors.reserve(json_text["colors"].size());
    for (const auto& color : json_text["colors"]) {
        colors.push_back(make_tuple(color[0], color[1], color[2]));
    }
    
    init();
}

BoardTemplate::BoardTemplate(const nlohmann::json& json_text) :
    width(json_text["board"]["width"]),
    height(json_text["board"]["height"]),
    start(_to_board(json_text["board"]["start"])),
    goal(_to_board(json_text["board"]["goal"])),
    n(json_text["general"]["n"]),
    general_cells(_to_general_cells(json_text["general"]["patterns"])),
    transform_code(0)
{
    init();
}

BoardTemplate::BoardTemplate(int width, int height, const Vector2D start, const Vector2D goal, const vector<Vector2D> general) :
    width(width),
    height(height),
    start(start),
    goal(goal),
    n(general.size()),
    general_cells(general),
    transform_code(0)
{
    init();
}

BoardTemplate::BoardTemplate(int width, int height, const Vector2D&& start, const Vector2D&& goal, const vector<Vector2D>&& general, int transform_code) :
    width(width),
    height(height),
    start(start),
    goal(goal),
    n(general.size()),
    general_cells(general),
    transform_code(transform_code)
{
    init();
}

BoardTemplate BoardTemplate::transform(int code) const {
    int rotate_num = (code & 0b011) - (transform_code & 0b011);
    int transpose_num = (code & 0b100) ^ (transform_code & 0b100);
    int is_resize = (rotate_num & 0b01) ^ (transpose_num >> 2);

    int new_width;
    int new_height;
    Vector2D new_start;
    Vector2D new_goal;
    vector<Vector2D> new_general;

    if (is_resize) {
        new_width = height;
        new_height = width;
    } else {
        new_width = width;
        new_height = height;
    }

    new_start = transpose_vector2D(rotate_vector2D(start, rotate_num), transpose_num);
    new_goal = transpose_vector2D(rotate_vector2D(goal, rotate_num), transpose_num);

    new_general.reserve(n);
    for (const auto& e: general_cells) {
        new_general.push_back(transpose_vector2D(rotate_vector2D(e, rotate_num), transpose_num));
    }

    return BoardTemplate(new_width, new_height, move(new_start), move(new_goal), move(new_general), code%8);
}

void BoardTemplate::print() const {
    bitset<3> binary(transform_code);
    cout << "Infomation----------------------" << endl;
    cout << "Tranform Code : " << transform_code << " - 0b" << binary << endl;
    cout << "Board (Size: " << width << " - " << height  << ")" << endl;
    cout << "Start / Goal" << endl; 
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cout << start[y][x] << " ";
        }
        cout << "     ";
        for (int x = 0; x < width; x++) {
            cout << goal[y][x] << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "General Pieces : (Total : " << n << ")" << endl;;
    for (int p = 0; p < n; p++ ){
        cout << "Piece " << p << "(" << BoardTemplate::to_p_number(GENERAL, p) << ") : (Size : " << general_width[p] << " - " << general_height[p] << ")" << endl;
        for (const auto& line : general_cells[p]) {
            for (const auto& cell : line) {
                cout << (cell ? "o" : "-") << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    cout << "--------------------------------" << endl;
}

void BoardTemplate::print_color() const {
    bitset<3> binary(transform_code);
    cout << "Infomation--------------------------------------------" << endl;
    cout << "Tranform Code : " << transform_code << " (0b" << binary << ")" << endl;
    cout << "Board (Size: " << width << " - " << height  << ")  /  General Pieces (Total : " << n << ")" << endl; 
    cout << endl;

    cout << "Start / Goal" << endl;
    cout << "   ";
    for (int x = 0; x < width; x++) {
        cout << (x / 100 ? sfill(x / 100, 2) : "  ");
    }
    cout << "        ";
    for (int x = 0; x < width; x++) {
        cout << (x / 100 ? sfill(x / 100, 2) : "  ");
    }
    cout << endl;

    cout << "   ";
    for (int x = 0; x < width; x++) {
        cout << sfill(x % 100, 2);
    }
    cout << "        ";
    for (int x = 0; x < width; x++) {
        cout << sfill(x % 100, 2);
    }
    cout << endl;

    for (int y = 0; y < height; y++) {
        cout << sfill(y, 3);
        for (int x = 0; x < width; x++) {
            font::set_back_color(static_cast<font::Color>(start[y][x] + 1));
            cout << "  ";
        }
        font::reset();
        cout << "     " << sfill(y, 3);
        for (int x = 0; x < width; x++) {
            font::set_back_color(static_cast<font::Color>(goal[y][x] + 1));
            cout << "  ";
        }
        font::reset();
        cout << endl;
    }
    cout << endl;

    for (int p = 0; p < n; p++){
        cout << "Piece " << p << "(" << BoardTemplate::to_p_number(GENERAL, p) << ") : (Size : " << general_width[p] << " - " << general_height[p] << ")" << endl;
        cout << "   ";
        for (int x = 0; x < general_width[p]; x++) {
            cout << (x / 100 ? sfill(x / 100, 2) : "  ");
        }
        cout << endl;
        cout << "   ";
        for (int x = 0; x < general_width[p]; x++) {
            cout << sfill(x % 100, 2);
        }
        cout << endl;

        for (int y = 0; y < general_height[p]; y++) {
            cout << sfill(y, 3);
            for (int x = 0; x < general_width[p]; x++) {
                font::set_back_color(general_cells[p][y][x] ? font::magenta : font::cyan);
                cout << "  ";
            }
            font::reset();
            cout << endl;
        }
        cout << endl;
    }
    cout << "------------------------------------------------------" << endl;
}

const function<bool(int, int)> BoardTemplate::judge_func(Type type, int size, int x0, int y0) const {
    function<bool(int, int)> func;

    int left_lim = left_limit(type, size, x0, y0);
    int right_lim = right_limit(type, size, x0, y0);
    int up_lim = up_limit(type, size, x0, y0);
    int down_lim = down_limit(type, size, x0, y0);
    int parity;

    switch (type) {
    case GENERAL:
        func = [=, this](int x, int y) -> bool {
            if (left_lim <= x && x <= right_lim && up_lim <= y && y <= down_lim) {
                return static_cast<bool>(general_cells[size][y-y0][x-x0]);
            } else {
                return false;
            }
        };
        break;
    
    case I:
        func = [=](int x, int y) -> bool {
            return left_lim <= x && x <= right_lim && up_lim <= y && y <= down_lim;
        };
        break;

    case II:
        parity = (y0 % 2 == 0) ? 0 : 1;
        func = [=](int x, int y) -> bool {
            return left_lim <= x && x <= right_lim && up_lim <= y && y <= down_lim && y % 2 == parity;
        };
        break;

    case III:
        parity = (x0 % 2 == 0) ? 0 : 1;
        func = [=](int x, int y) -> bool {
            return left_lim <= x && x <= right_lim && up_lim <= y && y <= down_lim && x % 2 == parity;
        };
        break;
    }
    return func;
}

const function<int(int)> BoardTemplate::line_count(Type type, int size, int x0, int y0) const {
    function<int(int)> func;
    int left_lim = left_limit(type, size, x0, y0);
    int right_lim = right_limit(type, size, x0, y0);
    int up_lim = up_limit(type, size, x0, y0);
    int down_lim = down_limit(type, size, x0, y0);
    int parity;
    bool is_right_side;

    switch (type) {
    case GENERAL:
        func = [=, this](int y) -> int {
            if (up_lim <= y && y <= down_lim) {
                int result = 0;
                for (int x = left_lim; x <= right_lim; x++) {
                    result += static_cast<int>(general_cells[size][y-y0][x-x0]);
                }
                return result;
            } else {
                return 0;
            }
        };
        break;
    
    case I:
        func = [=](int y) -> int {
            return (up_lim <= y && y <= down_lim) ? (right_lim - left_lim + 1) : 0;
        };
        break;

    case II:
        parity = (y0 % 2 == 0) ? 0 : 1;
        func = [=](int y) -> int {
            return (up_lim <= y && y <= down_lim && y % 2 == parity) ? (right_lim - left_lim + 1) : 0;
        };
        break;

    case III:
        is_right_side = right_lim == (width - 1);
        func = [=](int y) -> int {
            if (is_right_side) {
                return (up_lim <= y && y <= down_lim) ? (right_lim - left_lim + 2) / 2 : 0;
            } else {
                return (up_lim <= y && y <= down_lim) ? (right_lim - left_lim + 1) / 2 : 0;
            }
        };
        break;
    }
    return func;
}

const function<int(int)> BoardTemplate::row_count(Type type, int size, int x0, int y0) const {
    function<int(int)> func;
    int left_lim = left_limit(type, size, x0, y0);
    int right_lim = right_limit(type, size, x0, y0);
    int up_lim = up_limit(type, size, x0, y0);
    int down_lim = down_limit(type, size, x0, y0);
    int parity;
    bool is_down_side;
    switch (type) {
    case GENERAL:
        func = [=, this](int x) -> int {
            if (left_lim <= x && x <= right_lim) {
                int result = 0;
                for (int y = up_lim; y <= down_lim; y++) {
                    result += static_cast<int>(general_cells[size][y-y0][x-x0]);
                }
                return result;
            } else {
                return 0;
            }
        };
        break;
    
    case I:
        func = [=](int x) -> int {
            return (left_lim <= x && x <= right_lim) ? (down_lim - up_lim + 1) : 0;
        };
        break;

    case II:
        is_down_side = down_lim == (height - 1);
        func = [=](int x) -> int {
            if (is_down_side) {
                return (left_lim <= x && x <= right_lim) ? (down_lim - up_lim + 2) / 2 : 0;
            } else {
                return (left_lim <= x && x <= right_lim) ? (down_lim - up_lim + 1) / 2 : 0;
            }
        };
        break;

    case III:
        parity = (x0 % 2 == 0) ? 0 : 1;
        func = [=](int x) -> int {
            return (left_lim <= x && x <= right_lim && x % 2 == parity) ? (down_lim - up_lim + 1) : 0;
        };
        break;
    }
    return func;
}

const Operation BoardTemplate::inverse(const Operation& operation) const {
    int x_size, y_size;
    int new_x, new_y;

    switch(transform_code) {
    // 変形なし
    case 0b000:
        new_x = operation.x;
        new_y = operation.y;
        break;
    // 右90°回転
    case 0b001:
        x_size = (operation.type == GENERAL ? general_width[operation.size] : operation.size);
        new_x = operation.y;
        new_y = (width - 1) - operation.x - (x_size - 1);
        break;
    // 180°回転
    case 0b010:
        x_size = (operation.type == GENERAL ? general_width[operation.size] : operation.size);
        y_size = (operation.type == GENERAL ? general_height[operation.size] : operation.size);
        new_x = (width - 1) - operation.x - (x_size - 1);
        new_y = (height - 1) - operation.y - (y_size - 1);
        break; 
    // 左90°回転
    case 0b011:
        y_size = (operation.type == GENERAL ? general_height[operation.size] : operation.size);
        new_x = (height - 1) - operation.y - (y_size - 1);
        new_y = operation.x;
        break;
    // 転置
    case 0b100:
        new_x = operation.y;
        new_y = operation.x;
        break;
    // 右90°回転, 転置
    case 0b101:
        y_size = (operation.type == GENERAL ? general_height[operation.size] : operation.size);
        new_x = operation.x;
        new_y = (height - 1) - operation.y - (y_size - 1);
        break;
    // 180°回転, 転置
    case 0b110:
        x_size = (operation.type == GENERAL ? general_width[operation.size] : operation.size);
        y_size = (operation.type == GENERAL ? general_height[operation.size] : operation.size);
        new_x = (height - 1) - operation.y - (x_size - 1);
        new_y = (width - 1) - operation.x - (y_size - 1);
        break; 
    // 左90°回転, 転置
    case 0b111:
        x_size = (operation.type == GENERAL ? general_width[operation.size] : operation.size);
        new_x = (width - 1) - operation.x - (x_size - 1);
        new_y = operation.y;
        break;
    }

    if (operation.type == II) {
        switch (transform_code) {
        case 0b011:
        case 0b110:
            new_x++;
            break;

        case 0b010:
        case 0b101:
            new_y++;
            break;
        }
    } else if (operation.type == III) {
        switch (transform_code) {
        case 0b010:
        case 0b111:
            new_x++;
            break;
        case 0b001:
        case 0b110:
            new_y++;
            break;
        }
    }
    return {type_dict.at(operation.type), operation.size, new_x, new_y, direction_dict.at(operation.direction)};
}

int BoardTemplate::to_p_number(Type type, int size) {
    if (type == GENERAL) {
        return size + FIXED_TYPE_NUM;
    } else {
        return size == 1 ? 0 : 3*(bit::log2_bitwise(size) - 1) + static_cast<int>(type);
    }
}

pair<Type, int> BoardTemplate::to_type_size(int p) {
    if (p == 0) {
        return make_pair<Type, int>(I, 1);
    } else if (1 <= p && p <= 24) {
        return make_pair<Type, int>(static_cast<Type>((p+2)%3 + 1), bit::power2_bitwise((p-1)/3 + 1));
    } else if (p <= 280) {
        return make_pair<Type, int>(GENERAL, p - FIXED_TYPE_NUM);
    } else {
        return make_pair<Type, int>(I, -1);
    }
}
