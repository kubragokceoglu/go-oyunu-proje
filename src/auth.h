#ifndef AUTH_H
#define AUTH_H

#include <string>

class Auth {
public:
    // Yeni bir kullanıcı kaydeder. Başarılıysa true, kullanıcı zaten varsa false döner.
    static bool registerUser(const std::string& username, const std::string& password);

    // Bir kullanıcının girişini doğrular. Şifre doğruysa true, aksi halde false döner.
    static bool loginUser(const std::string& username, const std::string& password);

private:
    static const std::string DB_FILE;
};

#endif // AUTH_H
