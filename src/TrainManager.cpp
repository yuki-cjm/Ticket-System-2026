#include <fstream>
#include <cstring>

#include "TrainManager.hpp"
#include "STLite/vector.hpp"
#include "STLite/BPT.hpp"
#include "STLite/pair.hpp"
#include "STLite/string.hpp"
#include "STLite/set.hpp"
#include "utils/constants.hpp"

bool TrainManager::compareTicketTime(const Ticket &lhs, const Ticket &rhs) {
    if (lhs.arrivingtime - lhs.leavingtime < rhs.arrivingtime - rhs.leavingtime) { return true; }
    if (lhs.arrivingtime - lhs.leavingtime > rhs.arrivingtime - rhs.leavingtime) { return false; }
    return lhs.trainID < rhs.trainID;
}

bool TrainManager::compareTicketCost(const Ticket &lhs, const Ticket &rhs) {
    if (lhs.price < rhs.price) { return true; }
    if (lhs.price > rhs.price) { return false; }
    return lhs.trainID < rhs.trainID;
}

bool TrainManager::compareTransferTime(const Transfer &lhs, const Transfer &rhs) {
    if (lhs.arrivingtime2 - lhs.leavingtime1 < rhs.arrivingtime2 - rhs.leavingtime1) { return true; }
    if (lhs.arrivingtime2 - lhs.leavingtime1 > rhs.arrivingtime2 - rhs.leavingtime1) { return false; }
    if (lhs.price1 + lhs.price2 < rhs.price1 + rhs.price2) { return true; }
    if (lhs.price1 + lhs.price2 > rhs.price1 + rhs.price2) { return false; }
    if (lhs.train1ID < rhs.train1ID) { return true; }
    if (lhs.train1ID > rhs.train1ID) { return false; }
    return lhs.train2ID < rhs.train2ID;
}

bool TrainManager::compareTransferCost(const Transfer &lhs, const Transfer &rhs) {
    if (lhs.price1 + lhs.price2 < rhs.price1 + rhs.price2) { return true; }
    if (lhs.price1 + lhs.price2 > rhs.price1 + rhs.price2) { return false; }
    if (lhs.arrivingtime2 - lhs.leavingtime1 < rhs.arrivingtime2 - rhs.leavingtime1) { return true; }
    if (lhs.arrivingtime2 - lhs.leavingtime1 > rhs.arrivingtime2 - rhs.leavingtime1) { return false; }
    if (lhs.arrivingtime1 - lhs.leavingtime1 + lhs.arrivingtime2 - lhs.leavingtime2 > rhs.arrivingtime1 - rhs.leavingtime1 + rhs.arrivingtime2 - rhs.leavingtime2) { return false; }
    if (lhs.train1ID < rhs.train1ID) { return true; }
    if (lhs.train1ID > rhs.train1ID) { return false; }
    return lhs.train2ID < rhs.train2ID;
}

int TrainManager::getSeat(int train_index, int origin, int destination) {
    Train train;
    train_file.seekg(sizeofTrain * train_index);
    train_file.read(reinterpret_cast<char*>(&train), sizeofTrain);
    int seat = maxSeatNum;
    for (int i = origin; i < destination; i++) {
        if (train.seats[i] < seat) {
            seat = train.seats[i];
        }
    }
    return seat;
}






TrainManager::TrainManager() : train_filename("traindata"), trainID_filename("trainIDdata"), trainID_bpt("trainIDbasic", "trainIDindex"), station_bpt("stationbasic", "stationindex"), stationpair_bpt("stationpairbasic", "stationpairindex") {
    train_file.open(train_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!train_file) {
        train_file.open(train_filename, std::ios::out | std::ios::binary);
        train_file.close();
        train_file.open(train_filename, std::ios::in | std::ios::out | std::ios::binary);
    }
    trainID_file.open(trainID_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!trainID_file) {
        trainID_file.open(trainID_filename, std::ios::out | std::ios::binary);
        trainID_file.close();
        trainID_file.open(trainID_filename, std::ios::in | std::ios::out | std::ios::binary);
    }
}

