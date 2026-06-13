#pragma once

#include <string>
#include <fstream>

#include "STLite/vector.hpp"
#include "STLite/BPT.hpp"
#include "STLite/pair.hpp"
#include "STLite/string.hpp"

struct Train {
    int stationNum;
    sjtu::vector<int> stations, sum_prices, seats, arrivingTimes, leavingTimes;
    char type;
    int state; // 0->unreleased, 1->released, 2->deleted
};

struct StationDate {
    int station;
    int date;

    bool operator<(const StationDate &o) const { return station < o.station || date < o.date && station == o.station; }
    bool operator>(const StationDate &o) const { return station > o.station || date > o.date && station == o.station; }
    bool operator==(const StationDate &o) const { return station == o.station && date == o.date; }
};

class TrainManager {
  private:
    int train_count, station_count;
    std::string train_filename, station_filename;
    std::fstream train_file, station_file;
    BplusTree<sjtu::string<20>, int> train_bpt;
    BplusTree<sjtu::string<30>, int> station_bpt;
    BplusTree<StationDate, int> stationdate_bpt;
    static const int sizeofTrain = 2001; // 4 + 1 + 4 + 4 * （100 + 99 + 99 + 100 + 100）= 2001

    void writeTrain(int index, Train &train);
    void writeTrain(int index, int stationNum, char type, int state, sjtu::vector<int> &stations, sjtu::vector<int> &sum_prices, sjtu::vector<int> &seats, sjtu::vector<int> &arrivingTimes, sjtu::vector<int> leavingTimes);
    void writeStation(int index, sjtu::string<30> &station);
// 未发布的train一个文件，已发布的train一个文件（每天都有不同的index的train）
// 站点BPT(int 为data * 1000 + stationindex)
// OrderManager
// BPT记录username-orderindexs
// index-订单信息: trainindex + startstationindex + endstationindex + seatnum + orderstate(success, pengding, refunded)
// 候补购票队列可用deque记录orderindex
  public:
    TrainManager();
    ~TrainManager();
    void changeTrainCount(int count);
    int getTrainCount();
    void changeStationCount(int count);
    int getStationCount();

    sjtu::vector<int> getTrainIndex(const sjtu::string<20> &trainID);
    int getStationIndex(sjtu::string<30> &station);
    void addTrain(const sjtu::string<20> &trainID, int stationNum, int seatNum, sjtu::vector<sjtu::string<30>> &stations, sjtu::vector<int> &prices, int startTime, sjtu::vector<int> &travelTimes, sjtu::vector<int> &stopoverTimes, sjtu::pair<int, int> saleDate, char type);
    Train getTrain(int index);
    void deleteTrain(sjtu::vector<int> &indexs);
    void releaseTrain(sjtu::vector<int> &indexs);
    sjtu::string<30> getStation(int index);
};