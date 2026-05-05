#ifndef AUTH_H
#define AUTH_H

#include <string>

class Auth {
public:
    // Registers a new user. Returns true if successful, false if user exists.
    static bool registerUser(const std::string& username, const std::string& password);

    // Authenticates a user. Returns true if valid, false otherwise.
    static bool loginUser(const std::string& username, const std::string& password);

private:
    static const std::string DB_FILE;
};

#endif // AUTH_H
