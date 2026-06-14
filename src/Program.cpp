#include <iostream>
#include <string>
#include <fstream>

#include "Program.hpp"
#include "AccountManager.hpp"
#include "TrainManager.hpp"
#include "OrderManager.hpp"
#include "STLite/vector.hpp"
#include "STLite/pair.hpp"
#include "STLite/map.hpp"
#include "STLite/string.hpp"
#include "STLite/set.hpp"
#include "STLite/deque.hpp"

constexpr int daytime = 1440;
constexpr int hourtime = 60;

void printNum(int num) {
    if (num >= 10) {
        std::cout << num;
    } else if (num >= 1) {
        std::cout << '0' << num;
    } else {
        std::cout << "00";
    }
}

const int monthday[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void printTime(int time) {
    int day = time / daytime, month;
    for (month = 1; month <= 12; month++) {
        if (day > monthday[month]) {
            day -= monthday[month];
        } else {
            break;
        }
    }
    time %= daytime;
    printNum(month);
    std::cout << '-';
    printNum(day);
    std::cout << ' ';
    printNum(time / hourtime);
    std::cout << ':';
    printNum(time % hourtime);
}

Program::Program() : total_filename("total") {
    total_file.open(total_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!total_file) {
        total_file.open(total_filename, std::ios::out | std::ios::binary);
        total_file.close();
        total_file.open(total_filename, std::ios::in | std::ios::out | std::ios::binary);
        accountmanager.changeAccountCount(0);
        trainmanager.changeTrainCount(0);
        trainmanager.changeStationCount(0);
        ordermanager.changeOrderCount(0);
        pendingorders.clear();
    } else {
        total_file.seekg(0);
        int count, pendingorder;
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        accountmanager.changeAccountCount(count);
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        trainmanager.changeTrainCount(count);
        total_file.read(reinterpret_cast<char*>(&count), sizeofint);
        trainmanager.changeStationCount(count);
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
    count = trainmanager.getTrainCount();
    total_file.write(reinterpret_cast<char*>(&count), sizeofint);
    count = trainmanager.getStationCount();
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

void Program::AddUser(sjtu::string<20> &cur_username, sjtu::string <20>&username, sjtu::string<30> &password, sjtu::string<15> &name, sjtu::string<30> &mailAddr, int privilege) {
    // std::cout << "cur_username: " << cur_username << '\n';
    // std::cout << "username: " << username << '\n';
    // std::cout << "password: " << password << '\n';
    // std::cout << "name: " << name << '\n';
    // std::cout << "mailAddr: " << mailAddr << '\n';
    // std::cout << "privilege: " << privilege << std::endl;

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

void Program::Login(const sjtu::string<20> &username, const sjtu::string<30> &password) {
    // std::cout << "username: " << username << '\n';
    // std::cout << "password: " << password << std::endl;
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

void Program::Logout(const sjtu::string<20> &username) {
    // std::cout << "username: " << username << std::endl;
    auto it = loginrecorder.find(username);
    if (it == loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    loginrecorder.erase(it);
    std::cout << "0\n";
}

void Program::QueryProfile(const sjtu::string<20> &cur_username, const sjtu::string<20> &username) {
    // std::cout << "cur_username: " << cur_username << '\n';
    // std::cout << "username: " << username << std::endl;
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

void Program::AddTrain(sjtu::string<20> &trainID, int stationNum, int seatNum, sjtu::vector<sjtu::string<30>> &stations, sjtu::vector<int> &prices, int startTime, sjtu::vector<int> &travelTimes, sjtu::vector<int> &stopoverTimes, sjtu::pair<int, int> saleDate, char type) {
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
    if (!trainmanager.getTrainIndexs(trainID).empty()) {
        std::cout << "-1\n";
        return;
    }
    trainmanager.addTrain(trainID, stationNum, seatNum, stations, prices, startTime, travelTimes, stopoverTimes, saleDate, type);
    std::cout << "0\n";
}

void Program::DeleteTrain(sjtu::string<20> &trainID) {
    // std::cout << "trainID: " << trainID << std::endl;
    sjtu::vector<int> indexs = trainmanager.getTrainIndexs(trainID);
    if (indexs.empty()) {
        std::cout << "-1\n";
        return;
    }
    Train train = trainmanager.getTrain(indexs[0]);
    if (train.state != 0) {
        std::cout << "-1\n";
        return;
    }
    trainmanager.deleteTrain(trainID, indexs);
    std::cout << "0\n";
}

void Program::ReleaseTrain(const sjtu::string<20> &trainID) {
    // std::cout << "trainID: " << trainID << std::endl;
    sjtu::vector<int> indexs = trainmanager.getTrainIndexs(trainID);
    if (indexs.empty()) {
        std::cout << "-1\n";
        return;
    }
    Train train = trainmanager.getTrain(indexs[0]);
    if (train.state != 0) {
        std::cout << "-1\n";
        return;
    }
    trainmanager.releaseTrain(indexs);
    std::cout << "0\n";
}

void Program::QueryTrain(const sjtu::string<20> &trainID, int date) {
    // std::cout << "trainID: " << trainID << '\n';
    // std::cout << "Date: " << date << std::endl;
    sjtu::vector<int> indexs = trainmanager.getTrainIndexs(trainID);
    if (indexs.empty()) {
        std::cout << "-1\n";
        return;
    }
    Train train = trainmanager.getTrain(indexs[0]);
    int startdate = train.leavingTimes[0] / daytime;
    int index = date - startdate;
    if (index < 0 || index >= indexs.size() || train.state == 2) {
        std::cout << "-1\n";
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
    std::cout << " -> xx-xx xx:xx " << train.sum_prices[last] << " x\n";
}

void Program::QueryTicket(sjtu::string<30> &station1, sjtu::string<30> &station2, int date, bool query_type) {
    // std::cout << "station1: " << station1 << '\n';
    // std::cout << "station2: " << station2 << '\n';
    // std::cout << "date: " << date << '\n';
    // if (!query_type) {
    //     std::cout << "time" << std::endl;
    // } else {
    //     std::cout << "cost" << std::endl;
    // }
    bool (*compare)(const Ticket &lhs, const Ticket &rhs) = nullptr;
    if (!query_type) {
        compare = ordermanager.compareTicketTime;
    } else {
        compare = ordermanager.compareTicketCost;
    }

    int fromstation_index = trainmanager.getStationIndex(station1);
    int tostation_index = trainmanager.getStationIndex(station2);
    StationDate stationdate(fromstation_index, date);
    sjtu::vector<sjtu::pair<int, int>> trainstations = trainmanager.getStationDateTrainStations(stationdate);
    Train train;
    sjtu::set<Ticket> tickets(compare);
    Ticket ticket;
    int trainindex, fromstation, tostation, available_seat;
    for (sjtu::pair<int, int> trainstation : trainstations) {
        trainindex = trainstation.first;
        fromstation = trainstation.second;
        train = trainmanager.getTrain(trainindex);
        available_seat = 1e5;
        for (tostation = trainstation.second + 1; tostation < train.stationNum; tostation++) {
            if (train.seats[tostation - 1] < available_seat) {
                available_seat = train.seats[tostation - 1];
            }
            if (train.stations[tostation] == tostation_index) {
                ticket.trainID = train.trainID;
                ticket.leavingtime = train.leavingTimes[fromstation];
                ticket.arrivingtime = train.arrivingTimes[tostation];
                ticket.price = train.sum_prices[tostation] - train.sum_prices[fromstation];
                ticket.seat = available_seat;
                tickets.insert(ticket);
                break;
            }
        }
    }
    std::cout << tickets.size() << '\n';
    for (Ticket &ticket : tickets) {
        std::cout << ticket.trainID << ' ' << station1 << ' ';
        printTime(ticket.leavingtime);
        std::cout << " -> ";
        std::cout << station2 << ' ';
        printTime(ticket.arrivingtime);
        std::cout << ' ' << ticket.price << ' ' << ticket.seat << '\n';
    }
}

void Program::QueryTransfer(sjtu::string<30> &station1, sjtu::string<30> station2, int date, bool query_type) {
    // std::cout << "station1: " << station1 << '\n';
    // std::cout << "station2: " << station2 << '\n';
    // std::cout << "date: " << date << '\n';
    // if (!query_type) {
    //     std::cout << "time" << std::endl;
    // } else {
    //     std::cout << "cose" << std::endl;
    // }
    bool (*compare)(const Transfer &lhs, const Transfer &rhs) = nullptr;
    if (!query_type) {
        compare = ordermanager.compareTransferTime;
    } else {
        compare = ordermanager.compareTransferCost;
    }

    int fromstation_index = trainmanager.getStationIndex(station1);
    int tostation_index = trainmanager.getStationIndex(station2);
    StationDate stationdate(fromstation_index, date);
    sjtu::vector<sjtu::pair<int, int>> trainstations1, trainstations2;
    trainstations1 = trainmanager.getStationDateTrainStations(stationdate);
    Train train1, train2;
    int fromstation1, fromstation2;
    int tostation1, tostation2;
    int transferstation_index;
    int available_seat1, available_seat2;
    sjtu::set<Transfer> transfers(compare);
    Transfer transfer;
    for (sjtu::pair<int, int> trainstation1 : trainstations1) {
        train1 = trainmanager.getTrain(trainstation1.first);
        fromstation1 = trainstation1.second;
        available_seat1 = 1e5;
        for (tostation1 = fromstation1 + 1; tostation1 < train1.stationNum; tostation1++) {
            transferstation_index = train1.stations[tostation1];
            stationdate.station = transferstation_index;
            stationdate.date = -1;
            trainstations2 = trainmanager.getStationDateTrainStations(stationdate);
            if (train1.seats[tostation1 - 1] < available_seat1) {
                available_seat1 = train1.seats[tostation1 - 1];
            }
            for (sjtu::pair<int, int> trainstation2 : trainstations2) {
                train2 = trainmanager.getTrain(trainstation2.first);
                if (train1.trainID == train2.trainID) continue;
                fromstation2 = trainstation2.second;
                if (train1.arrivingTimes[tostation1] > train2.leavingTimes[fromstation2]) {
                    continue;
                }
                available_seat2 = 1e5;
                for (tostation2 = fromstation2 + 1; tostation2 < train2.stationNum; tostation2++) {
                    if (train2.seats[tostation2 - 1] < available_seat2) {
                        available_seat2 = train2.seats[tostation2 - 1];
                    }
                    if (train2.stations[tostation2] == tostation_index) {
                        transfer.train1ID = train1.trainID;
                        transfer.train2ID = train2.trainID;
                        transfer.transferstation = trainmanager.getStation(transferstation_index);
                        transfer.leavingtime1 = train1.leavingTimes[fromstation1];
                        transfer.arrivingtime1 = train1.arrivingTimes[tostation1];
                        transfer.leavingtime2 = train2.leavingTimes[fromstation2];
                        transfer.arrivingtime2 = train2.arrivingTimes[tostation2];
                        transfer.price1 = train1.sum_prices[tostation1] - train1.sum_prices[fromstation1];
                        transfer.price2 = train2.sum_prices[tostation2] - train2.sum_prices[fromstation2];
                        transfer.seat1 = available_seat1;
                        transfer.seat2 = available_seat2;
                        transfers.insert(transfer);
                        break;
                    }
                }
            }
        }
    }
    if (transfers.size()) {
        transfer = *(transfers.begin());
        std::cout << transfer.train1ID << ' ' << station1 << ' ';
        printTime(transfer.leavingtime1);
        std::cout << " -> " << transfer.transferstation << ' ';
        printTime(transfer.arrivingtime1);
        std::cout << ' ' << transfer.price1 << ' ' << transfer.seat1 << '\n';
        std::cout << transfer.train2ID << ' ' << transfer.transferstation << ' ';
        printTime(transfer.leavingtime2);
        std::cout << " -> " << station2 << ' ';
        printTime(transfer.arrivingtime2);
        std::cout << ' ' << transfer.price2 << ' ' << transfer.seat2 << '\n';
    } else {
        std::cout << "0\n";
    }
}

void Program::BuyTicket(const sjtu::string<20> &username, const sjtu::string<20> &trainID, int date, int ticketnum, sjtu::string<30> &station1, sjtu::string<30> &station2, bool buy_type) {
    // std::cout << "username: " << username << '\n';
    // std::cout << "trainID: " << trainID << '\n';
    // std::cout << "date: " << date << '\n';
    // std::cout << "ticketnum: " << ticketnum << '\n';
    // std::cout << "station1: " << station1 << '\n';
    // std::cout << "station2: " << station2 << '\n';
    // if (!buy_type) {
    //     std::cout << "false" << std::endl;
    // } else {
    //     std::cout << "true" << std::endl;
    // }
    if (loginrecorder.find(username) == loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    sjtu::vector<int> trainindexs = trainmanager.getTrainIndexs(trainID);
    if (trainindexs.empty()) {
        std::cout << "-1\n";
        return;
    }
    Train train = trainmanager.getTrain(trainindexs[0]);
    if (train.state != 1 || ticketnum > train.seatNum) {
        std::cout << "-1\n";
        return;
    }
    int fromstation_index = trainmanager.getStationIndex(station1);
    int tostation_index = trainmanager.getStationIndex(station2);
    int fromstation, tostation;
    for (fromstation = 0; fromstation < train.stationNum && train.stations[fromstation] != fromstation_index; fromstation++) {}
    for (tostation = 0; tostation < train.stationNum && train.stations[tostation] != tostation_index; tostation++) {}
    if (fromstation == train.stationNum || tostation == train.stationNum || fromstation >= tostation) {
        std::cout << "-1\n";
        return;
    }
    int dateoffset = date - train.leavingTimes[fromstation] / daytime;
    if (dateoffset < 0 || dateoffset >= trainindexs.size()) {
        std::cout << "-1\n";
        return;
    }
    int trainindex = trainindexs[dateoffset];
    train = trainmanager.getTrain(trainindex);
    int available_seat = 1e5;
    for (int station = fromstation; station < tostation; station++) {
        if (train.seats[station] < available_seat) {
            available_seat = train.seats[station];
        }
    }
    int price = train.sum_prices[tostation] - train.sum_prices[fromstation];
    if (available_seat < ticketnum) {
        if (!buy_type) {
            std::cout << "-1\n";
        } else {
            pendingorders.push_back(ordermanager.getOrderCount());
            ordermanager.addPendingOrder(username, trainindex, fromstation, tostation, fromstation_index, tostation_index, train.leavingTimes[fromstation], train.arrivingTimes[tostation], price, ticketnum);
            std::cout << "queue\n";
        }
        return;
    }
    ordermanager.addSuccessOrder(username, trainindex, fromstation, tostation, fromstation_index, tostation_index, train.leavingTimes[fromstation], train.arrivingTimes[tostation], price, ticketnum);
    for (int station = fromstation; station < tostation; station++) {
        train.seats[station] -= ticketnum;
    }
    trainmanager.writeTrain(trainindex, train);
    std::cout << (long long)(price) * ticketnum << '\n';
}

void Program::QueryOrder(const sjtu::string<20> &username) {
    // std::cout << "username: " << username << std::endl;
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
    std::cout << orderindexs.size() << '\n';
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
        std::cout << trainmanager.getTrainID(order.train) << ' ' << trainmanager.getStation(order.fromstation_index) << ' ';
        printTime(order.leavingtime);
        std::cout << " -> " << trainmanager.getStation(order.tostation_index) << ' ';
        printTime(order.arrivingtime);
        std::cout << ' ' << order.price << ' ' << order.num << '\n'; 
    } while(it != orderindexs.begin());
}

void Program::RefundTicket(const sjtu::string<20> &username, int ticketnum) {
    // std::cout << "username: " << username << '\n';
    // std::cout << "ticketnum: " << ticketnum << std::endl;
    if (loginrecorder.find(username) == loginrecorder.end()) {
        std::cout << "-1\n";
        return;
    }
    sjtu::vector<int> orderindexs = ordermanager.getOrderIndexs(username);
    if (orderindexs.size() < ticketnum) {
        std::cout << "-1\n";
        return;
    }
    Order order;
    sjtu::vector<int>::iterator it = orderindexs.end() - ticketnum;
    order = ordermanager.getOrder(*it);
    if (order.state == 0) {
        Order pendingorder;
        Train train;
        train = trainmanager.getTrain(order.train);
        for (int station = order.fromstation; station < order.tostation; station++) {
            train.seats[station] += order.num;
        }
        sjtu::deque<int>::iterator it2 = pendingorders.begin();
        while (it2 != pendingorders.end()) {
            pendingorder = ordermanager.getOrder(*it2);
            if (order.train != pendingorder.train) {
                it2++;
                continue;
            }
            int available_seat = 1e5;
            for (int station = pendingorder.fromstation; station < pendingorder.tostation; station++) {
                if (train.seats[station] < available_seat) {
                    available_seat = train.seats[station];
                }
            }
            if (available_seat >= pendingorder.num) {
                for (int station = pendingorder.fromstation; station < pendingorder.tostation; station++) {
                    train.seats[station] -= pendingorder.num;
                }
                pendingorder.state = 0;
                ordermanager.writeOrder(*it2, pendingorder);
                it2 = pendingorders.erase(it2);
            } else {
                it2++;
            }
        }
        trainmanager.writeTrain(order.train, train);
    } else if (order.state == 1) {
        for (auto it2 = pendingorders.begin(); it2 != pendingorders.end(); it2++) {
            if (*it2 == *it) {
                pendingorders.erase(it2);
                break;
            }
        }
    } else if (order.state == 2) {
        std::cout << "-1\n";
        return;
    }
    order.state = 2;
    ordermanager.writeOrder(*it, order);
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