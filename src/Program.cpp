#include <iostream>
#include <string>
#include <fstream>

#include "Program.hpp"
#include "AccountManager.hpp"
#include "TrainManager.hpp"
#include "STLite/vector.hpp"
#include "STLite/pair.hpp"
#include "STLite/map.hpp"
#include "STLite/string.hpp"

constexpr int sizeofint = sizeof(int);
constexpr int daytime = 1440;
constexpr int hourtime = 60;

void printTime(int time) {
    int day = time / daytime, month = 6;
    time %= daytime;
    if (day > 30) {
        day -= 30;
        month++;
    }
    if (day > 31) {
        day -= 31;
        month++;
    }
    printf("%02d-%02d %02d:%02d", month, day, time / hourtime, time % hourtime);
}

Program::Program() : total_filename("total") {
    total_file.open(total_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!total_file) {
        total_file.open(total_filename, std::ios::out | std::ios::binary);
        total_file.close();
        total_file.open(total_filename, std::ios::in | std::ios::out | std::ios::binary);
        accountmanager.changeCount(0);
        trainmanager.changeTrainCount(0);
        trainmanager.changeStationCount(0);
    } else {
        total_file.seekg(0);
        int count;
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        accountmanager.changeCount(count);
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        trainmanager.changeTrainCount(count);
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        trainmanager.changeStationCount(count);
    }
    loginrecorder.clear();
}

