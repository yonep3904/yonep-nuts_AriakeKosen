#ifndef STATE_HPP
#define STATE_HPP

#include "board.hpp"
#include "solver.hpp"
#include "point_xy.hpp"

struct State {
    solver::evaluater::ScoreType score;
    Board board;
    int depth;
    PointXY first_action;

    State();
    State(Board board, PointXY first_action);
    State(Board board, State state);

    // scoreの比較(等号, 否定等号, 不等号)が可能
    bool operator==(const BeamNode& other) const;
    bool operator!=(const BeamNode& other) const;
    bool operator<(const BeamNode& other) const;
    bool operator<=(const BeamNode& other) const;
    bool operator>(const BeamNode& other) const;
    bool operator>=(const BeamNode& other) const;
        
};

inline State::State(Board board, PointXY first_action):
    score(solver::evaluater::count_connected(board, {0, 0})),
    board(board),
    depth(0),
    first_action(first_action)
{

}

inline State::State(Board board, State state):
    score(solver::evaluater::count_connected(board, {0, 0})),
    board(board),
    depth(state.depth + 1),
    first_action(state.first_action)
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