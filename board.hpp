#pragma once
#include <vector>

class Board {
public:
    enum Cell { EMPTY = 0, X = 1, O = 2 };

    explicit Board(int n) : size(n), cells(n * n, EMPTY) {}

    bool makeMove(int idx, Cell p) {
        if (idx < 0 || idx >= size * size || cells[idx] != EMPTY) return false;
        cells[idx] = p;
        return true;
    }

    bool isFull() const {
        for (int c : cells) if (c == EMPTY) return false;
        return true;
    }

    int checkWin() const {
        for (int r = 0; r < size; ++r) {
            int first = cells[r * size];
            if (first == EMPTY) continue;
            bool win = true;
            for (int c = 1; c < size; ++c) {
                if (cells[r * size + c] != first) { win = false; break; }
            }
            if (win) return first;
        }

        for (int c = 0; c < size; ++c) {
            int first = cells[c];
            if (first == EMPTY) continue;
            bool win = true;
            for (int r = 1; r < size; ++r) {
                if (cells[r * size + c] != first) { win = false; break; }
            }
            if (win) return first;
        }

        int first = cells[0];
        if (first != EMPTY) {
            bool win = true;
            for (int i = 1; i < size; ++i) {
                if (cells[i * size + i] != first) { win = false; break; }
            }
            if (win) return first;
        }

        first = cells[size - 1];
        if (first != EMPTY) {
            bool win = true;
            for (int i = 1; i < size; ++i) {
                if (cells[i * size + (size - 1 - i)] != first) { win = false; break; }
            }
            if (win) return first;
        }

        return EMPTY;
    }

public:
    int size;
    std::vector<int> cells;
};