#pragma once

#include <string>
#include <fstream>

#include "Parser.hpp"
#include "AccountManager.hpp"
#include "TrainManager.hpp"
#include "STLite/vector.hpp"
#include "STLite/pair.hpp"
#include "STLite/map.hpp"

class Program {
  private:
    std::string total_filename;
    std::fstream total_file;
    AccountManager accountmanager;
    TrainManager trainmanager;
    Parser parser;
    sjtu::map<std::string, int> loginrecorder;

    bool programEnd_;

  public:
    Program();
    ~Program();

    void execute(const std::string &line);

    void programEnd();
    bool programRun();

    void AddUser(const std::string &cur_username, const std::string &username, const std::string &password, const std::string &name, const std::string &mailAddr, int privilege);
    void Login(const std::string &username, const std::string &password);
    void Logout(const std::string &username);
    void QueryProfile(const std::string &sur_username, const std::string &username);
    void ModifyProfile(const std::string &cur_username, const std::string &username, const std::string &password, const std::string &name, const std::string &mailAddr, int privilege);

    void AddTrain(const std::string &trainID, int stationNum, int seatNum, sjtu::vector<std::string> &stations, sjtu::vector<int> &prices, int startTime, sjtu::vector<int> &travelTimes, sjtu::vector<int> &stopoverTimes, sjtu::pair<int, int> saleDate, char type);
    void DeleteTrain(const std::string &trainID);
    void ReleaseTrain(const std::string &trainID);

    void QueryTrain(const std::string &trainID, int date);
    void QueryTicket(const std::string &station1, const std::string station2, int date, bool query_type);
    void QueryTransfer(const std::string &station1, const std::string station2, int date, bool query_type);
    void BuyTicket(const std::string &username, const std::string &trainID, int date, int ticketnum, std::string &station1, std::string &station2, bool buy_type);
    void QueryOrder(const std::string &username);

    void RefundTicket(const std::string &username, int ticketnum);

    void Clean();
    void Exit();
};