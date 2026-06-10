#pragma once

#include <string>

class Program;

class Parser {
  public:
    void parseLine(const std::string &line, Program *program);

  private:
    void parseAddUser       (int &pointer, const std::string &line, Program *program);
    void parseLogin         (int &pointer, const std::string &line, Program *program);
    void parseLogout        (int &pointer, const std::string &line, Program *program);
    void parseQueryProfile  (int &pointer, const std::string &line, Program *program);
    void parseModifyProfile (int &pointer, const std::string &line, Program *program);
    void parseAddTrain      (int &pointer, const std::string &line, Program *program);
    void parseDeleteTrain   (int &pointer, const std::string &line, Program *program);
    void parseReleaseTrain  (int &pointer, const std::string &line, Program *program);
    void parseQueryTrain    (int &pointer, const std::string &line, Program *program);
    void parseQueryTicket   (int &pointer, const std::string &line, Program *program);
    void parseQueryTransfer (int &pointer, const std::string &line, Program *program);
    void parseBuyTicket     (int &pointer, const std::string &line, Program *program);
    void parseQueryOrder    (int &pointer, const std::string &line, Program *program);
    void parseRefundTicket  (int &pointer, const std::string &line, Program *program);
    void parseClean         (int &pointer, const std::string &line, Program *program);
    void parseExit          (int &pointer, const std::string &line, Program *program);
};