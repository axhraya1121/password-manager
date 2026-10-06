#include "Vault.hpp"
#include "Crypto.hpp"

Vault::Vault(size_t initialSize)
    : buckets(initialSize), tableSize(initialSize), entryCount(0) {}

size_t Vault::hash(const std::string& key) const {
    size_t h = 0;
    for (char c : key) {
        h = h * 31 + static_cast<unsigned char>(c);
    }
    return h % tableSize;
}

void Vault::resizeIfNeeded() {
    double loadFactor = static_cast<double>(entryCount) / static_cast<double>(tableSize);
    if (loadFactor <= 0.7) return;

    size_t newSize = tableSize * 2;
    std::vector<std::vector<Entry>> newBuckets(newSize);

    for (auto& bucket : buckets) {
        for (auto& entry : bucket) {
            size_t h = 0;
            for (char c : entry.getSiteName()) {
                h = h * 31 + static_cast<unsigned char>(c);
            }
            newBuckets[h % newSize].push_back(entry);
        }
    }

    buckets = std::move(newBuckets);
    tableSize = newSize;
}

void Vault::insert(const std::string& site, const std::string& username,
                   const std::string& plainPassword, const std::string& masterPassword) {
    std::string encrypted = Crypto::encrypt(plainPassword, masterPassword);
    size_t idx = hash(site);
    buckets[idx].emplace_back(site, username, encrypted);
    ++entryCount;
    resizeIfNeeded();
}

std::pair<std::string, std::string> Vault::retrieve(const std::string& site,
                                                     const std::string& masterPassword) const {
    size_t idx = hash(site);
    for (const auto& entry : buckets[idx]) {
        if (entry.getSiteName() == site) {
            std::string decrypted = Crypto::decrypt(entry.getEncryptedPassword(), masterPassword);
            return {entry.getUsername(), decrypted};
        }
    }
    return {"", ""};
}

bool Vault::update(const std::string& site, const std::string& newUsername,
                   const std::string& newPlainPassword, const std::string& masterPassword) {
    size_t idx = hash(site);
    for (auto& entry : buckets[idx]) {
        if (entry.getSiteName() == site) {
            entry.setUsername(newUsername);
            entry.setEncryptedPassword(Crypto::encrypt(newPlainPassword, masterPassword));
            return true;
        }
    }
    return false;
}

bool Vault::remove(const std::string& site) {
    size_t idx = hash(site);
    auto& bucket = buckets[idx];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->getSiteName() == site) {
            bucket.erase(it);
            --entryCount;
            return true;
        }
    }
    return false;
}

std::vector<std::string> Vault::listAllSites() const {
    std::vector<std::string> sites;
    sites.reserve(entryCount);
    for (const auto& bucket : buckets) {
        for (const auto& entry : bucket) {
            sites.push_back(entry.getSiteName());
        }
    }
    return sites;
}

std::vector<Entry> Vault::getAllEntries() const {
    std::vector<Entry> all;
    all.reserve(entryCount);
    for (const auto& bucket : buckets) {
        for (const auto& entry : bucket) {
            all.push_back(entry);
        }
    }
    return all;
}

void Vault::loadEntry(const Entry& entry) {
    size_t idx = hash(entry.getSiteName());
    buckets[idx].push_back(entry);
    ++entryCount;
    resizeIfNeeded();
}

bool Vault::contains(const std::string& site) const {
    size_t idx = hash(site);
    for (const auto& entry : buckets[idx]) {
        if (entry.getSiteName() == site) return true;
    }
    return false;
}
