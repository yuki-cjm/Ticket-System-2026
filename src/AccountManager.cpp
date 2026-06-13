#include <string>
#include <fstream>
#include <iostream>

#include "AccountManager.hpp"
#include "STLite/vector.hpp"
#include "STLite/BPT.hpp"
#include "STLite/string.hpp"

constexpr int sizeofpassword = sizeof(sjtu::string<30>);
constexpr int sizeofname = sizeof(sjtu::string<15>);
constexpr int sizeofmailAddr = sizeof(sjtu::string<30>);
constexpr int sizeofint = sizeof(int);

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

int AccountManager::getIndex(const sjtu::string<20> &username) {
    sjtu::vector<int> indexs = account_bpt.find(username);
    if (!indexs.size()) {
        return -1;
    }
    return indexs[0];
}

void AccountManager::addAccount(sjtu::string<20> &username, sjtu::string<30> &password, sjtu::string<15> &name, sjtu::string<30> &mailAddr, int privilege) {
    int index = account_count;
    account_count++;
    account_bpt.insert(username, index);
    account_file.seekp(account_size * index);
    account_file.write(reinterpret_cast<char*>(&password), sizeofpassword);
    account_file.write(reinterpret_cast<char*>(&name), sizeofname);
    account_file.write(reinterpret_cast<char*>(&mailAddr), sizeofmailAddr);
    account_file.write(reinterpret_cast<char*>(&privilege), sizeofint);
}

Account AccountManager::getAccount(int index) {
    account_file.seekg(account_size * index);
    Account account;
    account_file.read(reinterpret_cast<char*>(&account.password), sizeofpassword);
    account_file.read(reinterpret_cast<char*>(&account.name), sizeofname);
    account_file.read(reinterpret_cast<char*>(&account.mailAddr), sizeofmailAddr);
    account_file.read(reinterpret_cast<char*>(&account.privilege), sizeofint);
    return account;
}

void AccountManager::writeAccount(int index, Account &account) {
    account_file.seekp(account_size * index);
    account_file.write(reinterpret_cast<char*>(&account.password), sizeofpassword);
    account_file.write(reinterpret_cast<char*>(&account.name), sizeofname);
    account_file.write(reinterpret_cast<char*>(&account.mailAddr), sizeofmailAddr);
    account_file.write(reinterpret_cast<char*>(&account.privilege), sizeofint);
}