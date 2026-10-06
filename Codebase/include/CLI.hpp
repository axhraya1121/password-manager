#ifndef CLI_HPP
#define CLI_HPP

#include <string>
#include "Vault.hpp"
#include "RecentStack.hpp"

class CLI {
private:
    Vault vault;
    RecentStack recentStack;
    std::string masterPassword;
    std::string masterHash;
    std::string vaultFile;

    void showBanner();
    bool setupNewVault();
    bool login();

    void addEntry();
    void retrieveEntry();
    void updateEntry();
    void deleteEntry();
    void listSites();
    void showRecent();
    void generatePassword();
    void checkStrength();
    void saveAndExit();

    static std::string getHiddenInput();

public:
    explicit CLI(const std::string& filename = "vault.dat");
    void run();
};

#endif
