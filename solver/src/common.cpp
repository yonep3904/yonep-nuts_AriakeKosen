
#include "common.hpp"

#include <iostream>
#include <iomanip>
#include <map>

using namespace std;

ostream& operator<<(ostream& os, const Type& type) {
    static map<Type, string> type_map = {
        {GENERAL, "GEN"},
        {I, "I"},
        {II, "II"},
        {III, "III"},
    };
    return os << type_map[type];
}

ostream& operator<<(ostream& os, const Direction& direction) {
    static map<Direction, string> direction_map = {
        {UP, "UP"},
        {DOWN, "DOWN"},
        {LEFT, "LEFT"},
        {RIGHT, "RIGHT"},
    };
    return os << direction_map[direction];
}

namespace bit {

    int bits_count(int n) {
        n = (n & 0x55555555) + (n >>  1 & 0x55555555);
        n = (n & 0x33333333) + (n >>  2 & 0x33333333);
        n = (n & 0x0f0f0f0f) + (n >>  4 & 0x0f0f0f0f);
        n = (n & 0x00ff00ff) + (n >>  8 & 0x00ff00ff);
        n = (n & 0x0000ffff) + (n >> 16 & 0x0000ffff);
        return n;
    }

    int log2_bitwise(int n) {
        [[assume(n > 0)]];
        int log = 0;
        while (n >>= 1) {
            ++log;
        }
        return log;
    }

    int upper_next_power_of_2(int n) {
        if (bit::is_power_of_2(n)) {
            return n;
        } else {
            n |= n >> 1;
            n |= n >> 2;
            n |= n >> 4;
            return n + 1;
        }
    }

    int downer_next_power_of_2(int n) {
        if (bit::is_power_of_2(n)) {
            return n;
        } else {
            n |= n >> 1;
            n |= n >> 2;
            n |= n >> 4;
            return (n + 1) >> 1;
        }
    }

}
