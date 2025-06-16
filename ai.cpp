#include "ai.hpp"
#include <algorithm>
#include <limits>
#include <vector>
#include <random>
#include <chrono>

namespace {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    int evaluatePosition(const Board& board, const int position) {
        int row = position / board.size;
        int col = position % board.size;
        int center = board.size / 2;

        int centerBonus = 0;
        if (board.size % 2 == 1) {
            if (row == center && col == center) centerBonus = 4;
            else if (abs(row - center) <= 1 && abs(col - center) <= 1) centerBonus = 2;
        }

        int cornerBonus = 0;
        if ((row == 0 || row == board.size - 1) && (col == 0 || col == board.size - 1)) {
            cornerBonus = 3;
        }

        int edgeBonus = 0;
        if ((row == 0 || row == board.size - 1 || col == 0 || col == board.size - 1) && cornerBonus == 0) {
            edgeBonus = 1;
        }

        return centerBonus + cornerBonus + edgeBonus;
    }

    bool wouldWin(const Board& board, int position, Board::Cell player) {
        Board testBoard = board;
        testBoard.makeMove(position, player);
        int winner = testBoard.checkWin();
        return (winner == player);
    }

    int minimax_ab(Board board, int depth, bool maximizingPlayer, int alpha, int beta, Board::Cell aiPlayer) {
        int maxDepth = std::max(1, 30 / board.size);

        int winner = board.checkWin();
        if (winner == aiPlayer) return 1000 - depth;
        if (winner != Board::EMPTY && winner != aiPlayer) return -1000 + depth;
        if (board.isFull()) return 0;
        if (depth >= maxDepth) return 0;

        if (maximizingPlayer) {
            int maxEval = std::numeric_limits<int>::min();
            for (int i = 0; i < board.size * board.size; ++i) {
                if (board.cells[i] == Board::EMPTY) {
                    Board nextBoard = board;
                    nextBoard.makeMove(i, aiPlayer);
                    int eval = minimax_ab(nextBoard, depth + 1, false, alpha, beta, aiPlayer);
                    maxEval = std::max(maxEval, eval);
                    alpha = std::max(alpha, eval);
                    if (beta <= alpha) break;
                }
            }
            return maxEval;
        } else {
            int minEval = std::numeric_limits<int>::max();
            Board::Cell opponent = (aiPlayer == Board::O) ? Board::X : Board::O;
            for (int i = 0; i < board.size * board.size; ++i) {
                if (board.cells[i] == Board::EMPTY) {
                    Board nextBoard = board;
                    nextBoard.makeMove(i, opponent);
                    int eval = minimax_ab(nextBoard, depth + 1, true, alpha, beta, aiPlayer);
                    minEval = std::min(minEval, eval);
                    beta = std::min(beta, eval);
                    if (beta <= alpha) break;
                }
            }
            return minEval;
        }
    }
}

int AI::findBest(const Board& board, Board::Cell aiPlayer) {
    std::vector<int> availableMoves;
    for (int i = 0; i < board.size * board.size; ++i) {
        if (board.cells[i] == Board::EMPTY) availableMoves.push_back(i);
    }
    if (availableMoves.empty()) return -1;

    Board::Cell opponent = (aiPlayer == Board::O) ? Board::X : Board::O;

    for (int move : availableMoves) {
        if (wouldWin(board, move, aiPlayer)) {
            return move;
        }
    }

    for (int move : availableMoves) {
        if (wouldWin(board, move, opponent)) {
            return move;
        }
    }

    std::vector<int> moveScores;
    int bestScore = std::numeric_limits<int>::min();
    for (int move : availableMoves) {
        Board nextBoard = board;
        nextBoard.makeMove(move, aiPlayer);
        int score = minimax_ab(nextBoard, 0, false, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), aiPlayer);
        //score += evaluatePosition(board, move);
        moveScores.push_back(score);
        bestScore = std::max(bestScore, score);
    }

    std::vector<int> bestMoves;
    for (size_t i = 0; i < availableMoves.size(); ++i) {
        if (moveScores[i] == bestScore) {
            bestMoves.push_back(availableMoves[i]);
        }
    }

    std::uniform_int_distribution<int> dist(0, bestMoves.size() - 1);
    return bestMoves[dist(rng)];
}