Program::~Program() {
    total_file.seekp(0);
    int count;
    count = accountmanager.getCount();
    total_file.write(reinterpret_cast<char*>(&count), sizeofint);
    count = trainmanager.getTrainCount();
    total_file.write(reinterpret_cast<char*>(&count), sizeofint);
    count = trainmanager.getStationCount();
    total_file.write(reinterpret_cast<char*>(&count), sizeofint);
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

void Program::AddUser(sjtu::string<20> &cur_username, sjtu::string <20>&username, sjtu::string<30> &password, sjtu::string<15> &name, sjtu::string<30> &mailAddr, int privilege) {
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

void Program::Login(const sjtu::string<20> &username, const sjtu::string<30> &password) {
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

void Program::Logout(const sjtu::string<20> &username) {
    // std::cout << "username: " << username << std::endl;
    auto it = loginrecorder.find(username);
    if (it == loginrecorder.end()) {
        std::cout << "-1" << std::endl;
        return;
    }
    loginrecorder.erase(it);
    std::cout << "0" << std::endl;
}

void Program::QueryProfile(const sjtu::string<20> &cur_username, const sjtu::string<20> &username) {
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

void Program::ModifyProfile(const sjtu::string<20> &cur_username, const sjtu::string<20> &username, const sjtu::string<30> &password, const sjtu::string<15> &name, const sjtu::string<30> &mailAddr, int privilege) {
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

void Program::AddTrain(const sjtu::string<20> &trainID, int stationNum, int seatNum, sjtu::vector<sjtu::string<30>> &stations, sjtu::vector<int> &prices, int startTime, sjtu::vector<int> &travelTimes, sjtu::vector<int> &stopoverTimes, sjtu::pair<int, int> saleDate, char type) {
    // std::cout << "trainID: " << trainID << '\n';
    // std::cout << "stationNum: " << stationNum << '\n';
    // std::cout << "seatNum: " << seatNum << '\n';
    // std::cout << "stations: ";
    // for (auto station : stations) {
    //     std::cout << station << ' ';
    // }
    // std::cout << '\n';
    // std::cout << "prices: ";
    // for (auto price : prices) {
    //     std::cout << price << ' ';
    // }
    // std::cout << '\n';
    // std::cout << "startTime: " << startTime / 60 << ':' << startTime % 60 << '\n';
    // for (auto travelTime : travelTimes) {
    //     std::cout << travelTime << ' ';
    // }
    // std::cout << '\n';
    // std::cout << "stopoverTimes: ";
    // for (auto stopoverTime : stopoverTimes) {
    //     std::cout << stopoverTime << ' ';
    // }
    // std::cout << '\n';
    // std::cout << "saleDate: " << saleDate.first << ' ' << saleDate.second << '\n';
    // std::cout << "type: " << type << std::endl;
    if (!trainmanager.getTrainIndex(trainID).empty()) {
        std::cout << "-1" << std::endl;
        return;
    }
    trainmanager.addTrain(trainID, stationNum, seatNum, stations, prices, startTime, travelTimes, stopoverTimes, saleDate, type);
    std::cout << "0" << std::endl;
}

void Program::DeleteTrain(const sjtu::string<20> &trainID) {
    // std::cout << "trainID: " << trainID << std::endl;
    sjtu::vector<int> indexs = trainmanager.getTrainIndex(trainID);
    if (indexs.empty()) {
        std::cout << "-1" << std::endl;
        return;
    }
    Train train = trainmanager.getTrain(indexs[0]);
    if (train.state != 0) {
        std::cout << "-1" << std::endl;
    }
    trainmanager.deleteTrain(indexs);
    std::cout << "0" << std::endl;
}

void Program::ReleaseTrain(const sjtu::string<20> &trainID) {
    // std::cout << "trainID: " << trainID << std::endl;
    sjtu::vector<int> indexs = trainmanager.getTrainIndex(trainID);
    if (indexs.empty()) {
        std::cout << "-1" << std::endl;
        return;
    }
    Train train = trainmanager.getTrain(indexs[0]);
    if (train.state != 0) {
        std::cout << "-1" << std::endl;
        return;
    }
    trainmanager.releaseTrain(indexs);
    std::cout << "0" << std::endl;
}

void Program::QueryTrain(const sjtu::string<20> &trainID, int date) {
    // std::cout << "trainID: " << trainID << '\n';
    // std::cout << "Date: " << date << std::endl;
    sjtu::vector<int> indexs = trainmanager.getTrainIndex(trainID);
    if (indexs.empty()) {
        std::cout << "-1" << std::endl;
        return;
    }
    Train train = trainmanager.getTrain(indexs[0]);
    int startdate = train.leavingTimes[0] / daytime;
    int index = date - startdate;
    if (index < 0 || index >= indexs.size() || train.state == 2) {
        std::cout << "-1" << std::endl;
        return;
    }
    train = trainmanager.getTrain(indexs[index]);
    int last = train.stationNum - 1;
    std::cout << trainID << ' ' << train.type << '\n';
    std::cout << trainmanager.getStation(train.stations[0]) << ' ' << "xx-xx xx:xx -> ";
    printTime(train.leavingTimes[0]);
    std::cout << ' ' << train.sum_prices[0] << ' ' << train.seats[0] << '\n';
    for (int i = 1; i < last; i++) {
        std::cout << trainmanager.getStation(train.stations[i]) << ' ';
        printTime(train.arrivingTimes[i]);
        std::cout << " -> ";
        printTime(train.leavingTimes[i]);
        std::cout << ' ' << train.sum_prices[i] << ' ' << train.seats[i] << '\n';
    }
    std::cout << trainmanager.getStation(train.stations[last]) << ' ';
    printTime(train.arrivingTimes[last]);
    std::cout << " -> xx-xx xx:xx " << train.sum_prices[last] << " x" << std::endl;
}

void Program::QueryTicket(const sjtu::string<30> &station1, const sjtu::string<30> &station2, int date, bool query_type) {
    // std::cout << "station1: " << station1 << '\n';
    // std::cout << "station2: " << station2 << '\n';
    // std::cout << "date: " << date << '\n';
    // if (!query_type) {
    //     std::cout << "time" << std::endl;
    // } else {
    //     std::cout << "cost" << std::endl;
    // }
}

void Program::QueryTransfer(const sjtu::string<30> &station1, const sjtu::string<30> station2, int date, bool query_type) {
    std::cout << "station1: " << station1 << '\n';
    std::cout << "station2: " << station2 << '\n';
    std::cout << "date: " << date << '\n';
    if (!query_type) {
        std::cout << "time" << std::endl;
    } else {
        std::cout << "cose" << std::endl;
    }
}

void Program::BuyTicket(const sjtu::string<20> &username, const sjtu::string<20> &trainID, int date, int ticketnum, sjtu::string<30> &station1, sjtu::string<30> &station2, bool buy_type) {
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

void Program::QueryOrder(const sjtu::string<20> &username) {
    std::cout << "username: " << username << std::endl;
}

void Program::RefundTicket(const sjtu::string<20> &username, int ticketnum) {
    std::cout << "username: " << username << '\n';
    std::cout << "ticketnum: " << ticketnum << std::endl;
}

void Program::Clean() {
    
}

void Program::Exit() {
    programEnd_ = true;
    std::cout << "bye" << std::endl;
}