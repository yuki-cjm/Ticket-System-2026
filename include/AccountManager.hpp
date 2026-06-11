#pragma once

#include <fstream>
#include <string>

#include "STLite/BPT.hpp"

struct Account {
    // std::string username; // 20
    std::string password; // 30
    std::string name; // 15
    std::string mailAddr; // 30
    int privilege;
};

template <int strlength>
class BplusTree;

class AccountManager {
  private:
    int account_count;
    std::string account_filename;
    std::fstream account_file;
    BplusTree<20> account_bpt;
    static const int account_size = 82;
  public:
    AccountManager();
    ~AccountManager();
    void changeCount(int count);
    int getCount();

    int getIndex(const std::string &username);
    void addAccount(const std::string &username, const std::string &password, const std::string &name, const std::string &mailAddr, int privilege);
    Account getAccount(int index);
    void writeAccount(int index, Account &account);
};