#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cctype>
#include <cstdlib>
#include <map>
#include <random>
#include <fstream>
#include <ctime>

#include "auth.h"
#include "go_logic.h"

std::mutex gameMutex;
GoLogic game(9);

struct PlayerSession {
    std::string token;
    std::string username;
    Stone color;
};

std::vector<PlayerSession> players;

std::string generateToken() {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(16);

    for (int i = 0; i < 16; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return tmp_s;
}

std::map<std::string, std::string> parseQueryParams(const std::string& path) {
    std::map<std::string, std::string> params;
    size_t qmark = path.find('?');
    if (qmark == std::string::npos) return params;

    std::string query = path.substr(qmark + 1);
    std::istringstream iss(query);
    std::string kv;
    while (std::getline(iss, kv, '&')) {
        size_t eq = kv.find('=');
        if (eq != std::string::npos) {
            params[kv.substr(0, eq)] = kv.substr(eq + 1);
        }
    }
    return params;
}

std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void sendHttpResponse(int sock, const std::string& body, const std::string& contentType = "application/json", int statusCode = 200) {
    std::string statusStr = "200 OK";
    if (statusCode == 404) statusStr = "404 Not Found";
    else if (statusCode == 405) statusStr = "405 Method Not Allowed";
    else if (statusCode == 400) statusStr = "400 Bad Request";

    std::ostringstream response;
    response << "HTTP/1.1 " << statusStr << "\r\n"
             << "Content-Type: " << contentType << "\r\n"
             << "Access-Control-Allow-Origin: *\r\n"
             << "Cache-Control: no-cache, no-store, must-revalidate\r\n"
             << "Content-Length: " << body.length() << "\r\n"
             << "Connection: close\r\n\r\n"
             << body;
             
    std::string respStr = response.str();
    send(sock, respStr.c_str(), respStr.length(), 0);
}

void handleRequest(int sock) {
    char buffer[4096] = {0};
    int valread = read(sock, buffer, 4096);
    if (valread <= 0) {
        close(sock);
        return;
    }

    std::string request(buffer);
    std::istringstream reqStream(request);
    std::string method, path, version;
    reqStream >> method >> path >> version;

    if (method != "GET") {
        sendHttpResponse(sock, "{\"error\": \"Method not allowed\"}", "application/json", 405);
        close(sock);
        return;
    }

    std::map<std::string, std::string> params = parseQueryParams(path);
    std::string endpoint = path.substr(0, path.find('?'));

    std::lock_guard<std::mutex> lock(gameMutex);

    if (endpoint == "/" || endpoint == "/index.html") {
        std::string content = readFile("web/index.html");
        sendHttpResponse(sock, content, "text/html");
    }
    else if (endpoint == "/style.css") {
        std::string content = readFile("web/style.css");
        sendHttpResponse(sock, content, "text/css");
    }
    else if (endpoint == "/app.js") {
        std::string content = readFile("web/app.js");
        sendHttpResponse(sock, content, "application/javascript");
    }
    else if (endpoint == "/login") {
        std::string u = params["u"];
        std::string p = params["p"];
        
        if (Auth::loginUser(u, p) || Auth::registerUser(u, p)) {
            // Check if user is already in the game
            bool found = false;
            for (auto& pl : players) {
                if (pl.username == u) {
                    sendHttpResponse(sock, "{\"status\": \"ok\", \"token\": \"" + pl.token + "\", \"color\": \"" + GoLogic::getStoneColor(pl.color) + "\"}");
                    found = true;
                    break;
                }
            }

            if (!found) {
                if (players.size() >= 2) {
                    sendHttpResponse(sock, "{\"status\": \"error\", \"msg\": \"Oda dolu.\"}");
                } else {
                    PlayerSession sess;
                    sess.token = generateToken();
                    sess.username = u;
                    if (players.empty()) {
                        sess.color = Stone::BLACK;
                    } else {
                        sess.color = (players[0].color == Stone::BLACK) ? Stone::WHITE : Stone::BLACK;
                    }
                    players.push_back(sess);
                    sendHttpResponse(sock, "{\"status\": \"ok\", \"token\": \"" + sess.token + "\", \"color\": \"" + GoLogic::getStoneColor(sess.color) + "\"}");
                }
            }
        } else {
            sendHttpResponse(sock, "{\"status\": \"error\", \"msg\": \"Yanlis sifre.\"}");
        }
    } 
    else if (endpoint == "/status") {
        std::string token = params["token"];
        Stone myColor = Stone::EMPTY;
        for (auto& pl : players) {
            if (pl.token == token) myColor = pl.color;
        }

        std::ostringstream json;
        json << "{";
        json << "\"board\": [";
        for (int r = 0; r < game.getSize(); ++r) {
            json << "[";
            for (int c = 0; c < game.getSize(); ++c) {
                json << "\"" << GoLogic::getStoneColor(game.getStone(r, c)) << "\"";
                if (c < game.getSize() - 1) json << ",";
            }
            json << "]";
            if (r < game.getSize() - 1) json << ",";
        }
        json << "],";
        json << "\"turn\": \"" << GoLogic::getStoneColor(game.getCurrentTurn()) << "\",";
        json << "\"myColor\": \"" << GoLogic::getStoneColor(myColor) << "\",";
        json << "\"gameOver\": " << (game.isGameOver() ? "true" : "false") << ",";
        json << "\"playersReady\": " << (players.size() == 2 ? "true" : "false");
        json << "}";

        sendHttpResponse(sock, json.str());
    }
    else if (endpoint == "/move") {
        std::string token = params["token"];
        int r = std::stoi(params["r"]);
        int c = std::stoi(params["c"]);

        Stone myColor = Stone::EMPTY;
        for (auto& pl : players) {
            if (pl.token == token) myColor = pl.color;
        }

        if (myColor == Stone::EMPTY || players.size() < 2) {
            sendHttpResponse(sock, "{\"status\": \"error\", \"msg\": \"Not your turn or game not started\"}");
        } else {
            if (game.playMove(r, c, myColor)) {
                sendHttpResponse(sock, "{\"status\": \"ok\"}");
            } else {
                sendHttpResponse(sock, "{\"status\": \"error\", \"msg\": \"Gecersiz hamle.\"}");
            }
        }
    }
    else if (endpoint == "/pass") {
        std::string token = params["token"];
        Stone myColor = Stone::EMPTY;
        for (auto& pl : players) {
            if (pl.token == token) myColor = pl.color;
        }

        if (myColor != Stone::EMPTY && game.getCurrentTurn() == myColor && players.size() == 2) {
            game.passTurn();
            sendHttpResponse(sock, "{\"status\": \"ok\"}");
        } else {
            sendHttpResponse(sock, "{\"status\": \"error\", \"msg\": \"Not your turn.\"}");
        }
    }
    else if (endpoint == "/reset") {
        // Optional endpoint for testing
        game = GoLogic(9);
        players.clear();
        sendHttpResponse(sock, "{\"status\": \"ok\"}");
    }
    else {
        sendHttpResponse(sock, "{\"status\": \"error\", \"msg\": \"Endpoint not found\"}", "application/json", 404);
    }

    close(sock);
}

int main() {
    srand(time(NULL));
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        std::exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        std::exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        std::exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        std::exit(EXIT_FAILURE);
    }

    std::cout << "HTTP Server is listening on port 8080..." << std::endl;

    while (true) {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        std::thread clientThread(handleRequest, new_socket);
        clientThread.detach();
    }

    return 0;
}
