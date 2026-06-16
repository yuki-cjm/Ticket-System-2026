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
    int stations[100], sum_prices[100], seats[99], arrivingTimes[100], leavingTimes[100];
    char type;
    int state; // 0->unreleased, 1->released, 2->deleted

    Train() = default;
};

struct StationDate {
    int station;
    int date;

    StationDate() = default;
    StationDate(int station, int date) : station(station), date(date) {}
    bool operator<(const StationDate &o) const { return station < o.station || date < o.date && station == o.station && date != -1 && o.date != -1; }
    bool operator>(const StationDate &o) const { return station > o.station || date > o.date && station == o.station && date != -1 && o.date != -1; }
    bool operator==(const StationDate &o) const { return station == o.station && (date == o.date || date == -1 || o.date == -1); }
};

struct StationInformation {
    int seat, price, arrivingtime;

    StationInformation() = default;
    StationInformation(int seat, int price, int arrivingtime) : seat(seat), price(price), arrivingtime(arrivingtime) {}
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


    void writeTrainID(int index, sjtu::string<20> &trainID);

    void writeStation(int index, sjtu::string<30> &station);
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
    int findStationIndex(sjtu::string<30> &station);
    sjtu::string<20> getTrainID(int index);
    sjtu::string<30> getStation(int index);
    sjtu::vector<sjtu::pair<int, int>> getStationDateTrainStations(StationDate stationdate);

    void addTrain(sjtu::string<20> &trainID, int stationNum, int seatNum, sjtu::string<30> *stations, int *prices, int startTime, int *travelTimes, int *stopoverTimes, sjtu::pair<int, int> saleDate, char type);

    void deleteTrain(sjtu::string<20> &trainID, sjtu::vector<int> &indexs);
    void releaseTrain(sjtu::vector<int> &indexs);

    void clean();
};