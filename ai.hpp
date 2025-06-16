#pragma once
#include "board.hpp"

class AI {
public:
    static int findBest(const Board& board, Board::Cell aiPlayer);
};