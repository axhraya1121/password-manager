#include "CLI.hpp"
#include "Crypto.hpp"
#include "FileHandler.hpp"
#include "Colors.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <limits>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

static void clearInputLine() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string CLI::getHiddenInput() {
#ifdef _WIN32
    std::string input;
    char ch;
    while ((ch = static_cast<char>(_getch())) != '\r') {
        if (ch == '\b' || ch == 127) {
            if (!input.empty()) { input.pop_back(); std::cout << "\b \b"; }
        } else {
            input += ch;
            std::cout << '*';
        }
    }
    std::cout << "\n";
    return input;
#else
    termios oldt{}, newt{};
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~static_cast<unsigned>(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    std::string input;
    std::getline(std::cin, input);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << "\n";
    return input;
#endif
}

CLI::CLI(const std::string& filename) : vaultFile(filename) {}

void CLI::showBanner() {
    std::cout << Colors::CYAN << Colors::BOLD
              << R"(
 __     __         _ _   _  __
 \ \   / /_ _ _   | | | (_)/ _|_   _
  \ \ / / _` | | | | | __| | |_| | | |
   \ V / (_| | |_| | | |_| |  _| |_| |
    \_/ \__,_|\__,_|_|\__|_|_|  \__, |
                                 |___/
)"
              << Colors::RESET
              << "  Secure Local Password Manager (educational)\n\n";
}

bool CLI::setupNewVault() {
    std::cout << Colors::YELLOW << "No existing vault found. Let's create one.\n" << Colors::RESET;
    std::cout << "Choose a master password: ";
    masterPassword = getHiddenInput();
    if (masterPassword.empty()) {
        std::cout << Colors::RED << "Master password cannot be empty.\n" << Colors::RESET;
        return false;
    }
    std::cout << "Confirm master password: ";
    std::string confirm = getHiddenInput();
    if (confirm != masterPassword) {
        std::cout << Colors::RED << "Passwords do not match.\n" << Colors::RESET;
        return false;
    }
    masterHash = Crypto::hashPassword(masterPassword);
    FileHandler::save({}, masterHash, vaultFile);
    std::cout << Colors::GREEN << "Vault created successfully!\n" << Colors::RESET;
    return true;
}

bool CLI::login() {
    auto [storedHash, entries] = FileHandler::load(vaultFile);
    masterHash = storedHash;

    for (int attempt = 1; attempt <= 3; ++attempt) {
        std::cout << "Enter master password (" << attempt << "/3): ";
        masterPassword = getHiddenInput();
        if (Crypto::hashPassword(masterPassword) == masterHash) {
            for (const auto& entry : entries) {
                vault.loadEntry(entry);
            }
            std::cout << Colors::GREEN << "Vault unlocked.\n" << Colors::RESET;
            return true;
        }
        std::cout << Colors::RED << "Incorrect master password.\n" << Colors::RESET;
    }
    std::cout << Colors::RED << "Too many failed attempts. Exiting.\n" << Colors::RESET;
    return false;
}

void CLI::addEntry() {
    std::string site, user, pass;
    std::cout << "Site name: ";
    std::getline(std::cin, site);
    if (vault.contains(site)) {
        std::cout << Colors::RED << "Site already exists. Use update instead.\n" << Colors::RESET;
        return;
    }
    std::cout << "Username:  ";
    std::getline(std::cin, user);
    std::cout << "Password:  ";
    pass = getHiddenInput();
    vault.insert(site, user, pass, masterPassword);
    std::cout << Colors::GREEN << "Entry added successfully.\n" << Colors::RESET;
}

void CLI::retrieveEntry() {
    std::string site;
    std::cout << "Site name: ";
    std::getline(std::cin, site);
    auto [user, pass] = vault.retrieve(site, masterPassword);
    if (user.empty() && pass.empty()) {
        std::cout << Colors::RED << "Site not found.\n" << Colors::RESET;
        return;
    }
    std::cout << Colors::GREEN << "Username: " << Colors::RESET << user << "\n"
              << Colors::GREEN << "Password: " << Colors::RESET << pass << "\n";
    recentStack.push(site);
}

void CLI::updateEntry() {
    std::string site;
    std::cout << "Site name to update: ";
    std::getline(std::cin, site);
    if (!vault.contains(site)) {
        std::cout << Colors::RED << "Site not found.\n" << Colors::RESET;
        return;
    }
    std::string user, pass;
    std::cout << "New username:  ";
    std::getline(std::cin, user);
    std::cout << "New password:  ";
    pass = getHiddenInput();
    vault.update(site, user, pass, masterPassword);
    std::cout << Colors::GREEN << "Entry updated successfully.\n" << Colors::RESET;
}

void CLI::deleteEntry() {
    std::string site;
    std::cout << "Site name to delete: ";
    std::getline(std::cin, site);
    if (vault.remove(site)) {
        std::cout << Colors::GREEN << "Entry deleted.\n" << Colors::RESET;
    } else {
        std::cout << Colors::RED << "Site not found.\n" << Colors::RESET;
    }
}

void CLI::listSites() {
    auto sites = vault.listAllSites();
    if (sites.empty()) {
        std::cout << Colors::YELLOW << "Vault is empty.\n" << Colors::RESET;
        return;
    }
    std::sort(sites.begin(), sites.end());
    std::cout << Colors::CYAN << Colors::BOLD
              << std::left << std::setw(6) << "#" << "Site Name\n"
              << std::string(30, '-') << "\n" << Colors::RESET;
    int idx = 1;
    for (const auto& s : sites) {
        std::cout << std::left << std::setw(6) << idx++ << s << "\n";
    }
}

void CLI::showRecent() {
    if (recentStack.empty()) {
        std::cout << Colors::YELLOW << "No recent retrievals yet.\n" << Colors::RESET;
        return;
    }
    auto recent = recentStack.peekTopN(10);
    std::cout << Colors::CYAN << Colors::BOLD << "Recently accessed:\n" << Colors::RESET;
    for (size_t i = 0; i < recent.size(); ++i) {
        std::cout << "  " << i + 1 << ". " << recent[i] << "\n";
    }
}

void CLI::generatePassword() {
    int length = 0;
    std::cout << "Password length (8-128): ";
    std::cin >> length;
    clearInputLine();
    if (length < 8 || length > 128) {
        std::cout << Colors::RED << "Length must be between 8 and 128.\n" << Colors::RESET;
        return;
    }

    const std::string charset =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789"
        "!@#$%^&*()-_=+[]{}|;:,.<>?";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);

    std::string password;
    password.reserve(static_cast<size_t>(length));
    for (int i = 0; i < length; ++i) {
        password += charset[dist(gen)];
    }
    std::cout << Colors::GREEN << "Generated: " << Colors::RESET << password << "\n";
}

void CLI::checkStrength() {
    std::string password;
    std::cout << "Enter password to check: ";
    password = getHiddenInput();

    int score = 0;
    std::string reasons;

    if (password.length() >= 8)  { ++score; } else { reasons += "  - Too short (min 8 chars)\n"; }
    if (password.length() >= 12) { ++score; }

    bool hasUpper = false, hasLower = false, hasDigit = false, hasSymbol = false;
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
        else hasSymbol = true;
    }
    if (hasUpper)  { ++score; } else { reasons += "  - Add uppercase letters\n"; }
    if (hasLower)  { ++score; } else { reasons += "  - Add lowercase letters\n"; }
    if (hasDigit)  { ++score; } else { reasons += "  - Add digits\n"; }
    if (hasSymbol) { ++score; } else { reasons += "  - Add symbols\n"; }

    int filled = std::min(10, score * 10 / 6);
    std::string bar = "[" + std::string(static_cast<size_t>(filled), '#')
                          + std::string(static_cast<size_t>(10 - filled), '-') + "]";

    const char* color;
    const char* label;
    if (score <= 2)      { color = Colors::RED;    label = "Weak";   }
    else if (score <= 4) { color = Colors::YELLOW;  label = "Medium"; }
    else                 { color = Colors::GREEN;  label = "Strong"; }

    std::cout << color << "Strength: " << label << " " << bar << "\n" << Colors::RESET;
    if (!reasons.empty()) {
        std::cout << Colors::YELLOW << "Suggestions:\n" << reasons << Colors::RESET;
    }
}

void CLI::saveAndExit() {
    FileHandler::save(vault.getAllEntries(), masterHash, vaultFile);
    std::cout << Colors::GREEN << "Vault saved. Goodbye!\n" << Colors::RESET;
}

void CLI::run() {
    showBanner();

    auto [storedHash, _] = FileHandler::load(vaultFile);
    if (storedHash.empty()) {
        if (!setupNewVault()) return;
    } else {
        if (!login()) return;
    }

    bool running = true;
    while (running) {
        std::cout << "\n" << Colors::BOLD
                  << "[1] Add new entry\n"
                  << "[2] Retrieve a password\n"
                  << "[3] Update an entry\n"
                  << "[4] Delete an entry\n"
                  << "[5] List all saved sites\n"
                  << "[6] Show recently accessed\n"
                  << "[7] Generate random password\n"
                  << "[8] Check password strength\n"
                  << "[9] Save & Exit\n"
                  << Colors::RESET
                  << "Choice: ";

        int choice = 0;
        if (!(std::cin >> choice)) {
            clearInputLine();
            std::cout << Colors::RED << "Please enter a number 1-9.\n" << Colors::RESET;
            continue;
        }
        clearInputLine();

        switch (choice) {
            case 1: addEntry();         break;
            case 2: retrieveEntry();    break;
            case 3: updateEntry();      break;
            case 4: deleteEntry();      break;
            case 5: listSites();        break;
            case 6: showRecent();       break;
            case 7: generatePassword(); break;
            case 8: checkStrength();    break;
            case 9: saveAndExit(); running = false; break;
            default:
                std::cout << Colors::RED << "Invalid choice. Try 1-9.\n" << Colors::RESET;
        }
    }
}
