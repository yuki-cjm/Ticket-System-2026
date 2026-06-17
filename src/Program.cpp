#include <iostream>
#include <ostream>
#include <string>
#include <fstream>
#include <cmath>

#include "Program.hpp"
#include "AccountManager.hpp"
#include "TrainManager.hpp"
#include "OrderManager.hpp"
#include "STLite/vector.hpp"
#include "STLite/pair.hpp"
#include "STLite/map.hpp"
#include "STLite/string.hpp"
#include "STLite/deque.hpp"
#include "utils/constants.hpp"

Program::Program() : total_filename("total") {
    total_file.open(total_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!total_file) {
        total_file.open(total_filename, std::ios::out | std::ios::binary);
        total_file.close();
        total_file.open(total_filename, std::ios::in | std::ios::out | std::ios::binary);
        accountmanager.changeAccountCount(0);
        trainmanager.changeTrainIDCount(0);
        trainmanager.changeTrainCount(0);
        ordermanager.changeOrderCount(0);
        pendingorders.clear();
    } else {
        total_file.seekg(0);
        int count, pendingorder;
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        accountmanager.changeAccountCount(count);
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        trainmanager.changeTrainIDCount(count);
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        trainmanager.changeTrainCount(count);
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        ordermanager.changeOrderCount(count);
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        pendingorders.clear();
        for (int i = 0; i < count; i++) {
            total_file.read(reinterpret_cast<char*>(&pendingorder), sizeofint);
            pendingorders.push_back(pendingorder);
        }
    }
    loginrecorder.clear();
}

