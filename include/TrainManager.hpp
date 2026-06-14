#pragma once

#include <string>
#include <fstream>

#include "STLite/vector.hpp"
#include "STLite/BPT.hpp"
#include "STLite/pair.hpp"
#include "STLite/string.hpp"

struct Train {
    sjtu::string<20> trainID;
    int stationNum, seatNum;
    sjtu::vector<int> stations, sum_prices, seats, arrivingTimes, leavingTimes;
    char type;
    int state; // 0->unreleased, 1->released, 2->deleted

    Train() = default;
};

struct StationDate {
    int station;
    int date;

    StationDate() = default;
    StationDate(int station, int date) : station(station), date(date) {}
    bool operator<(const StationDate &o) const { return station < o.station || date < o.date && station == o.station; }
    bool operator>(const StationDate &o) const { return station > o.station || date > o.date && station == o.station; }
    bool operator==(const StationDate &o) const { return station == o.station && date == o.date; }
};

class TrainManager {
  private:
    int train_count, station_count;
    std::string train_filename, trainID_filename, station_filename;
    std::fstream train_file, trainID_file, station_file;
    BplusTree<sjtu::string<20>, int> train_bpt;
    BplusTree<sjtu::string<30>, int> station_bpt;
    BplusTree<StationDate, sjtu::pair<int, int>> stationdate_bpt;
    static const int sizeofTrain = 2009 + sizeof(sjtu::string<20>); // 4 + 1 + 4 + 4 + 4 * （100 + 100 + 99 + 100 + 100）= 2005

    void writeTrain(int index, sjtu::string<20> &trainID, int stationNum, int seatNum, char type, int state, sjtu::vector<int> &stations, sjtu::vector<int> &sum_prices, sjtu::vector<int> &seats, sjtu::vector<int> &arrivingTimes, sjtu::vector<int> leavingTimes);
    void writeTrainID(int index, sjtu::string<20> &trainID);
    void writeStation(int index, sjtu::string<30> &station);
// index-订单信息: trainindex + startstationindex + endstationindex + seatnum + orderstate(success, pengding, refunded)
// 候补购票队列可用deque记录orderindex
  public:
    TrainManager();
    ~TrainManager();
    void changeTrainCount(int count);
    int getTrainCount();
    void changeStationCount(int count);
    int getStationCount();

    void writeTrain(int index, Train &train);
    sjtu::vector<int> getTrainIndexs(const sjtu::string<20> &trainID);
    Train getTrain(int index);
    int getStationIndex(sjtu::string<30> &station);
    sjtu::string<20> getTrainID(int index);
    sjtu::string<30> getStation(int index);
    sjtu::vector<sjtu::pair<int, int>> getStationDateTrainStations(StationDate &stationdate);

    void addTrain(sjtu::string<20> &trainID, int stationNum, int seatNum, sjtu::vector<sjtu::string<30>> &stations, sjtu::vector<int> &prices, int startTime, sjtu::vector<int> &travelTimes, sjtu::vector<int> &stopoverTimes, sjtu::pair<int, int> saleDate, char type);
    void deleteTrain(sjtu::string<20> &trainID, sjtu::vector<int> &indexs);
    void releaseTrain(sjtu::vector<int> &indexs);

    void clean();
};