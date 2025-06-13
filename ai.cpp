#include "ai.hpp"
#include <algorithm>
#include <limits>
#include <vector>
#include <random>
#include <chrono>

const int MAX_AI_DEPTH = 7;

namespace {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    int evaluatePosition(const Board& board, int position) {
        int row = position / board.size;
        int col = position % board.size;
        int center = board.size / 2;

        int centerBonus = 0;
        if (row == center && col == center) centerBonus = 4;
        else if (abs(row - center) <= 1 && abs(col - center) <= 1) centerBonus = 2;

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

    int checkImmediateWinOrBlock(const Board& board, int position, Board::Cell player) {
        Board testBoard = board;
        testBoard.makeMove(position, player);
        int winner = testBoard.checkWin();
        if (winner == Board::O) return 1000;
        if (winner == Board::X) return -1000;
        return 0;
    }

    int minimax_ab(Board board, int depth, bool maximizingPlayer, int alpha, int beta) {
        int winner = board.checkWin();
        if (winner == Board::O) return 10 - depth;
        if (winner == Board::X) return -10 + depth;
        if (board.isFull()) return 0;
        if (depth >= MAX_AI_DEPTH) return 0;

        if (maximizingPlayer) {
            int maxEval = std::numeric_limits<int>::min();
            for (int i = 0; i < board.size * board.size; ++i) {
                if (board.cells[i] == Board::EMPTY) {
                    Board nextBoard = board;
                    nextBoard.makeMove(i, Board::O);
                    int eval = minimax_ab(nextBoard, depth + 1, false, alpha, beta);
                    maxEval = std::max(maxEval, eval);
                    alpha = std::max(alpha, eval);
                    if (beta <= alpha) break;
                }
            }
            return maxEval;
        } else {
            int minEval = std::numeric_limits<int>::max();
            for (int i = 0; i < board.size * board.size; ++i) {
                if (board.cells[i] == Board::EMPTY) {
                    Board nextBoard = board;
                    nextBoard.makeMove(i, Board::X);
                    int eval = minimax_ab(nextBoard, depth + 1, true, alpha, beta);
                    minEval = std::min(minEval, eval);
                    beta = std::min(beta, eval);
                    if (beta <= alpha) break;
                }
            }
            return minEval;
        }
    }
}

int AI::findBest(const Board& board) {
    std::vector<int> availableMoves;
    for (int i = 0; i < board.size * board.size; ++i) {
        if (board.cells[i] == Board::EMPTY) availableMoves.push_back(i);
    }
    if (availableMoves.empty()) return -1;

    for (int move : availableMoves) {
        if (checkImmediateWinOrBlock(board, move, Board::O) == 1000) return move;
    }

    for (int move : availableMoves) {
        if (checkImmediateWinOrBlock(board, move, Board::X) == -1000) return move;
    }

    std::vector<int> moveScores;
    int bestScore = std::numeric_limits<int>::min();
    for (int move : availableMoves) {
        Board nextBoard = board;
        nextBoard.makeMove(move, Board::O);
        int score = minimax_ab(nextBoard, 0, false, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        score += evaluatePosition(board, move);
        moveScores.push_back(score);
        bestScore = std::max(bestScore, score);
    }

    std::vector<int> bestMoves;
    for (size_t i = 0; i < availableMoves.size(); ++i) {
        if (moveScores[i] == bestScore) bestMoves.push_back(availableMoves[i]);
    }

    std::uniform_int_distribution<int> dist(0, bestMoves.size() - 1);
    return bestMoves[dist(rng)];
}