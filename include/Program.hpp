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

class Program {
  private:
    std::string total_filename;
    std::fstream total_file;
    AccountManager accountmanager;
    TrainManager trainmanager;
    OrderManager ordermanager;
    Parser parser;
    sjtu::map<sjtu::string<20>, int> loginrecorder;
    sjtu::deque<int> pendingorders;

    bool programEnd_;

  public:
    Program();
    ~Program();

    void execute(const std::string &line);

    void programEnd();
    bool programRun();

    void AddUser(sjtu::string<20> &cur_username, sjtu::string<20> &username, sjtu::string<30> &password, sjtu::string<15> &name, sjtu::string<30> &mailAddr, int privilege);
    void Login(const sjtu::string<20> &username, const sjtu::string<30> &password);
    void Logout(const sjtu::string<20> &username);
    void QueryProfile(const sjtu::string<20> &cur_username, const sjtu::string<20> &username);
    void ModifyProfile(const sjtu::string<20> &cur_username, const sjtu::string<20> &username, const sjtu::string<30> &password, const sjtu::string<15> &name, const sjtu::string<30> &mailAddr, int privilege);

    void AddTrain(sjtu::string<20> &trainID, int stationNum, int seatNum, sjtu::string<30> *stations, int *prices, int startTime, int *travelTimes, int *stopoverTimes, sjtu::pair<int, int> saleDate, char type);

    void DeleteTrain(sjtu::string<20> &trainID);
    void ReleaseTrain(sjtu::string<20> &trainID);

    void QueryTrain(const sjtu::string<20> &trainID, int date);
    void QueryTicket(sjtu::string<30> &station1, sjtu::string<30> &station2, int date, bool query_type);
    void QueryTransfer(sjtu::string<30> &station1, sjtu::string<30> station2, int date, bool query_type);
    void BuyTicket(const sjtu::string<20> &username, sjtu::string<20> &trainID, int date, int ticketnum, sjtu::string<30> &station1, sjtu::string<30> &station2, bool buy_type);
    void QueryOrder(const sjtu::string<20> &username);

    void RefundTicket(const sjtu::string<20> &username, int ticketnum);

    void Clean();
    void Exit();
};