TrainManager::~TrainManager() {
    train_file.close();
    trainID_file.close();
}

void TrainManager::changeTrainIDCount(int count) {
    trainID_count = count;
}


int TrainManager::getTrainIDCount() {
    return trainID_count;
}

void TrainManager::changeTrainCount(int count) {
    train_count = count;
}

int TrainManager::getTrainCount() {
    return train_count;
}

int TrainManager::getTrainIDIndex(const sjtu::string<20> &trainID) {
    sjtu::vector<int> index = trainID_bpt.find(trainID);
    if (index.empty()) {
        return -1;
    }
    return index[0];
}


void TrainManager::addTrain(sjtu::string<TrainIDLength> &trainID, int stationNum, int seatNum, sjtu::string<StationLength> *stations, int *prices, int startTime, int *travelTimes, int *stopoverTimes, sjtu::pair<int, int> saleDate, char type) {
    TrainBasic train;
    train.trainID = trainID;
    train.stationNum = stationNum;
    train.seatNum = seatNum;
    train.type = type;
    train.state = 0;
    train.trainNum = saleDate.second - saleDate.first + 1;
    train.type = type;
    for (int i = 0; i < stationNum; i++) {
        train.stations[i] = stations[i];
    }
    int sum_time = saleDate.first * daytime + startTime;
    int sum_price = 0;
    train.arrivingTimes[0] = -1;
    train.leavingTimes[0] = sum_time;
    train.sum_prices[0] = sum_price;
    for (int i = 0; i < stationNum - 2; i++) {
        sum_time += travelTimes[i];
        train.arrivingTimes[i + 1] = sum_time;
        sum_time += stopoverTimes[i];
        train.leavingTimes[i + 1] = sum_time;
        sum_price += prices[i];
        train.sum_prices[i + 1] = sum_price;
    }
    train.arrivingTimes[stationNum - 1] = sum_time + travelTimes[stationNum - 2];
    train.leavingTimes[stationNum - 1] = -1;
    train.sum_prices[stationNum - 1] = sum_price + prices[stationNum - 2];
    trainID_file.seekp(sizeofTrainBasic * trainID_count);
    trainID_file.write(reinterpret_cast<char*>(&train), sizeofTrainBasic);
    trainID_bpt.insert(trainID, trainID_count);
    trainID_count++;
}

int TrainManager::getTrainState(int index) {
    int state;
    trainID_file.seekg(sizeofTrainBasic * index);
    trainID_file.read(reinterpret_cast<char*>(&state), sizeofint);
    return state;
}

void TrainManager::deleteTrain(sjtu::string<TrainIDLength> &trainID, int index) {
    int state = 2;
    trainID_file.seekp(sizeofTrainBasic * index);
    trainID_file.write(reinterpret_cast<char*>(&state), sizeofint);
    trainID_bpt.remove(trainID, index);
}

void TrainManager::releaseTrain(int index) {
    int state = 1;
    trainID_file.seekp(sizeofTrainBasic * index);
    trainID_file.write(reinterpret_cast<char*>(&state), sizeofint);
    trainID_file.write(reinterpret_cast<char*>(&train_count), sizeofint);
    TrainBasic trainbasic;
    trainID_file.seekg(sizeofTrainBasic * index);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic), sizeofTrainBasic);

    Train train;
    for (int i = 0; i < trainbasic.stationNum - 1; i++) {
        train.seats[i] = trainbasic.seatNum;
    }
    train_file.seekp(sizeofTrain * train_count);
    for (int i = 0; i < trainbasic.trainNum; i++) {
        train_file.write(reinterpret_cast<char*>(&train), sizeofTrain);
        train_count++;
    }
    for (int i = 0; i < trainbasic.stationNum - 1; i++) { 
        for (int j = i + 1; j < trainbasic.stationNum; j++) {
            stationpair_bpt.insert(sjtu::pair(trainbasic.stations[i], trainbasic.stations[j]), sjtu::pair(index, sjtu::pair(i, j)));
        }
        station_bpt.insert(trainbasic.stations[i], sjtu::pair(index, i));
    }
}

