#pragma once

#include <string>
#include <fstream>

#include "STLite/vector.hpp"
#include "STLite/BPT.hpp"
#include "STLite/pair.hpp"
#include "STLite/string.hpp"
#include "utils/constants.hpp"

struct TrainBasic {
    int state; // 0->unreleased, 1->released, 2->deleted
    int headtrainindex, trainNum;
    int leavingTimes[maxStationNum], arrivingTimes[maxStationNum], sum_prices[maxStationNum];
    int stationNum, seatNum;
    sjtu::string<TrainIDLength> trainID;
    sjtu::string<StationLength> stations[maxStationNum];
    char type;

    TrainBasic() = default;
    ~TrainBasic() = default;
};

struct TrainView {
    int state; // 0->unreleased, 1->released, 2->deleted
    int headtrainindex, trainNum;
    int leavingTimes[maxStationNum], arrivingTimes[maxStationNum], sum_prices[maxStationNum];
    int stationNum, seatNum;
    sjtu::string<TrainIDLength> trainID;
};

struct Train {
    int seats[maxStationNum - 1];
};

struct StationInformation {
    int seat, price, arrivingtime;

    StationInformation() = default;
    StationInformation(int seat, int price, int arrivingtime) : seat(seat), price(price), arrivingtime(arrivingtime) {}
};

struct Ticket {
    sjtu::string<TrainIDLength> trainID;
    int leavingtime, arrivingtime, price, seat;

    Ticket() = default;
};

struct Transfer {
    sjtu::string<TrainIDLength> train1ID, train2ID;
    sjtu::string<StationLength> transferstation;
    int leavingtime1, arrivingtime1, price1, seat1;
    int leavingtime2, arrivingtime2, price2, seat2;

    Transfer() = default;
};



class TrainManager {
  private:
    int trainID_count, train_count;
    std::string train_filename, trainID_filename;
    std::fstream train_file, trainID_file;
    BplusTree<sjtu::string<TrainIDLength>, int> trainID_bpt;
    BplusTree<sjtu::string<StationLength>, sjtu::pair<int, int>> station_bpt;
    BplusTree<sjtu::pair<sjtu::string<StationLength>, sjtu::string<StationLength>>, sjtu::pair<int, sjtu::pair<int, int>>> stationpair_bpt;

    static const int sizeofTrainBasic = sizeofint * (5 + 3 * maxStationNum) + sizeofchar + sizeoftrainID + maxStationNum * sizeofstation;
    static const int sizeofTrain = sizeofint * (maxStationNum - 1);
    static const int sizeofTrainView = sizeofint * (5 + 3 * maxStationNum) + sizeoftrainID;

    static bool compareTicketTime(const Ticket &lhs, const Ticket &rhs);
    static bool compareTicketCost(const Ticket &lhs, const Ticket &rhs);
    static bool compareTransferTime(const Transfer &lhs, const Transfer &rhs);
    static bool compareTransferCost(const Transfer &lhs, const Transfer &rhs);

    int getSeat(int train_index, int origin, int destination);
  public:
    TrainManager();
    ~TrainManager();
    void changeTrainIDCount(int count);
    int getTrainIDCount();
    void changeTrainCount(int count);
    int getTrainCount();

    int getTrainIDIndex(const sjtu::string<TrainIDLength> &trainID);
    void addTrain(sjtu::string<TrainIDLength> &trainID, int stationNum, int seatNum, sjtu::string<StationLength> *stations, int *prices, int startTime, int *travelTimes, int *stopoverTimes, sjtu::pair<int, int> saleDate, char type);
    int getTrainState(int index);
    void deleteTrain(sjtu::string<TrainIDLength> &trainID, int index);
    void releaseTrain(int index);
    void queryTrain(int trainID_index, int date);
    void queryTicket(sjtu::string<StationLength> &station1, sjtu::string<StationLength> &station2, int date, bool query_type);
    void queryTransfer(sjtu::string<StationLength> &station1, sjtu::string<StationLength> &station2, int date, bool query_type);
    int getSeatNum(int trainID_index);
    TrainBasic getTrainBasic(int trainID_index);
    Train getTrain(int train_index);
    void writeTrain(int train_index, Train &train);

    void clean();
};