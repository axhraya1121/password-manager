#include "FileHandler.hpp"
#include <fstream>
#include <sstream>

namespace FileHandler {

void save(const std::vector<Entry>& entries, const std::string& masterHash,
          const std::string& filename) {
    std::ofstream out(filename);
    out << masterHash << "\n";
    for (const auto& entry : entries) {
        out << entry.getSiteName() << "|"
            << entry.getUsername() << "|"
            << entry.getEncryptedPassword() << "\n";
    }
}

std::pair<std::string, std::vector<Entry>> load(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        return {"", {}};
    }

    std::string masterHash;
    std::getline(in, masterHash);

    std::vector<Entry> entries;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string site, user, encPass;
        if (std::getline(iss, site, '|') &&
            std::getline(iss, user, '|') &&
            std::getline(iss, encPass)) {
            entries.emplace_back(site, user, encPass);
        }
    }

    return {masterHash, entries};
}

}
