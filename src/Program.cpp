#include <iostream>
#include <string>
#include <fstream>

#include "Program.hpp"
#include "AccountManager.hpp"
#include "STLite/vector.hpp"
#include "STLite/pair.hpp"
#include "STLite/map.hpp"

struct Account;

Program::Program() : total_filename("total") {
    total_file.open(total_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!total_file) {
        total_file.open(total_filename, std::ios::out | std::ios::binary);
        total_file.close();
        total_file.open(total_filename, std::ios::in | std::ios::out | std::ios::binary);
        accountmanager.changeCount(0);
    } else {
        total_file.seekg(0);
        int account_count;
        total_file.read(reinterpret_cast<char*>(&account_count), 4);
        accountmanager.changeCount(account_count);
    }
    loginrecorder.clear();
}

Program::~Program() {
    total_file.seekp(0);
    int account_count = accountmanager.getCount();
    total_file.write(reinterpret_cast<char*>(&account_count), 4);
    total_file.close();
}

void Program::execute(const std::string &line) {
    parser.parseLine(line, this);
}

void Program::programEnd() {
    programEnd_ = true;
}

bool Program::programRun() {
    return !programEnd_;
}

void Program::AddUser(const std::string &cur_username, const std::string &username, const std::string &password, const std::string &name, const std::string &mailAddr, int privilege) {
    // std::cout << "cur_username: " << cur_username << '\n';
    // std::cout << "username: " << username << '\n';
    // std::cout << "password: " << password << '\n';
    // std::cout << "name: " << name << '\n';
    // std::cout << "mailAddr: " << mailAddr << '\n';
    // std::cout << "privilege: " << privilege << std::endl;

    if (accountmanager.getCount() == 0) {
        accountmanager.addAccount(username, password, name, mailAddr, 10);
        std::cout << "0" << std::endl;
        return;
    }
    if (loginrecorder.find(cur_username) == loginrecorder.end()) {
        std::cout << "-1" << std::endl;
        return;
    }
    int cur_privilege = loginrecorder[cur_username];
    if (privilege >= cur_privilege) {
        std::cout << "-1" << std::endl;
        return;
    }
    if (accountmanager.getIndex(username) != -1) {
        std::cout << "-1" << std::endl;
        return;
    }
    accountmanager.addAccount(username, password, name, mailAddr, privilege);
    std::cout << "0" << std::endl;
}

void Program::Login(const std::string &username, const std::string &password) {
    // std::cout << "username: " << username << '\n';
    // std::cout << "password: " << password << std::endl;
    int index = accountmanager.getIndex(username);
    if (index == -1) {
        std::cout << "-1" << std::endl;
        return;
    }
    if (loginrecorder.find(username) != loginrecorder.end()) {
        std::cout << "-1" << std::endl;
        return;
    }
    Account account = accountmanager.getAccount(index);
    if (account.password == password) {
        loginrecorder.insert(sjtu::pair(username, account.privilege));
        std::cout << "0" << std::endl;
        return;
    }
    std::cout << "-1" << std::endl;
}

void Program::Logout(const std::string &username) {
    // std::cout << "username: " << username << std::endl;
    auto it = loginrecorder.find(username);
    if (it == loginrecorder.end()) {
        std::cout << "-1" << std::endl;
        return;
    }
    loginrecorder.erase(it);
    std::cout << "0" << std::endl;
}

void Program::QueryProfile(const std::string &cur_username, const std::string &username) {
    // std::cout << "cur_username: " << cur_username << '\n';
    // std::cout << "username: " << username << std::endl;
    auto it = loginrecorder.find(cur_username);
    if (it == loginrecorder.end()) {
        std::cout << "-1" << std::endl;
        return;
    }
    int index = accountmanager.getIndex(username);
    if (index == -1) {
        std::cout << "-1" << std::endl;
        return;
    }
    Account account = accountmanager.getAccount(index);
    if (it->second < account.privilege || it->second == account.privilege && cur_username != username) {
        std::cout << "-1" << std::endl;
        return;
    }
    std::cout << username << ' ' << account.name << ' ' << account.mailAddr << ' ' << account.privilege << std::endl;
}

