#include <SDL3/SDL.h>
#include "board.hpp"
#include "ai.hpp"
#include <cmath>

enum class GameState { PLAYING, GAME_OVER };

constexpr int WINDOW_W = 1200;
constexpr int WINDOW_H = 1200;
constexpr int BOARD_SIZE = 5;

class Game {
public:
    Game(int boardSize, int windowWidth, int windowHeight)
        : board(boardSize), state(GameState::PLAYING), playerTurn(false), winner(0),
          window(nullptr), renderer(nullptr), windowWidth(windowWidth), windowHeight(windowHeight),
          cellW(windowWidth / boardSize), cellH(windowHeight / boardSize),
          btn{ windowWidth / 2.0f - 100.0f, windowHeight / 2.0f + 20.0f, 200.0f, 50.0f }
    {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow("TicTacToe SDL3", windowWidth, windowHeight, 0);
        renderer = SDL_CreateRenderer(window, nullptr);
    }

    ~Game() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void run() {
        bool running = true;
        while (running) {
            handleEvents(running);
            if (state == GameState::PLAYING) {
                update();
            }
            render();
        }
    }

private:
    void handleEvents(bool& running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (state == GameState::PLAYING && playerTurn && e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
                int x = e.button.x / cellW;
                int y = e.button.y / cellH;
                int idx = y * board.size + x;
                if (board.makeMove(idx, Board::X)) {
                    playerTurn = false;
                }
            } else if (state == GameState::GAME_OVER && e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_LEFT) {
                int mx = e.button.x, my = e.button.y;
                if (mx >= btn.x && mx <= btn.x + btn.w && my >= btn.y && my <= btn.y + btn.h) {
                    reset();
                }
            }
        }
    }

    void update() {
        if (!playerTurn && !board.isFull() && !board.checkWin()) {
            int mv = AI::findBest(board);
            board.makeMove(mv, Board::O);
            playerTurn = true;
        }
        if (board.checkWin() || board.isFull()) {
            state = GameState::GAME_OVER;
            winner = board.checkWin();
        }
    }

    void render() {
        if (state == GameState::PLAYING) {
            SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            drawGrid();
            drawBoard();
            SDL_RenderPresent(renderer);
        } else {
            renderGameOver();
            SDL_RenderPresent(renderer);
        }
    }

    void drawGrid() {
        for (int i = 1; i < board.size; ++i) {
            SDL_RenderLine(renderer, i * cellW, 0, i * cellW, windowHeight);
            SDL_RenderLine(renderer, 0, i * cellH, windowWidth, i * cellH);
        }
    }

    void drawBoard() {
        for (int i = 0; i < board.size * board.size; ++i) {
            int vx = (i % board.size) * cellW;
            int vy = (i / board.size) * cellH;
            if (board.cells[i] == Board::X) {
                SDL_RenderLine(renderer, vx + 10, vy + 10, vx + cellW - 10, vy + cellH - 10);
                SDL_RenderLine(renderer, vx + cellW - 10, vy + 10, vx + 10, vy + cellH - 10);
            } else if (board.cells[i] == Board::O) {
                int cx = vx + cellW / 2, cy = vy + cellH / 2;
                int rad = std::min(cellW, cellH) / 2 - 10;
                for (int a = 0; a < 360; a += 10) {
                    SDL_RenderLine(renderer,
                        cx + cos(a * M_PI / 180.0) * rad, cy + sin(a * M_PI / 180.0) * rad,
                        cx + cos((a + 10) * M_PI / 180.0) * rad, cy + sin((a + 10) * M_PI / 180.0) * rad
                    );
                }
            }
        }
    }

    void renderGameOver() {
        if (winner == Board::X) SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        else if (winner == Board::O) SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        else SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
        SDL_RenderFillRect(renderer, &btn);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderRect(renderer, &btn);
    }

    void reset() {
        board = Board(board.size);
        state = GameState::PLAYING;
        playerTurn = false;
        winner = 0;
    }

private:
    Board board;
    GameState state;
    bool playerTurn;
    int winner;
    SDL_Window* window;
    SDL_Renderer* renderer;
    int windowWidth;
    int windowHeight;
    int cellW;
    int cellH;
    const SDL_FRect btn;
};

int main() {
    Game game(BOARD_SIZE, WINDOW_W, WINDOW_H);
    game.run();
    return 0;
}