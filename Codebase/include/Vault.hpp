#ifndef VAULT_HPP
#define VAULT_HPP

#include <string>
#include <vector>
#include <utility>
#include "Entry.hpp"

class Vault {
private:
    std::vector<std::vector<Entry>> buckets;
    size_t tableSize;
    size_t entryCount;

    size_t hash(const std::string& key) const;
    void resizeIfNeeded();

public:
    explicit Vault(size_t initialSize = 16);

    void insert(const std::string& site, const std::string& username,
                const std::string& plainPassword, const std::string& masterPassword);

    std::pair<std::string, std::string> retrieve(const std::string& site,
                                                  const std::string& masterPassword) const;

    bool update(const std::string& site, const std::string& newUsername,
                const std::string& newPlainPassword, const std::string& masterPassword);

    bool remove(const std::string& site);
    std::vector<std::string> listAllSites() const;
    std::vector<Entry> getAllEntries() const;
    void loadEntry(const Entry& entry);
    bool contains(const std::string& site) const;
    size_t size() const { return entryCount; }
};

#endif
