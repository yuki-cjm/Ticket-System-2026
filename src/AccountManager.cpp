#include <string>
#include <fstream>
#include <iostream>

#include "AccountManager.hpp"
#include "STLite/vector.hpp"
#include "STLite/BPT.hpp"

struct Account;

AccountManager::AccountManager() : account_filename("accountdata"), account_bpt("accountbasic", "accountindex") {
    account_file.open(account_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!account_file) {
        account_file.open(account_filename, std::ios::out | std::ios::binary);
        account_file.close();
        account_file.open(account_filename, std::ios::in | std::ios::out | std::ios::binary);
    }
}

AccountManager::~AccountManager() {
    account_file.close();
}

void AccountManager::changeCount(int count) {
    account_count = count;
}

int AccountManager::getCount() {
    return account_count;
}

int AccountManager::getIndex(const std::string &username) {
    sjtu::vector<int> indexs = account_bpt.find(username);
    if (!indexs.size()) {
        return -1;
    }
    return indexs[0];
}

void AccountManager::addAccount(const std::string &username, const std::string &password, const std::string &name, const std::string &mailAddr, int privilege) {
    int index = account_count;
    account_count++;
    account_bpt.insert(username, index);
    account_file.seekp(account_size * index);
    char dest[31];
    std::strncpy(dest, password.c_str(), 30);
    dest[30] = '\0';
    account_file.write(dest, 31);
    std::strncpy(dest, name.c_str(), 15);
    dest[15] = '\0';
    account_file.write(dest, 16);
    std::strncpy(dest, mailAddr.c_str(), 30);
    dest[30] = '\0';
    account_file.write(dest, 31);
    account_file.write(reinterpret_cast<char*>(&privilege), 4);
}

Account AccountManager::getAccount(int index) {
    account_file.seekg(account_size * index);
    Account account;
    char dest[31];
    account_file.read(dest, 31);
    account.password = std::string(dest);
    account_file.read(dest, 16);
    account.name = std::string(dest);
    account_file.read(dest, 31);
    account.mailAddr = std::string(dest);
    account_file.read(reinterpret_cast<char*>(&account.privilege), 4);
    return account;
}

void AccountManager::writeAccount(int index, Account &account) {
    account_file.seekp(account_size * index);
    char dest[31];
    std::strncpy(dest, account.password.c_str(), 30);
    dest[30] = '\0';
    account_file.write(dest, 31);
    std::strncpy(dest, account.name.c_str(), 15);
    dest[15] = '\0';
    account_file.write(dest, 16);
    std::strncpy(dest, account.mailAddr.c_str(), 30);
    dest[30] = '\0';
    account_file.write(dest, 31);
    account_file.write(reinterpret_cast<char*>(&account.privilege), 4);
}