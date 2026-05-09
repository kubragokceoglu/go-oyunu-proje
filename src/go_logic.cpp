#include "go_logic.h"

// Sınıfın kurucusu (Constructor): Oyun ilk başladığında tahtayı hazırlar
GoLogic::GoLogic(int size) : size(size), currentTurn(Stone::BLACK), consecutivePasses(0) {
    // Tahtayı (matrisi) belirtilen boyutta (9x9) tamamen BOŞ (EMPTY) taşlarla doldur
    board.assign(size, std::vector<Stone>(size, Stone::EMPTY));
    // Başlangıçta eski tahta durumu da şimdikiyle aynıdır (tamamen boş)
    previousBoard = board;
}

// Verilen satır (r) ve sütun (c) koordinatlarının 9x9 tahtanın dışına taşıp taşmadığını kontrol eder
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

// BİR TAŞIN VEYA ZİNCİRİN NEFESİ (BOŞLUĞU) VAR MI? (Derinlik Öncelikli Arama - DFS Algoritması)
bool GoLogic::hasLiberty(int r, int c, Stone color, std::vector<std::vector<bool>>& visited) const {
    // 1. Eğer koordinat tahtanın dışındaysa oradan nefes alınmaz (false)
    if (isOutOfBounds(r, c)) return false;
    // 2. Eğer yan hücre BOŞ ise, harika! Nefes bulduk demektir (true)
    if (board[r][c] == Stone::EMPTY) return true;
    // 3. Yan hücrede rakip taş varsa, orası kapalıdır (false)
    if (board[r][c] != color) return false;
    // 4. Sonsuz döngüye girmemek için, daha önce kontrol ettiğimiz taşa tekrar bakma
    if (visited[r][c]) return false;

    // Bu taşı "kontrol edildi" olarak işaretle
    visited[r][c] = true;

    // REKÜRSİF (Özyinelemeli) ÇAĞRI: Kendi komşularına sor. Herhangi birinden 1 tane bile true dönerse zincir yaşıyordur.
    // 4 komşu yöne (sağ, sol, yukarı, aşağı) bak
    return hasLiberty(r + 1, c, color, visited) ||
           hasLiberty(r - 1, c, color, visited) ||
           hasLiberty(r, c + 1, color, visited) ||
           hasLiberty(r, c - 1, color, visited);
}

// ESİR ALINAN TAŞLARI SİLME ALGORİTMASI
// Eğer bir zincirin nefesi bitmişse, bu fonksiyon o taşı ve ona bağlı tüm kendi taşlarını tahtadan siler (EMPTY yapar)
void GoLogic::removeChain(int r, int c, Stone color) {
    if (isOutOfBounds(r, c) || board[r][c] != color) return;
    board[r][c] = Stone::EMPTY; // Taşı sil

    // Bağlı olduğu diğer tüm aynı renk taşlara da bulaşarak (virüs gibi) onları da sil
    removeChain(r + 1, c, color);
    removeChain(r - 1, c, color);
    removeChain(r, c + 1, color);
    removeChain(r, c - 1, color);
}

// HAMLE SONRASI RAKİBİN ESİR DÜŞEN TAŞLARINI TESPİT ETME
int GoLogic::processCaptures(Stone opponentColor) {
    int capturedCount = 0;
    // Ziyaret edilen taşların hafızası (Başta hepsi false)
    std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));

    // Tahtadaki her bir kareyi satır satır ve sütun sütun tararız
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board[r][c] == opponentColor && !visited[r][c]) {
                std::vector<std::vector<bool>> tempVisited = visited;
                if (!hasLiberty(r, c, opponentColor, tempVisited)) {
                    // Bu grubun hiç nefesi yok, esir al
                    removeChain(r, c, opponentColor);
                    capturedCount++; // Esir alınan taş sayısını artır
                } else {
                    // Ziyaret edilenleri asıl listeye kopyala ki aynı taşları tekrar kontrol etmeyelim
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

// ANA HAMLE FONKSİYONU: Bir oyuncu taş koymak istediğinde tüm kuralları burada işletiriz
bool GoLogic::playMove(int row, int col, Stone player) {
    if (isGameOver()) return false; // Oyun bittiyse hamle yapılamaz
    if (player != currentTurn) return false; // Sıra onda değilse hamle yapılamaz
    if (isOutOfBounds(row, col) || board[row][col] != Stone::EMPTY) return false; // Tahta dışı veya dolu yer seçildiyse geçersiz

    // Hamle geçersiz olursa tahtayı geri döndürebilmek için mevcut durumu yedekle
    std::vector<std::vector<Stone>> backupBoard = board;

    // Taşı tahtaya yerleştir
    board[row][col] = player;

    // Rakibin rengini hesapla
    Stone opponent = (player == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;

    // Rakibin esir düşen taşlarını temizle
    processCaptures(opponent);

    // İntihar kuralı (esir aldıktan sonra bile nefesimiz yoksa intihardır)
    std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));
    if (!hasLiberty(row, col, player, visited)) {
        // Tahtayı eski haline getir ve geçersiz hamle (false) dön
        board = backupBoard;
        return false; // İntihar hamlesi
    }

    // Ko kuralı (tahtanın yeni durumu bir önceki turla birebir aynı olamaz)
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
        return false; // Ko kuralı ihlali
    }

    // Hamle geçerli, durumu kaydet
    previousBoard = backupBoard;
    consecutivePasses = 0;
    currentTurn = opponent;

    return true;
}
