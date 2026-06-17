#pragma once

#include <fstream>

#include "STLite/BPT.hpp"
#include "STLite/string.hpp"
#include "utils/constants.hpp"

struct Account {
    sjtu::string<PassWordLength> password;
    sjtu::string<NameLength> name;
    sjtu::string<MailAddrLength> mailAddr;
    int privilege;
};

class AccountManager {
  private:
    int account_count;
    std::string account_filename;
    std::fstream account_file;
    BplusTree<sjtu::string<UserNameLength>, int> account_bpt;
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