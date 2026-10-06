#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>

namespace Crypto {
    std::string xorCipher(const std::string& text, const std::string& key);
    std::string encrypt(const std::string& plainText, const std::string& key);
    std::string decrypt(const std::string& cipherHex, const std::string& key);
    std::string hashPassword(const std::string& password);
}

#endif
