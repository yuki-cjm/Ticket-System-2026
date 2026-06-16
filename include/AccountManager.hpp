#pragma once

#include <fstream>

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
    static const int account_size = sizeof(Account);

  public:
    AccountManager();
    ~AccountManager();
    void changeAccountCount(int count);
    int getAccountCount();

    int getIndex(const sjtu::string<20> &username);
    void addAccount(sjtu::string<20> &username, sjtu::string<30> &password, sjtu::string<15> &name, sjtu::string<30> &mailAddr, int privilege);
    Account getAccount(int index);
    void writeAccount(int index, Account &account);

    void clean();
};