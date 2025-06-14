#include "board.hpp"


bool Board::makeMove(int idx, Cell p) {
    if (idx < 0 || idx >= size * size || cells[idx] != EMPTY) {
        return false;
    }
    cells[idx] = p;
    return true;
}

bool Board::isFull() const {
    for (int c : cells) {
        if (c == EMPTY) {
            return false;
        }
    }
    return true;
}

int Board::checkWin() const {
    for (int r = 0; r < size; ++r) {
        int first = cells[r * size];
        if (first == EMPTY) continue;
        bool win = true;
        for (int c = 1; c < size; ++c) {
            if (cells[r * size + c] != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    for (int c = 0; c < size; ++c) {
        int first = cells[c];
        if (first == EMPTY) continue;
        bool win = true;
        for (int r = 1; r < size; ++r) {
            if (cells[r * size + c] != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    int first = cells[0];
    if (first != EMPTY) {
        bool win = true;
        for (int i = 1; i < size; ++i) {
            if (cells[i * size + i] != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    first = cells[size - 1];
    if (first != EMPTY) {
        bool win = true;
        for (int i = 1; i < size; ++i) {
            if (cells[i * size + (size - 1 - i)] != first) {
                win = false;
                break;
            }
        }
        if (win) return first;
    }

    return EMPTY;
}