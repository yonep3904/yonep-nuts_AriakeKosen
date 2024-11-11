#ifndef STATE_HPP
#define STATE_HPP

#include "board.hpp"
#include "solver.hpp"
#include "point_xy.hpp"

struct State {
    int score;
    PointXY source;

    State();
    State(int score, const PointXY& source);

    // scoreの比較(等号, 否定等号, 不等号)が可能
    bool operator==(const State& other) const;
    bool operator!=(const State& other) const;
    bool operator<(const State& other) const;
    bool operator<=(const State& other) const;
    bool operator>(const State& other) const;
    bool operator>=(const State& other) const;
        
};

inline State::State():
    score(-1),
    source({-1, -1})
{

}

inline State::State(int score, const PointXY& source):
    score(score),
    source(source)
{

}

inline bool State::operator==(const State& other) const {
    return score == other.score;
}

inline bool State::operator!=(const State& other) const {
    return score != other.score;
}

inline bool State::operator<(const State& other) const {
    return score < other.score;
}

inline bool State::operator<=(const State& other) const {
    return score <= other.score;
}

inline bool State::operator>(const State& other) const {
    return score > other.score;
}

inline bool State::operator>=(const State& other) const {
    return score >= other.score;
}

#endif