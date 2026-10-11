#ifndef FILEHANDLER_HPP
#define FILEHANDLER_HPP

#include <string>
#include <vector>
#include <utility>
#include "Entry.hpp"

namespace FileHandler {
    void save(const std::vector<Entry>& entries, const std::string& masterHash,
              const std::string& filename);
    std::pair<std::string, std::vector<Entry>> load(const std::string& filename);
}

#endif
