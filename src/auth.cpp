#include "auth.h"
#include <fstream>
#include <sstream>
#include <iostream>

const std::string Auth::DB_FILE = "data/users.txt";

bool Auth::registerUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) return false;

    std::ifstream inFile(DB_FILE);
    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string u, p;
        if (iss >> u >> p) {
            if (u == username) {
                return false; // User already exists
            }
        }
    }
    inFile.close();

    std::ofstream outFile(DB_FILE, std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "Cannot open database file." << std::endl;
        return false;
    }
    outFile << username << " " << password << "\n";
    outFile.close();
    return true;
}

bool Auth::loginUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) return false;

    std::ifstream inFile(DB_FILE);
    std::string line;
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string u, p;
        if (iss >> u >> p) {
            if (u == username && p == password) {
                return true;
            }
        }
    }
    return false;
}
