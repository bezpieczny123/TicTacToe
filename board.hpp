#pragma once

#include <vector>

class Board {
public:
    enum Cell { EMPTY = 0, X = 1, O = 2 };

    explicit Board(const int n) : size(n), cells(n * n, EMPTY) {}

    bool makeMove(int idx, Cell p);

    bool isFull() const;

    int checkWin() const;

public:
    int size;
    std::vector<int> cells;
};