void TrainManager::queryTrain(int trainID_index, int date) {
    TrainBasic trainbasic;
    trainID_file.seekg(sizeofTrainBasic * trainID_index);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.state), sizeofint);
    if (trainbasic.state == 2) {
        std::cout << "-1\n";
        return;
    }
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.headtrainindex), sizeofint);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.trainNum), sizeofint);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.leavingTimes[0]), sizeofint);
    int dayoff = date - trainbasic.leavingTimes[0] / daytime;
    if (dayoff < 0 || dayoff >= trainbasic.trainNum) {
        std::cout << "-1\n";
        return;
    }
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.leavingTimes[1]), sizeofint * (maxStationNum - 1));
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.arrivingTimes[0]), sizeofint * maxStationNum);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.sum_prices[0]), sizeofint * maxStationNum);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.stationNum), sizeofint);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.seatNum), sizeofint);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.trainID), sizeoftrainID);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.stations[0]), sizeofstation * maxStationNum);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic.type), sizeofchar);
    int timeoff = dayoff * daytime;
    int last = trainbasic.stationNum - 1;
    if (trainbasic.state == 0) {
        std::cout << trainbasic.trainID << ' ' << trainbasic.type << '\n';
        std::cout << trainbasic.stations[0] << ' ' << "xx-xx xx:xx -> ";
        printTime(trainbasic.leavingTimes[0] + timeoff);
        std::cout << ' ' << trainbasic.sum_prices[0] << ' ' << trainbasic.seatNum << '\n';
        for (int i = 1; i < last; i++) {
            std::cout << trainbasic.stations[i] << ' ';
            printTime(trainbasic.arrivingTimes[i] + timeoff);
            std::cout << " -> ";
            printTime(trainbasic.leavingTimes[i] + timeoff);
            std::cout << ' ' << trainbasic.sum_prices[i] << ' ' << trainbasic.seatNum << '\n';
        }
        std::cout << trainbasic.stations[last] << ' ';
        printTime(trainbasic.arrivingTimes[last] + timeoff);
        std::cout << " -> xx-xx xx:xx " << trainbasic.sum_prices[last] << " x\n";
    } else if (trainbasic.state == 1) {
        int train_index = trainbasic.headtrainindex + dayoff;
        Train train;
        train_file.seekg(sizeofTrain * train_index);
        train_file.read(reinterpret_cast<char*>(&train), sizeofTrain);

        std::cout << trainbasic.trainID << ' ' << trainbasic.type << '\n';
        std::cout << trainbasic.stations[0] << ' ' << "xx-xx xx:xx -> ";
        printTime(trainbasic.leavingTimes[0] + timeoff);
        std::cout << ' ' << trainbasic.sum_prices[0] << ' ' << train.seats[0] << '\n';
        for (int i = 1; i < last; i++) {
            std::cout << trainbasic.stations[i] << ' ';
            printTime(trainbasic.arrivingTimes[i] + timeoff);
            std::cout << " -> ";
            printTime(trainbasic.leavingTimes[i] + timeoff);
            std::cout << ' ' << trainbasic.sum_prices[i] << ' ' << train.seats[i] << '\n';
        }
        std::cout << trainbasic.stations[last] << ' ';
        printTime(trainbasic.arrivingTimes[last] + timeoff);
        std::cout << " -> xx-xx xx:xx " << trainbasic.sum_prices[last] << " x\n";
    }
}