void Program::ModifyProfile(const std::string &cur_username, const std::string &username, const std::string &password, const std::string &name, const std::string &mailAddr, int privilege) {
    // std::cout << "cur_username: " << cur_username << '\n';
    // std::cout << "username: " << username << std::endl;
    // if (!password.empty()) {
    //     std::cout << "password: " << password << std::endl;
    // }
    // if (!name.empty()) {
    //     std::cout << "name: " << name << std::endl;
    // }
    // if (!mailAddr.empty()) {
    //     std::cout << "mailAddr: " << mailAddr << std::endl;
    // }
    // if (privilege != -1) {
    //     std::cout << "privilege: " << privilege << std::endl;
    // }
    auto it = loginrecorder.find(cur_username);
    if (it == loginrecorder.end()) {
        std::cout << "-1" << std::endl;
        return;
    }
    if (privilege != -1 && privilege >= it->second) {
        std::cout << "-1" << std::endl;
        return;
    }
    int index = accountmanager.getIndex(username);
    if (index == -1) {
        std::cout << "-1" << std::endl;
        return;
    }
    Account account = accountmanager.getAccount(index);
    if (it->second < account.privilege || it->second == account.privilege && cur_username != username) {
        std::cout << "-1" << std::endl;
        return;
    }
    if (!password.empty()) {
        account.password = password;
    }
    if (!name.empty()) {
        account.name = name;
    }
    if (!mailAddr.empty()) {
        account.mailAddr = mailAddr;
    }
    if (privilege != -1) {
        account.privilege = privilege;
        auto it2 = loginrecorder.find(username);
        if (it2 != loginrecorder.end()) {
            it2->second = privilege;
        }
    }
    accountmanager.writeAccount(index, account);
    std::cout << username << ' ' << account.name << ' ' << account.mailAddr << ' ' << account.privilege << std::endl;
}

void Program::AddTrain(const std::string &trainID, int stationNum, int seatNum, sjtu::vector<std::string> &stations, sjtu::vector<int> &prices, int startTime, sjtu::vector<int> &travelTimes, sjtu::vector<int> &stopoverTimes, sjtu::pair<int, int> saleDate, char type) {
    std::cout << "trainID: " << trainID << '\n';
    std::cout << "stationNum: " << stationNum << '\n';
    std::cout << "seatNum: " << seatNum << '\n';
    std::cout << "stations: ";
    for (auto station : stations) {
        std::cout << station << ' ';
    }
    std::cout << '\n';
    std::cout << "prices: ";
    for (auto price : prices) {
        std::cout << price << ' ';
    }
    std::cout << '\n';
    std::cout << "startTime: " << startTime / 60 << ':' << startTime % 60 << '\n';
    for (auto travelTime : travelTimes) {
        std::cout << travelTime << ' ';
    }
    std::cout << '\n';
    std::cout << "stopoverTimes: ";
    for (auto stopoverTime : stopoverTimes) {
        std::cout << stopoverTime << ' ';
    }
    std::cout << '\n';
    std::cout << "saleDate: " << saleDate.first << ' ' << saleDate.second << '\n';
    std::cout << "type: " << type << std::endl;
}

void Program::DeleteTrain(const std::string &trainID) {
    std::cout << "trainID: " << trainID << std::endl;
}

void Program::ReleaseTrain(const std::string &trainID) {
    std::cout << "trainID: " << trainID << std::endl;
}

void Program::QueryTrain(const std::string &trainID, int date) {
    std::cout << "trainID: " << trainID << '\n';
    std::cout << "Date: " << date << std::endl;
}

void Program::QueryTicket(const std::string &station1, const std::string station2, int date, bool query_type) {
    std::cout << "station1: " << station1 << '\n';
    std::cout << "station2: " << station2 << '\n';
    std::cout << "date: " << date << '\n';
    if (!query_type) {
        std::cout << "time" << std::endl;
    } else {
        std::cout << "cose" << std::endl;
    }
}

void Program::QueryTransfer(const std::string &station1, const std::string station2, int date, bool query_type) {
    std::cout << "station1: " << station1 << '\n';
    std::cout << "station2: " << station2 << '\n';
    std::cout << "date: " << date << '\n';
    if (!query_type) {
        std::cout << "time" << std::endl;
    } else {
        std::cout << "cose" << std::endl;
    }
}

void Program::BuyTicket(const std::string &username, const std::string &trainID, int date, int ticketnum, std::string &station1, std::string &station2, bool buy_type) {
    std::cout << "username: " << username << '\n';
    std::cout << "trainID: " << trainID << '\n';
    std::cout << "date: " << date << '\n';
    std::cout << "ticketnum: " << ticketnum << '\n';
    std::cout << "station1: " << station1 << '\n';
    std::cout << "station2: " << station2 << '\n';
    if (!buy_type) {
        std::cout << "false" << std::endl;
    } else {
        std::cout << "true" << std::endl;
    }
}

void Program::QueryOrder(const std::string &username) {
    std::cout << "username: " << username << std::endl;
}

void Program::RefundTicket(const std::string &username, int ticketnum) {
    std::cout << "username: " << username << '\n';
    std::cout << "ticketnum: " << ticketnum << std::endl;
}

void Program::Clean() {
    
}

void Program::Exit() {
    programEnd_ = true;
    std::cout << "bye" << std::endl;
}