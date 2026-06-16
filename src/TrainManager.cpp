#include <fstream>
#include <cstring>

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
constexpr int stateoffset = sizeoftrainID + sizeofint + sizeofint + sizeofchar;

int culculateTime(int date, int time) {
    return date * daytime + time;
}

void TrainManager::writeTrain(int index, Train &train) {
    static char buffer[sizeofTrain];
    int offset = 0;
    std::memcpy(buffer + offset, &train.trainID, sizeoftrainID);
    offset += sizeoftrainID;
    std::memcpy(buffer + offset, &train.stationNum, sizeofint);
    offset += sizeofint;
    std::memcpy(buffer + offset, &train.seatNum, sizeofint);
    offset += sizeofint;
    std::memcpy(buffer + offset, &train.type, sizeofchar);
    offset += sizeofchar;
    std::memcpy(buffer + offset, &train.state, sizeofint);
    offset += sizeofint;
    std::memcpy(buffer + offset, train.stations, train.stationNum * sizeofint);
    offset += train.stationNum * sizeofint;
    std::memcpy(buffer + offset, train.sum_prices, train.stationNum * sizeofint);
    offset += train.stationNum * sizeofint;
    std::memcpy(buffer + offset, train.seats, (train.stationNum - 1) * sizeofint);
    offset += (train.stationNum - 1) * sizeofint;
    std::memcpy(buffer + offset, train.arrivingTimes, train.stationNum * sizeofint);
    offset += train.stationNum * sizeofint;
    std::memcpy(buffer + offset, train.leavingTimes, train.stationNum * sizeofint);
    offset += train.stationNum * sizeofint;
    train_file.seekp(sizeofTrain * index);
    train_file.write(buffer, sizeofTrain);
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

void TrainManager::addTrain(sjtu::string<20> &trainID, int stationNum, int seatNum, sjtu::string<30> *stations, int *prices, int startTime, int *travelTimes, int *stopoverTimes, sjtu::pair<int, int> saleDate, char type) {

    Train train;
    train.trainID = trainID;
    train.stationNum = stationNum;
    train.seatNum = seatNum;
    train.type = type;
    train.state = 0;
    for (int i = 0; i < stationNum; i++) {
        train.stations[i] = getStationIndex(stations[i]);
    }
    for (int i = 0; i < stationNum - 1; i++) {
        train.seats[i] = seatNum;
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
    writeTrain(train_count, train);
    writeTrainID(train_count, trainID);
    train_bpt.insert(trainID, train_count);
    train_count++;
    for (int i = saleDate.first + 1; i <= saleDate.second; i++) {
        for (int j = 0; j < stationNum; j++) {
            train.arrivingTimes[j] += daytime;
            train.leavingTimes[j] += daytime;
        }
        train.arrivingTimes[0] = -1;
        train.leavingTimes[stationNum - 1] = -1;
        writeTrain(train_count, train);
        writeTrainID(train_count, trainID);
        train_bpt.insert(trainID, train_count);
        train_count++;
    }
}


Train TrainManager::getTrain(int index) {
    static char buffer[sizeofTrain];
    train_file.seekg(sizeofTrain * index);
    train_file.read(buffer, sizeofTrain);
    Train train;
    int offset = 0;
    std::memcpy(&train.trainID, buffer + offset, sizeoftrainID);
    offset += sizeoftrainID;
    std::memcpy(&train.stationNum, buffer + offset, sizeofint);
    offset += sizeofint;
    std::memcpy(&train.seatNum, buffer + offset, sizeofint);
    offset += sizeofint;
    std::memcpy(&train.type, buffer + offset, sizeofchar);
    offset += sizeofchar;
    std::memcpy(&train.state, buffer + offset, sizeofint);
    offset += sizeofint;
    std::memcpy(train.stations, buffer + offset, train.stationNum * sizeofint);
    offset += train.stationNum * sizeofint;
    std::memcpy(train.sum_prices, buffer + offset, train.stationNum * sizeofint);
    offset += train.stationNum * sizeofint;
    std::memcpy(train.seats, buffer + offset, (train.stationNum - 1) * sizeofint);
    offset += (train.stationNum - 1) * sizeofint;
    std::memcpy(train.arrivingTimes, buffer + offset, train.stationNum * sizeofint);
    offset += train.stationNum * sizeofint;
    std::memcpy(train.leavingTimes, buffer + offset, train.stationNum * sizeofint);
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
        train_file.seekp(sizeofTrain * index + stateoffset);
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
        train_file.seekp(sizeofTrain * indexs[i] + stateoffset);
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
