#include <fstream>

#include "TrainManager.hpp"
#include "STLite/vector.hpp"
#include "STLite/BPT.hpp"
#include "STLite/pair.hpp"
#include "STLite/string.hpp"

constexpr int daytime = 1440;
constexpr int sizeoftrainID = sizeof(sjtu::string<20>);
constexpr int sizeofint = sizeof(int);
constexpr int sizeofchar = sizeof(char);
constexpr int sizeofstation = sizeof(sjtu::string<30>);

int culculateTime(int date, int time) {
    return date * daytime + time;
}

void TrainManager::writeTrain(int index, Train &train) {
    writeTrain(index, train.trainID, train.stationNum, train.seatNum, train.type, train.state, train.stations, train.sum_prices, train.seats, train.arrivingTimes, train.leavingTimes);
}

void TrainManager::writeTrain(int index, sjtu::string<20> &trainID, int stationNum, int seatNum, char type, int state, sjtu::vector<int> &stations, sjtu::vector<int> &sum_prices, sjtu::vector<int> &seats, sjtu::vector<int> &arrivingTimes, sjtu::vector<int> leavingTimes) {
    train_file.seekp(sizeofTrain * index);
    train_file.write(reinterpret_cast<char*>(&trainID), sizeoftrainID);
    train_file.write(reinterpret_cast<char*>(&stationNum), sizeofint);
    train_file.write(reinterpret_cast<char*>(&seatNum), sizeofint);
    train_file.write(reinterpret_cast<char*>(&type), sizeofchar);
    train_file.write(reinterpret_cast<char*>(&state), sizeofint);
    for (auto it = stations.begin(); it != stations.end(); it++) {
        train_file.write(reinterpret_cast<char*>(&(*it)), sizeofint);
    }
    for (auto it = sum_prices.begin(); it != sum_prices.end(); it++) {
        train_file.write(reinterpret_cast<char*>(&(*it)), sizeofint);
    }
    for (auto it = seats.begin(); it != seats.end(); it++) {
        train_file.write(reinterpret_cast<char*>(&(*it)), sizeofint);
    }
    for (auto it = arrivingTimes.begin(); it != arrivingTimes.end(); it++) {
        train_file.write(reinterpret_cast<char*>(&(*it)), sizeofint);
    }
    for (auto it = leavingTimes.begin(); it != leavingTimes.end(); it++) {
        train_file.write(reinterpret_cast<char*>(&(*it)), sizeofint);
    }
}

void TrainManager::writeTrainID(int index, sjtu::string<20> &trainID) {
    trainID_file.seekp(index * sizeoftrainID);
    trainID_file.write(reinterpret_cast<char*>(&trainID), sizeoftrainID);
}

void TrainManager::writeStation(int index, sjtu::string<30> &station) {
    station_file.seekp(index * sizeofstation);
    station_file.write(reinterpret_cast<char*>(&station), sizeofstation);
}

TrainManager::TrainManager() : train_filename("traindata"), trainID_filename("trainIDdata"), station_filename("stationdata"), train_bpt("trainbasic", "trainindex"), station_bpt("stationbasic", "stationindex"), stationdate_bpt("stationdatebasic", "stationdateindex") {
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

    station_file.open(station_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!station_file) {
        station_file.open(station_filename, std::ios::out | std::ios::binary);
        station_file.close();
        station_file.open(station_filename, std::ios::in | std::ios::out | std::ios::binary);
    }
}

TrainManager::~TrainManager() {
    train_file.close();
    station_file.close();
}

void TrainManager::changeTrainCount(int count) {
    train_count = count;
}


int TrainManager::getTrainCount() {
    return train_count;
}

void TrainManager::changeStationCount(int count) {
    station_count = count;
}


int TrainManager::getStationCount() {
    return station_count;
}

sjtu::vector<int> TrainManager::getTrainIndexs(const sjtu::string<20> &trainID) {
    return train_bpt.find(trainID);
}

int TrainManager::getStationIndex(sjtu::string<30> &station) {
    sjtu::vector<int> indexs = station_bpt.find(station);
    if (indexs.empty()) {
        station_bpt.insert(station, station_count);
        writeStation(station_count, station);
        return station_count++;
    }
    return indexs[0];
}

int TrainManager::findStationIndex(sjtu::string<30> &station) {
    sjtu::vector<int> indexs = station_bpt.find(station);
    if (indexs.empty()) {
        return -1;
    }
    return indexs[0];
}

void TrainManager::addTrain(sjtu::string<20> &trainID, int stationNum, int seatNum, sjtu::vector<sjtu::string<30>> &stations, sjtu::vector<int> &prices, int startTime, sjtu::vector<int> &travelTimes, sjtu::vector<int> &stopoverTimes, sjtu::pair<int, int> saleDate, char type) {
    sjtu::vector<int> arrivingtimes, leavingtimes;
    sjtu::vector<int> seats(stationNum - 1, seatNum);
    sjtu::vector<int> station_indexs;
    sjtu::vector<int> sum_prices;
    for (auto station : stations) {
        station_indexs.push_back(getStationIndex(station));
    }
    int sum_time = saleDate.first * daytime + startTime;
    int sum_price = 0;
    arrivingtimes.push_back(-1);
    leavingtimes.push_back(sum_time);
    sum_prices.push_back(sum_price);
    for (int i = 0; i < stationNum - 2; i++) {
        sum_time += travelTimes[i];
        arrivingtimes.push_back(sum_time);
        sum_time += stopoverTimes[i];
        leavingtimes.push_back(sum_time);
        sum_price += prices[i];
        sum_prices.push_back(sum_price);
    }
    arrivingtimes.push_back(sum_time + travelTimes[stationNum - 2]);
    leavingtimes.push_back(-1);
    sum_prices.push_back(sum_price + prices[stationNum - 2]);
    writeTrain(train_count, trainID, stationNum, seatNum, type, 0, station_indexs, sum_prices, seats, arrivingtimes, leavingtimes);
    writeTrainID(train_count, trainID);
    train_bpt.insert(trainID, train_count);
    train_count++;
    for (int i = saleDate.first + 1; i <= saleDate.second; i++) {
        for (int j = 0; j < stationNum; j++) {
            arrivingtimes[j] += daytime;
            leavingtimes[j] += daytime;
        }
        arrivingtimes[0] = -1;
        leavingtimes[stationNum - 1] = -1;
        writeTrain(train_count, trainID, stationNum, seatNum, type, 0, station_indexs, sum_prices, seats, arrivingtimes, leavingtimes);
        writeTrainID(train_count, trainID);
        train_bpt.insert(trainID, train_count);
        train_count++;
    }
}

