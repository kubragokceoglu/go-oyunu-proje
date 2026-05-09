#ifndef GO_LOGIC_H
#define GO_LOGIC_H

#include <vector>
#include <string>

enum class Stone { EMPTY, BLACK, WHITE };

class GoLogic {
public:
    GoLogic(int size = 9);

    // Hamle yapmayı dener. Başarılı olursa true, geçersiz bir hamleyse false döner.
    bool playMove(int row, int col, Stone player);
    
    // Oyuncunun pas geçmesini sağlar
    void passTurn();

    // Belirli bir konumdaki taşı getirir
    Stone getStone(int row, int col) const;

    int getSize() const { return size; }
    Stone getCurrentTurn() const { return currentTurn; }
    bool isGameOver() const { return consecutivePasses >= 2; }

    // Arayüz için enum değerlerini string/char tipine dönüştüren yardımcı fonksiyonlar
    static char getStoneChar(Stone s);
    static std::string getStoneColor(Stone s);

private:
    int size;
    std::vector<std::vector<Stone>> board;
    std::vector<std::vector<Stone>> previousBoard;
    Stone currentTurn;
    int consecutivePasses;

    // Oyun motoru iç işleyiş mekanizmaları
    bool isOutOfBounds(int r, int c) const;
    
    // Esir alınan taşları tahtadan kaldırır ve kaç taş kaldırıldığını döndürür
    int processCaptures(Stone opponent);
    
    // Belirli bir taş grubunun (zincirin) en az bir nefesi (boşluğu) olup olmadığını kontrol eder
    bool hasLiberty(int r, int c, Stone color, std::vector<std::vector<bool>>& visited) const;
    
    // Belirli bir taş grubunu (zinciri) tahtadan siler
    void removeChain(int r, int c, Stone color);
    

};

#endif // GO_LOGIC_H
