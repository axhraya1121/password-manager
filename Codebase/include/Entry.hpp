#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <string>

class Entry {
private:
    std::string siteName;
    std::string username;
    std::string encryptedPassword;

public:
    Entry(const std::string& site, const std::string& user, const std::string& encPass)
        : siteName(site), username(user), encryptedPassword(encPass) {}

    Entry(const Entry& other)
        : siteName(other.siteName), username(other.username),
          encryptedPassword(other.encryptedPassword) {}

    Entry& operator=(const Entry& other) {
        if (this != &other) {
            siteName = other.siteName;
            username = other.username;
            encryptedPassword = other.encryptedPassword;
        }
        return *this;
    }

    const std::string& getSiteName() const { return siteName; }
    const std::string& getUsername() const { return username; }
    const std::string& getEncryptedPassword() const { return encryptedPassword; }

    void setUsername(const std::string& user) { username = user; }
    void setEncryptedPassword(const std::string& encPass) { encryptedPassword = encPass; }
};

#endif
