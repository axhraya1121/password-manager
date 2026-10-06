# Vaultify — Secure Local Password Manager

A single-user, offline, command-line password manager built in C++17 for a
Data Structures + OOP course project. Demonstrates manual hash table
implementation (separate chaining, dynamic resizing), XOR symmetric cipher,
and clean module separation.

## Build & Run

**Using CMake (recommended):**
```bash
mkdir build && cd build
cmake ..
make          # or: cmake --build .
./vaultify
```

**Using g++ directly:**
```bash
g++ -std=c++17 -Wall -Iinclude src/*.cpp -o vaultify
./vaultify
```

Works on Linux, macOS, and Windows (MinGW). No external dependencies —
standard library only.

## Project Structure

| File | Purpose |
|------|---------|
| `include/Entry.hpp` | Credential data class (site, username, encrypted password) |
| `include/Vault.hpp` | Hash table storage engine (separate chaining) |
| `include/Crypto.hpp` | XOR encryption + djb2 password hashing |
| `include/FileHandler.hpp` | Disk persistence (pipe-delimited flat file) |
| `include/RecentStack.hpp` | LIFO stack of recently retrieved sites |
| `include/Colors.hpp` | ANSI color constants for CLI output |
| `include/CLI.hpp` | Menu-driven interface |
| `src/*.cpp` | Corresponding implementations |

## Security Model & Limitations

This is an **educational implementation** — not production-grade.

- **Encryption:** XOR cipher with the master password as key. XOR is a valid
  symmetric cipher but is trivially broken with known-plaintext or frequency
  analysis. A real system would use AES-256-GCM with a key derived via
  PBKDF2/Argon2.
- **Master password verification:** A djb2-variant hash is stored on disk to
  verify login. The hash is **never** used as the encryption key — the actual
  plain-text master password (held only in memory for the session) is the
  decryption key. This means stealing the hash file alone cannot decrypt
  entries.
- **No salt:** The master hash is unsalted, making it vulnerable to
  precomputed rainbow-table attacks. Adding a random per-install salt is a
  documented extension point in the code.
- **No password recovery:** If the master password is lost, the vault is
  permanently inaccessible. This matches how real password managers work.
- **Local only:** All data stays on the machine; nothing is transmitted.
