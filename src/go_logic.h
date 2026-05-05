#ifndef GO_LOGIC_H
#define GO_LOGIC_H

#include <vector>
#include <string>

enum class Stone { EMPTY, BLACK, WHITE };

class GoLogic {
public:
    GoLogic(int size = 9);

    // Attempts to play a move. Returns true if successful, false if illegal.
    bool playMove(int row, int col, Stone player);
    
    // Player passes their turn
    void passTurn();

    // Get the stone at a given position
    Stone getStone(int row, int col) const;

    int getSize() const { return size; }
    Stone getCurrentTurn() const { return currentTurn; }
    bool isGameOver() const { return consecutivePasses >= 2; }

    // Helpers to convert enum to string/char for UI
    static char getStoneChar(Stone s);
    static std::string getStoneColor(Stone s);

private:
    int size;
    std::vector<std::vector<Stone>> board;
    std::vector<std::vector<Stone>> previousBoard;
    Stone currentTurn;
    int consecutivePasses;

    // Internal mechanics
    bool isOutOfBounds(int r, int c) const;
    
    // Removes captured stones and returns how many were removed
    int processCaptures(Stone opponent);
    
    // Checks if a specific chain of stones has at least one liberty
    bool hasLiberty(int r, int c, Stone color, std::vector<std::vector<bool>>& visited) const;
    
    // Removes a specific chain of stones from the board
    void removeChain(int r, int c, Stone color);
    

};

#endif // GO_LOGIC_H