void TrainManager::queryTicket(sjtu::string<StationLength> &station1, sjtu::string<StationLength> &station2, int date, bool query_type) {
    bool (*compare)(const Ticket &lhs, const Ticket &rhs) = nullptr;
    if (!query_type) {
        compare = compareTicketTime;
    } else {
        compare = compareTicketCost;
    }

    sjtu::vector<sjtu::pair<int ,sjtu::pair<int, int>>> res = stationpair_bpt.find(sjtu::pair(station1, station2));
    int trainID_index, origin, destination, dayoff, train_index;
    TrainView trainview;
    Train train;
    Ticket ticket;
    sjtu::set<Ticket> tickets(compare);
    for (auto tmp : res) {
        trainID_index = tmp.first;
        origin = tmp.second.first;
        destination = tmp.second.second;
        trainID_file.seekg(sizeofTrainBasic * trainID_index);
        trainID_file.read(reinterpret_cast<char*>(&trainview), sizeofTrainView);
        dayoff = date - trainview.leavingTimes[origin] / daytime;
        if (dayoff < 0 || dayoff >= trainview.trainNum) continue;
        ticket.trainID = trainview.trainID;
        ticket.leavingtime = trainview.leavingTimes[origin] + dayoff * daytime;
        ticket.arrivingtime = trainview.arrivingTimes[destination] + dayoff * daytime;
        ticket.price = trainview.sum_prices[destination] - trainview.sum_prices[origin];
        ticket.seat = getSeat(trainview.headtrainindex + dayoff, origin, destination);
        tickets.insert(ticket);
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

void TrainManager::queryTransfer(sjtu::string<StationLength> &station1, sjtu::string<StationLength> &station2, int date, bool query_type) {
    bool (*compare)(const Transfer &lhs, const Transfer &rhs) = nullptr;
    if (!query_type) {
        compare = compareTransferTime;
    } else {
        compare = compareTransferCost;
    }

    sjtu::vector<sjtu::pair<int, int>> trainstations = station_bpt.find(station1);
    sjtu::vector<sjtu::pair<int, sjtu::pair<int, int>>> trainstationpairs;
    int trainID_index1, trainID_index2, origin, destination, dayoff1, dayoff2, transferstation1, transferstation2, num;
    TrainView trainview1, trainview2;
    Train train;
    Transfer transfer, ans;
    bool get = false;
    sjtu::string<StationLength> stations[maxStationNum];
    for (sjtu::pair<int, int> trainstation : trainstations) {
        trainID_index1 = trainstation.first;
        origin = trainstation.second;
        trainID_file.seekg(sizeofTrainBasic * trainID_index1);
        trainID_file.read(reinterpret_cast<char*>(&trainview1), sizeofTrainView);
        dayoff1 = date - trainview1.leavingTimes[origin] / daytime;
        if (dayoff1 < 0 || dayoff1 >= trainview1.trainNum) continue;
        transfer.train1ID = trainview1.trainID;
        transfer.leavingtime1 = trainview1.leavingTimes[origin] + dayoff1 * daytime;
        trainID_file.read(reinterpret_cast<char*>(&stations[0]), sizeofstation * maxStationNum);
        train_file.seekg(sizeofTrain * (trainview1.headtrainindex + dayoff1));
        train_file.read(reinterpret_cast<char*>(&train), sizeofTrain);
        transfer.seat1 = trainview1.seatNum;
        for (transferstation1 = origin + 1; transferstation1 < trainview1.stationNum; transferstation1++) {
            if (train.seats[transferstation1 - 1] < transfer.seat1) {
                transfer.seat1 = train.seats[transferstation1 - 1];
            }
            trainstationpairs = stationpair_bpt.find(sjtu::pair(stations[transferstation1], station2));
            for (sjtu::pair<int, sjtu::pair<int, int>> trainstationpair : trainstationpairs) {
                trainID_index2 = trainstationpair.first;
                transferstation2 = trainstationpair.second.first;
                destination = trainstationpair.second.second;
                if (trainID_index1 == trainID_index2) continue;
                trainID_file.seekg(sizeofTrainBasic * trainID_index2);
                trainID_file.read(reinterpret_cast<char*>(&trainview2), sizeofTrainView);
                // trainview1.arrivingTimes[transferstation1] + dayoff1 * daytime <= trainview2.leavingTimes[transferstation2] + dayoff2 * daytime
                num = trainview1.arrivingTimes[transferstation1] + dayoff1 * daytime - trainview2.leavingTimes[transferstation2];
                dayoff2 = num > 0 ? (num - 1) / daytime + 1 : 0;
                if (dayoff2 >= trainview2.trainNum) continue;
                transfer.train2ID = trainview2.trainID;
                transfer.transferstation = stations[transferstation1];
                transfer.arrivingtime1 = trainview1.arrivingTimes[transferstation1] + dayoff1 * daytime;
                transfer.price1 = trainview1.sum_prices[transferstation1] - trainview1.sum_prices[origin];
                transfer.leavingtime2 = trainview2.leavingTimes[transferstation2] + dayoff2 * daytime;
                transfer.arrivingtime2 = trainview2.arrivingTimes[destination] + dayoff2 * daytime;
                transfer.price2 = trainview2.sum_prices[destination] - trainview2.sum_prices[transferstation2];
                transfer.seat2 = getSeat(trainview2.headtrainindex + dayoff2, transferstation2, destination);
                if (!get) {
                    ans = transfer;
                    get = true;
                } else if (compare(transfer, ans)) {
                    ans = transfer;
                }
            }
        }
    }
    if (get) {
        std::cout << ans.train1ID << ' ' << station1 << ' ';
        printTime(ans.leavingtime1);
        std::cout << " -> " << ans.transferstation << ' ';
        printTime(ans.arrivingtime1);
        std::cout << ' ' << ans.price1 << ' ' << ans.seat1 << '\n';
        std::cout << ans.train2ID << ' ' << ans.transferstation << ' ';
        printTime(ans.leavingtime2);
        std::cout << " -> " << station2 << ' ';
        printTime(ans.arrivingtime2);
        std::cout << ' ' << ans.price2 << ' ' << ans.seat2 << '\n';
    } else {
        std::cout << "0\n";
    }
}

int TrainManager::getSeatNum(int trainID_index) {
    int seatNum;
    trainID_file.seekg(sizeofTrainBasic * trainID_index + (4 + 3 * maxStationNum) * sizeofint);
    trainID_file.read(reinterpret_cast<char*>(&seatNum), sizeofint);
    return seatNum;
}

TrainBasic TrainManager::getTrainBasic(int trainID_index) {
    TrainBasic trainbasic;
    trainID_file.seekg(sizeofTrainBasic * trainID_index);
    trainID_file.read(reinterpret_cast<char*>(&trainbasic), sizeofTrainBasic);
    return trainbasic;
}

Train TrainManager::getTrain(int train_index) {
    Train train;
    train_file.seekg(sizeofTrain * train_index);
    train_file.read(reinterpret_cast<char*>(&train), sizeofTrain);
    return train;
}

void TrainManager::writeTrain(int train_index, Train &train) {
    train_file.seekp(sizeofTrain * train_index);
    train_file.write(reinterpret_cast<char*>(&train), sizeofTrain);
}

void TrainManager::clean() {
    train_count = 0;
    trainID_count = 0;
    train_file.close();
    train_file.open(train_filename, std::ios::out | std::ios::trunc | std::ios::binary);
    train_file.close();
    train_file.open(train_filename, std::ios::in | std::ios::out | std::ios::binary);

    trainID_file.close();
    trainID_file.open(trainID_filename, std::ios::out | std::ios::trunc | std::ios::binary);
    trainID_file.close();
    trainID_file.open(trainID_filename, std::ios::in | std::ios::out | std::ios::binary);

    trainID_bpt.clear();
    station_bpt.clear();
    stationpair_bpt.clear();
}
