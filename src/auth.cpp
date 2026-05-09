#include "auth.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Veritabanı yerine kullandığımız basit metin dosyasının konumu
const std::string Auth::DB_FILE = "data/users.txt";

// Yeni bir oyuncu kayıt olmak istediğinde bu fonksiyon çalışır
bool Auth::registerUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) return false;

    // 1. ADIM: Dosyayı okuma (read) modunda aç
    std::ifstream inFile(DB_FILE);
    std::string line;
    // Dosyayı satır satır sonuna kadar oku
    while (std::getline(inFile, line)) {
        std::istringstream iss(line); // O satırdaki kelimeleri ayırmak için string akışı oluştur
        std::string u, p;
        // Satırdaki ilk kelimeyi 'u' (username), ikinciyi 'p' (password) içine at
        if (iss >> u >> p) {
            // Eğer veritabanındaki isim, yeni kayıt olmak isteyenin ismiyle aynıysa reddet
            if (u == username) {
                return false; // Kullanıcı zaten var
            }
        }
    }
    inFile.close();

    // 2. ADIM: İsim dosyada yoksa, dosyayı ekleme (append/app) modunda yazmak için aç
    // ios::app -> Mevcut verileri silmeden en alta ekleme yapar
    std::ofstream outFile(DB_FILE, std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "Cannot open database file." << std::endl;
        return false;
    }
    // Kullanıcı adı ve şifreyi aralarında boşluk bırakarak dosyaya yaz ve alt satıra geç (\n)
    outFile << username << " " << password << "\n";
    outFile.close();
    return true; // Kayıt başarılı!
}

// Oyuncu giriş yapmak istediğinde çalışır
bool Auth::loginUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) return false;

    // Dosyayı oku
    std::ifstream inFile(DB_FILE);
    std::string line;
    // Satır satır tüm kullanıcıları tara
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string u, p;
        // u = satırdaki isim, p = satırdaki şifre
        if (iss >> u >> p) {
            // Eğer hem isim hem de şifre eşleşirse giriş başarılıdır!
            if (u == username && p == password) {
                return true;
            }
        }
    }
    // Dosya bitti ama isim-şifre eşleşmediyse yanlış şifredir
    return false;
}