Train TrainManager::getTrain(int index) {
    Train train;
    int temp;
    train_file.seekg(sizeofTrain * index);
    train_file.read(reinterpret_cast<char*>(&train.trainID), sizeoftrainID);
    train_file.read(reinterpret_cast<char*>(&train.stationNum), sizeofint);
    train_file.read(reinterpret_cast<char*>(&train.seatNum), sizeofint);
    train_file.read(reinterpret_cast<char*>(&train.type), sizeofchar);
    train_file.read(reinterpret_cast<char*>(&train.state), sizeofint);
    for (int i = 0; i < train.stationNum; i++) {
        train_file.read(reinterpret_cast<char*>(&temp), sizeofint);
        train.stations.push_back(temp);
    }
    for (int i = 0; i < train.stationNum; i++) {
        train_file.read(reinterpret_cast<char*>(&temp), sizeofint);
        train.sum_prices.push_back(temp);
    }
    for (int i = 0; i < train.stationNum - 1; i++) {
        train_file.read(reinterpret_cast<char*>(&temp), sizeofint);
        train.seats.push_back(temp);
    }
    for (int i = 0; i < train.stationNum; i++) {
        train_file.read(reinterpret_cast<char*>(&temp), sizeofint);
        train.arrivingTimes.push_back(temp);
    }
    for (int i = 0; i < train.stationNum; i++) {
        train_file.read(reinterpret_cast<char*>(&temp), sizeofint);
        train.leavingTimes.push_back(temp);
    }
    return train;
}

sjtu::string<20> TrainManager::getTrainID(int index) {
    trainID_file.seekg(index * sizeoftrainID);
    sjtu::string<20> trainID;
    trainID_file.read(reinterpret_cast<char*>(&trainID), sizeoftrainID);
    return trainID;
}

void TrainManager::deleteTrain(sjtu::string<20> &trainID, sjtu::vector<int> &indexs) {
    int state = 2;
    for (int index : indexs) {
        train_file.seekp(sizeofTrain * index + sizeoftrainID + sizeofint + sizeofint + sizeofchar);
        train_file.write(reinterpret_cast<char*>(&state), sizeofint);
        train_bpt.remove(trainID, index);
    }
}

void TrainManager::releaseTrain(sjtu::vector<int> &indexs) {
    if (indexs.empty()) {
        return;
    }
    int state = 1;
    Train train = getTrain(indexs[0]);
    StationDate stationdate;
    for (int i = 0; i < indexs.size(); i++) {
        train_file.seekp(sizeofTrain * indexs[i] + sizeoftrainID + sizeofint + sizeofint + sizeofchar);
        train_file.write(reinterpret_cast<char*>(&state), sizeofint);
        for (int j = 0; j < train.stationNum - 1; j++) {
            stationdate.station = train.stations[j];
            stationdate.date = train.leavingTimes[j] / daytime + i;
            stationdate_bpt.insert(stationdate, sjtu::pair(indexs[i], j));
            stationdate.date = -1;
            stationdate_bpt.insert(stationdate, sjtu::pair(indexs[i], j));
        }
        stationdate.station = train.stations[train.stationNum - 1];
        stationdate.date = -1;
        stationdate_bpt.insert(stationdate, sjtu::pair(indexs[i], train.stationNum - 1));

    }
}

sjtu::string<30> TrainManager::getStation(int index) {
    station_file.seekg(sizeofstation * index);
    sjtu::string<30> station;
    station_file.read(reinterpret_cast<char*>(&station), sizeofstation);
    return station;
}

sjtu::vector<sjtu::pair<int, int>> TrainManager::getStationDateTrainStations(StationDate stationdate) {
    return stationdate_bpt.find(stationdate);
}

void TrainManager::clean() {
    train_count = 0;
    station_count = 0;
    train_file.close();
    train_file.open(train_filename, std::ios::out | std::ios::trunc | std::ios::binary);
    train_file.close();
    train_file.open(train_filename, std::ios::in | std::ios::out | std::ios::binary);

    trainID_file.close();
    trainID_file.open(trainID_filename, std::ios::out | std::ios::trunc | std::ios::binary);
    trainID_file.close();
    trainID_file.open(trainID_filename, std::ios::in | std::ios::out | std::ios::binary);
    station_file.close();
    station_file.open(station_filename, std::ios::out | std::ios::trunc | std::ios::binary);
    station_file.close();
    station_file.open(station_filename, std::ios::in | std::ios::out | std::ios::binary);
    train_bpt.clear();
    station_bpt.clear();
    stationdate_bpt.clear();
}