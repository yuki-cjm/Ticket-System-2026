#pragma once

#include <string>
#include <fstream>

#include "STLite/vector.hpp"
#include "STLite/BPT.hpp"
#include "STLite/pair.hpp"
#include "STLite/string.hpp"

struct Train {
    int stationNum, seatNum, startTime;
    sjtu::vector<int> stations;
    sjtu::vector<int> prices, seats, arrivingTimes, leavingTimes;
    sjtu::pair<int, int> saleDate;
    bool type;
    int state; // 0->unreleased, 1->released, 2->deleted
};

struct StationDate {
    int station;
    int date;

    bool operator<(const StationDate &o) { return station < o.station || date < o.date && station == o.station; }
    bool operator>(const StationDate &o) { return station > o.station || date > o.date && station == o.station; }
    bool operator==(const StationDate &o) { return station == o.station && date == o.date; }
};

class TrainManager {
  private:
    int train_count;
    std::string train_filename;
    std::fstream file;
    BplusTree<sjtu::string<20>, int> train_bpt;
    BplusTree<sjtu::string<30>, int> station_bpt;
    BplusTree<StationDate, int> stationdate_bpt;
// 未发布的train一个文件，已发布的train一个文件（每天都有不同的index的train）
// 站点BPT(int 为data * 1000 + stationindex)
// OrderManager
// BPT记录username-orderindexs
// index-订单信息: trainindex + startstationindex + endstationindex + seatnum + orderstate(success, pengding, refunded)
// 候补购票队列可用deque记录orderindex
  public:
    int getTrainIndex(const sjtu::string<20> &trainID);
};