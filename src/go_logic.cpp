#include "go_logic.h"

GoLogic::GoLogic(int size) : size(size), currentTurn(Stone::BLACK), consecutivePasses(0) {
    board.assign(size, std::vector<Stone>(size, Stone::EMPTY));
    previousBoard = board;
}

bool GoLogic::isOutOfBounds(int r, int c) const {
    return r < 0 || r >= size || c < 0 || c >= size;
}

Stone GoLogic::getStone(int row, int col) const {
    if (isOutOfBounds(row, col)) return Stone::EMPTY;
    return board[row][col];
}

char GoLogic::getStoneChar(Stone s) {
    if (s == Stone::BLACK) return 'X';
    if (s == Stone::WHITE) return 'O';
    return '.';
}

std::string GoLogic::getStoneColor(Stone s) {
    if (s == Stone::BLACK) return "Black";
    if (s == Stone::WHITE) return "White";
    return "Empty";
}

void GoLogic::passTurn() {
    consecutivePasses++;
    previousBoard = board;
    currentTurn = (currentTurn == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
}

bool GoLogic::hasLiberty(int r, int c, Stone color, std::vector<std::vector<bool>>& visited) const {
    if (isOutOfBounds(r, c)) return false;
    if (board[r][c] == Stone::EMPTY) return true;
    if (board[r][c] != color) return false;
    if (visited[r][c]) return false;

    visited[r][c] = true;

    // Check 4 adjacent directions
    return hasLiberty(r + 1, c, color, visited) ||
           hasLiberty(r - 1, c, color, visited) ||
           hasLiberty(r, c + 1, color, visited) ||
           hasLiberty(r, c - 1, color, visited);
}

void GoLogic::removeChain(int r, int c, Stone color) {
    if (isOutOfBounds(r, c) || board[r][c] != color) return;
    board[r][c] = Stone::EMPTY;

    removeChain(r + 1, c, color);
    removeChain(r - 1, c, color);
    removeChain(r, c + 1, color);
    removeChain(r, c - 1, color);
}

int GoLogic::processCaptures(Stone opponentColor) {
    int capturedCount = 0;
    std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board[r][c] == opponentColor && !visited[r][c]) {
                std::vector<std::vector<bool>> tempVisited = visited;
                if (!hasLiberty(r, c, opponentColor, tempVisited)) {
                    // This chain has no liberties, capture it
                    removeChain(r, c, opponentColor);
                    capturedCount++; // Simplified counting
                } else {
                    // Copy tempVisited back into visited so we don't re-check these stones
                    for (int i = 0; i < size; ++i) {
                        for (int j = 0; j < size; ++j) {
                            if (tempVisited[i][j]) visited[i][j] = true;
                        }
                    }
                }
            }
        }
    }
    return capturedCount;
}

bool GoLogic::playMove(int row, int col, Stone player) {
    if (isGameOver()) return false;
    if (player != currentTurn) return false;
    if (isOutOfBounds(row, col) || board[row][col] != Stone::EMPTY) return false;

    std::vector<std::vector<Stone>> backupBoard = board;

    // Place stone
    board[row][col] = player;

    // Calculate opponent's color
    Stone opponent = (player == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;

    // Process captures
    processCaptures(opponent);

    // Check suicide rule (if we still have no liberties after captures, it's suicide)
    std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));
    if (!hasLiberty(row, col, player, visited)) {
        // Revert board and return false
        board = backupBoard;
        return false; // Suicide move
    }

    // Check Ko rule (board state cannot be exactly the same as the previous turn)
    bool isKo = true;
    for(int i=0; i<size; ++i) {
        for(int j=0; j<size; ++j) {
            if(board[i][j] != previousBoard[i][j]) {
                isKo = false;
                break;
            }
        }
        if(!isKo) break;
    }

    if (isKo) {
        board = backupBoard;
        return false; // Ko rule violation
    }

    // Move is valid
    previousBoard = backupBoard;
    consecutivePasses = 0;
    currentTurn = opponent;

    return true;
}
