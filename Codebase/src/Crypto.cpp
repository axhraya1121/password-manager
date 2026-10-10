#include "Crypto.hpp"
#include <sstream>
#include <iomanip>
#include <cstdint>

namespace Crypto {

static std::string toHex(const std::string& data) {
    std::ostringstream oss;
    for (unsigned char c : data) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return oss.str();
}

static std::string fromHex(const std::string& hex) {
    std::string result;
    result.reserve(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        auto byte = static_cast<char>(std::stoi(hex.substr(i, 2), nullptr, 16));
        result += byte;
    }
    return result;
}

std::string xorCipher(const std::string& text, const std::string& key) {
    std::string result;
    result.reserve(text.size());
    for (size_t i = 0; i < text.size(); ++i) {
        result += static_cast<char>(text[i] ^ key[i % key.size()]);
    }
    return result;
}

std::string encrypt(const std::string& plainText, const std::string& key) {
    return toHex(xorCipher(plainText, key));
}

std::string decrypt(const std::string& cipherHex, const std::string& key) {
    return xorCipher(fromHex(cipherHex), key);
}

std::string hashPassword(const std::string& password) {
    uint64_t hash = 5381;
    for (char c : password) {
        hash = hash * 33 + static_cast<unsigned char>(c);
    }
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return oss.str();
}

}
