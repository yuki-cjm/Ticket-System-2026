#pragma once

#include <fstream>
#include <string>

#include "STLite/BPT.hpp"
#include "STLite/string.hpp"

struct Account {
    sjtu::string<30> password;
    sjtu::string<15> name;
    sjtu::string<30> mailAddr;
    int privilege;
};

class AccountManager {
  private:
    int account_count;
    std::string account_filename;
    std::fstream account_file;
    BplusTree<sjtu::string<20>, int> account_bpt;
    static const int account_size = sizeof(sjtu::string<30>) + sizeof(sjtu::string<15>) + sizeof(sjtu::string<30>) + sizeof(int);
  public:
    AccountManager();
    ~AccountManager();
    void changeCount(int count);
    int getCount();

    int getIndex(const sjtu::string<20> &username);
    void addAccount(sjtu::string<20> &username, sjtu::string<30> &password, sjtu::string<15> &name, sjtu::string<30> &mailAddr, int privilege);
    Account getAccount(int index);
    void writeAccount(int index, Account &account);
};