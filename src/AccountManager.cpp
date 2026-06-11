#include <string>
#include <fstream>

#include "AccountManager.hpp"

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