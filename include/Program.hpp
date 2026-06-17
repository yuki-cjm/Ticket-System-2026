#pragma once

#include <string>
#include <fstream>

#include "Parser.hpp"
#include "AccountManager.hpp"
#include "TrainManager.hpp"
#include "OrderManager.hpp"
#include "STLite/vector.hpp"
#include "STLite/pair.hpp"
#include "STLite/map.hpp"
#include "STLite/string.hpp"
#include "STLite/deque.hpp"
#include "utils/constants.hpp"

class Program {
  private:
    std::string total_filename;
    std::fstream total_file;
    AccountManager accountmanager;
    TrainManager trainmanager;
    OrderManager ordermanager;
    Parser parser;
    sjtu::map<sjtu::string<UserNameLength>, int> loginrecorder;
    sjtu::deque<int> pendingorders;

    bool programEnd_;

  public:
    Program();
    ~Program();

    void execute(const std::string &line);

    void programEnd();
    bool programRun();

    void AddUser(sjtu::string<UserNameLength> &cur_username, sjtu::string<UserNameLength> &username, sjtu::string<PassWordLength> &password, sjtu::string<NameLength> &name, sjtu::string<MailAddrLength> &mailAddr, int privilege);
    void Login(const sjtu::string<UserNameLength> &username, const sjtu::string<PassWordLength> &password);
    void Logout(const sjtu::string<UserNameLength> &username);
    void QueryProfile(const sjtu::string<UserNameLength> &cur_username, const sjtu::string<UserNameLength> &username);
    void ModifyProfile(const sjtu::string<UserNameLength> &cur_username, const sjtu::string<UserNameLength> &username, const sjtu::string<PassWordLength> &password, const sjtu::string<NameLength> &name, const sjtu::string<MailAddrLength> &mailAddr, int privilege);

    void AddTrain(sjtu::string<TrainIDLength> &trainID, int stationNum, int seatNum, sjtu::string<StationLength> *stations, int *prices, int startTime, int *travelTimes, int *stopoverTimes, sjtu::pair<int, int> saleDate, char type);

    void DeleteTrain(sjtu::string<TrainIDLength> &trainID);
    void ReleaseTrain(sjtu::string<TrainIDLength> &trainID);

    void QueryTrain(const sjtu::string<TrainIDLength> &trainID, int date);
    void QueryTicket(sjtu::string<StationLength> &station1, sjtu::string<StationLength> &station2, int date, bool query_type);
    void QueryTransfer(sjtu::string<StationLength> &station1, sjtu::string<StationLength> station2, int date, bool query_type);
    void BuyTicket(const sjtu::string<UserNameLength> &username, sjtu::string<TrainIDLength> &trainID, int date, int ticketnum, sjtu::string<StationLength> &station1, sjtu::string<StationLength> &station2, bool buy_type);
    void QueryOrder(const sjtu::string<UserNameLength> &username);

    void RefundTicket(const sjtu::string<UserNameLength> &username, int ticketnum);

    void Clean();
    void Exit();
};