Program::~Program() {
    total_file.seekp(0);
    int count;
    count = accountmanager.getAccountCount();
    total_file.write(reinterpret_cast<char*>(&count), sizeofint);
    count = trainmanager.getTrainIDCount();
    total_file.write(reinterpret_cast<char*>(&count), sizeofint);
    count = trainmanager.getTrainCount();
    total_file.write(reinterpret_cast<char*>(&count), sizeofint);
    count = ordermanager.getOrderCount();
    total_file.write(reinterpret_cast<char*>(&count), sizeofint);
    count = pendingorders.size();
    total_file.write(reinterpret_cast<char*>(&count), sizeofint);
    for (int pendingorder : pendingorders) {
        total_file.write(reinterpret_cast<char*>(&pendingorder), sizeofint);
    }
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

void Program::AddUser(sjtu::string<UserNameLength> &cur_username, sjtu::string <UserNameLength>&username, sjtu::string<PassWordLength> &password, sjtu::string<NameLength> &name, sjtu::string<MailAddrLength> &mailAddr, int privilege) {
    if (accountmanager.getAccountCount() == 0) {
        accountmanager.addAccount(username, password, name, mailAddr, 10);
        std::cout << "0\n";
        return;
    }
    if (loginrecorder.find(cur_username) == loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    int cur_privilege = loginrecorder[cur_username];
    if (privilege >= cur_privilege) {
        std::cout << "-1\n";
        return;
    }
    if (accountmanager.getIndex(username) != -1) {
        std::cout << "-1\n";
        return;
    }
    accountmanager.addAccount(username, password, name, mailAddr, privilege);
    std::cout << "0\n";
}

void Program::Login(const sjtu::string<UserNameLength> &username, const sjtu::string<PassWordLength> &password) {
    int index = accountmanager.getIndex(username);
    if (index == -1) {
        std::cout << "-1\n";
        return;
    }
    if (loginrecorder.find(username) != loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    Account account = accountmanager.getAccount(index);
    if (account.password == password) {
        loginrecorder.insert(sjtu::pair(username, account.privilege));
        std::cout << "0\n";
        return;
    }
    std::cout << "-1\n";
}

void Program::Logout(const sjtu::string<UserNameLength> &username) {
    auto it = loginrecorder.find(username);
    if (it == loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    loginrecorder.erase(it);
    std::cout << "0\n";
}

void Program::QueryProfile(const sjtu::string<UserNameLength> &cur_username, const sjtu::string<UserNameLength> &username) {
    auto it = loginrecorder.find(cur_username);
    if (it == loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    int index = accountmanager.getIndex(username);
    if (index == -1) {
        std::cout << "-1\n";
        return;
    }
    Account account = accountmanager.getAccount(index);
    if (it->second < account.privilege || it->second == account.privilege && cur_username != username) {
        std::cout << "-1\n";
        return;
    }
    std::cout << username << ' ' << account.name << ' ' << account.mailAddr << ' ' << account.privilege << '\n';
}

void Program::ModifyProfile(const sjtu::string<UserNameLength> &cur_username, const sjtu::string<UserNameLength> &username, const sjtu::string<PassWordLength> &password, const sjtu::string<NameLength> &name, const sjtu::string<MailAddrLength> &mailAddr, int privilege) {
    auto it = loginrecorder.find(cur_username);
    if (it == loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    if (privilege != -1 && privilege >= it->second) {
        std::cout << "-1\n";
        return;
    }
    int index = accountmanager.getIndex(username);
    if (index == -1) {
        std::cout << "-1\n";
        return;
    }
    Account account = accountmanager.getAccount(index);
    if (it->second < account.privilege || it->second == account.privilege && cur_username != username) {
        std::cout << "-1\n";
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
    std::cout << username << ' ' << account.name << ' ' << account.mailAddr << ' ' << account.privilege << '\n';
}

void Program::AddTrain(sjtu::string<TrainIDLength> &trainID, int stationNum, int seatNum, sjtu::string<StationLength> *stations, int *prices, int startTime, int *travelTimes, int *stopoverTimes, sjtu::pair<int, int> saleDate, char type) {
    if (trainmanager.getTrainIDIndex(trainID) != -1) {
        std::cout << "-1\n";
        return;
    }

    trainmanager.addTrain(trainID, stationNum, seatNum, stations, prices, startTime, travelTimes, stopoverTimes, saleDate, type);
    std::cout << "0\n";
}

void Program::DeleteTrain(sjtu::string<TrainIDLength> &trainID) {
    int index = trainmanager.getTrainIDIndex(trainID);
    if (index == -1) {
        std::cout << "-1\n";
        return;
    }
    if (trainmanager.getTrainState(index) != 0) {
        std::cout << "-1\n";
        return;
    }
    trainmanager.deleteTrain(trainID, index);
    std::cout << "0\n";
}

void Program::ReleaseTrain(sjtu::string<TrainIDLength> &trainID) {
    int index = trainmanager.getTrainIDIndex(trainID);
    if (index == -1) {
        std::cout << "-1\n";
        return;
    }
    if (trainmanager.getTrainState(index) != 0) {
        std::cout << "-1\n";
        return;
    }
    trainmanager.releaseTrain(index);
    std::cout << "0\n";
}

void Program::QueryTrain(const sjtu::string<TrainIDLength> &trainID, int date) {
    int index = trainmanager.getTrainIDIndex(trainID);
    if (index == -1) {
        std::cout << "-1\n";
        return;
    }
    trainmanager.queryTrain(index, date);
}

void Program::QueryTicket(sjtu::string<StationLength> &station1, sjtu::string<StationLength> &station2, int date, bool query_type) {
    trainmanager.queryTicket(station1, station2, date, query_type);
}

void Program::QueryTransfer(sjtu::string<StationLength> &station1, sjtu::string<StationLength> station2, int date, bool query_type) {
    trainmanager.queryTransfer(station1, station2, date, query_type);
}

void Program::BuyTicket(const sjtu::string<UserNameLength> &username, sjtu::string<TrainIDLength> &trainID, int date, int ticketnum, sjtu::string<StationLength> &station1, sjtu::string<StationLength> &station2, bool buy_type) {
    if (loginrecorder.find(username) == loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    int trainID_index = trainmanager.getTrainIDIndex(trainID);
    if (trainID_index == -1) {
        std::cout << "-1\n";
        return;
    }
    
    if (trainmanager.getTrainState(trainID_index) != 1 || ticketnum > trainmanager.getSeatNum(trainID_index)) {
        std::cout << "-1\n";
        return;
    }
    TrainBasic trainbasic = trainmanager.getTrainBasic(trainID_index);
    int dayoff, origin, destination, train_index;
    for (origin = 0; origin < trainbasic.stationNum && station1 != trainbasic.stations[origin]; origin++); 
    if (origin == trainbasic.stationNum) {
        std::cout << "-1\n";
        return;
    }
    dayoff = date - trainbasic.leavingTimes[origin] / daytime;
    if (dayoff < 0 || dayoff >= trainbasic.trainNum) {
        std::cout << "-1\n";
        return;
    }
    for (destination = origin + 1; destination < trainbasic.stationNum && station2 != trainbasic.stations[destination]; destination++);
    if (destination == trainbasic.stationNum) {
        std::cout << "-1\n";
        return;
    }
    train_index = trainbasic.headtrainindex + dayoff;
    Train train = trainmanager.getTrain(train_index);
    int seat = trainbasic.seatNum;
    for (int i = origin; i < destination; i++) {
        if (train.seats[i] < seat) {
            seat = train.seats[i];
        }
    }
    if (seat < ticketnum) {
        if (!buy_type) {
            std::cout << "-1\n";
        } else {
            pendingorders.push_back(ordermanager.getOrderCount());
            ordermanager.addPendingOrder(username, trainID, station1, station2, origin, destination, trainbasic.leavingTimes[origin] + dayoff * daytime, trainbasic.arrivingTimes[destination] + dayoff * daytime, trainbasic.sum_prices[destination] - trainbasic.sum_prices[origin], ticketnum, train_index);
            std::cout << "queue\n";
        }
        return;
    }
    ordermanager.addSuccessOrder(username, trainID, station1, station2, origin, destination, trainbasic.leavingTimes[origin] + dayoff * daytime, trainbasic.arrivingTimes[destination] + dayoff * daytime, trainbasic.sum_prices[destination] - trainbasic.sum_prices[origin], ticketnum, train_index);
    for (int station = origin; station < destination; station++) {
        train.seats[station] -= ticketnum;
    }
    trainmanager.writeTrain(train_index, train);
    std::cout << (trainbasic.sum_prices[destination] - trainbasic.sum_prices[origin]) * ticketnum << '\n';
}

void Program::QueryOrder(const sjtu::string<UserNameLength> &username) {
    if (loginrecorder.find(username) == loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    sjtu::vector<int> orderindexs = ordermanager.getOrderIndexs(username);
    if (orderindexs.empty()) {
        std::cout << "0\n";
        return;
    }
    Order order;
    std::cout << orderindexs.size() << std::endl;
    sjtu::vector<int>::iterator it = orderindexs.end();
    do {
        it--;
        order = ordermanager.getOrder(*it);
        if (order.state == 0) {
            std::cout << "[success] ";
        } else if(order.state == 1) {
            std::cout << "[pending] ";
        } else if (order.state == 2) {
            std::cout << "[refunded] ";
        }
        std::cout << order.trainID << ' ' << order.station1 << ' ';
        printTime(order.leavingtime);
        std::cout << " -> " << order.station2 << ' ';
        printTime(order.arrivingtime);
        std::cout << ' ' << order.price << ' ' << order.num << '\n'; 
    } while(it != orderindexs.begin());
}

void Program::RefundTicket(const sjtu::string<UserNameLength> &username, int ticketnum) {
    if (loginrecorder.find(username) == loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    sjtu::vector<int> orderindexs = ordermanager.getOrderIndexs(username);
    if (orderindexs.size() < ticketnum) {
        std::cout << "-1\n";
        return;
    }
    OrderView orderview;
    sjtu::vector<int>::iterator it = orderindexs.end() - ticketnum;
    int state = ordermanager.getOrderState(*it);
    orderview = ordermanager.getOrderView(*it);
    if (state == 0) {
        Order pendingorder;
        Train train;
        train = trainmanager.getTrain(orderview.train_index);
        for (int station = orderview.origin; station < orderview.destination; station++) {
            train.seats[station] += orderview.num;
        }
        sjtu::deque<int>::iterator it2 = pendingorders.begin();
        while (it2 != pendingorders.end()) {
            pendingorder = ordermanager.getOrder(*it2);
            if (orderview.train_index != pendingorder.train_index) {
                it2++;
                continue;
            }
            int available_seat = maxSeatNum;
            for (int station = pendingorder.origin; station < pendingorder.destination; station++) {
                if (train.seats[station] < available_seat) {
                    available_seat = train.seats[station];
                }
            }
            if (available_seat >= pendingorder.num) {
                for (int station = pendingorder.origin; station < pendingorder.destination; station++) {
                    train.seats[station] -= pendingorder.num;
                }
                ordermanager.writeOrderState(*it2, 0);
                it2 = pendingorders.erase(it2);
            } else {
                it2++;
            }
        }
        trainmanager.writeTrain(orderview.train_index, train);
    } else if (state == 1) {
        for (auto it2 = pendingorders.begin(); it2 != pendingorders.end(); it2++) {
            if (*it2 == *it) {
                pendingorders.erase(it2);
                break;
            }
        }
    } else if (state == 2) {
        std::cout << "-1\n";
        return;
    }
    ordermanager.writeOrderState(*it, 2);
    std::cout << "0\n";
}

void Program::Clean() {
    total_file.close();
    total_file.open(total_filename, std::ios::out | std::ios::trunc | std::ios::binary);
    total_file.close();
    total_file.open(total_filename, std::ios::in | std::ios::out | std::ios::binary);
    accountmanager.clean();
    trainmanager.clean();
    ordermanager.clean();
    loginrecorder.clear();
    pendingorders.clear();
    std::cout << "0\n";
}

void Program::Exit() {
    programEnd_ = true;
    std::cout << "bye\n";
}