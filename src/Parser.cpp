#include <iostream>
#include <string>

#include "utils/Error.hpp"
#include "STLite/vector.hpp"
#include "Parser.hpp"
#include "Program.hpp"

class Program;

enum wordType {
    DEFAULT,
    USERNAME,
    PASSWORD,
    NAME,
    MAILADDR,
    PRIVILEGE,
    TRAINID,
    STATIONNUM,
    STATIONS,
    SEATNUM,
    PRICES,
    STARTTIME,
    TRAVELTIMES,
    STOPOVERTIMES,
    SALEDATE,
    TYPE,
    DATE,
    STATION
};


bool isvisibleletter(char &c)
{
    return !(c <= 32 || c == 127);
}

bool isnum(char c) {
    return c >= '0' && c <= '9';
}

bool isletter(char c) {
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';
}

// stations
sjtu::vector<std::string> getStations(const std::string &keywords)
{
    sjtu::vector<std::string> answer;
    std::string str{};
    for(int i = 0; i < keywords.length(); i++)
        if(keywords[i] == '|')
        {
            if(str.empty() || str.length() % 3 != 0 || str.length() > 30)
                throw invalid_input();
            answer.push_back(str);
            str = "";
        } else
        {
            str = str + keywords[i];
        }
    answer.push_back(str);
    return answer;
}

// prices, travelTimes, stopocerTimes
sjtu::vector<int> getNums(const std::string &keywords, wordType type) {
    sjtu::vector<int> answer;
    if (type == STOPOVERTIMES && keywords.length() == 1 && keywords[0] == '_') {
        return answer;
    }
    int num{0};
    for (int i = 0; i < keywords.length(); i++) {
        if (keywords[i] == '|') {
            if (!(type == PRICES && num > 1e5 || type == TRAVELTIMES && num > 1e4 || type == STOPOVERTIMES && num > 1e4))
                answer.push_back(num);
            num = 0;
        } else {
            num = num * 10 + (keywords[i] - '0');
        }
    }
    answer.push_back(num);
    return answer;
}

// startTime
int getStartTime(const std::string &str) {
    if (str.length() != 5 || !isnum(str[0]) || !isnum(str[1]) || str[2] != ':' || !isnum(str[3]) || !isnum(str[4])) {
        throw invalid_input();
    }
    int ans = ((str[0] - '0') * 10 + (str[1] - '0')) * 60 + ((str[3] - '0') * 10 + (str[4] - '0'));
    if (ans >= 1440) {
        throw invalid_input();
    } else {
        return ans;
    }
}

bool judgeDateCorrectness(int month, int day) {
    if (month < 6 || month > 8) {
        return false;
    }
    if (day == 0) {
        return false;
    }
    if (month == 6 && day > 30 || (month == 7 || month == 8) && day > 31) {
        return false;
    }
    return true;
}

int culculateDate(int month, int day) {
    int ans;
    if (month == 6) {
        ans = day;
    } else if (month == 7) {
        ans = 30 + day;
    } else {
        ans = 61 + day;
    }
    return ans;
}

// saleDate
sjtu::pair<int, int> getSaleDate(const std::string &str) {
    if (str.length() != 11) {
        throw invalid_input();
    }
    for (int i = 0; i < 11; i++) {
        if (i == 2 || i == 8) {
            if (str[i] != '-') {
                throw invalid_input();
            }
        } else if (i == 5) {
            if (str[i] != '|') {
                throw invalid_input();
            }
        } else if (!isnum(str[i])) {
            throw invalid_input();
        }
    }
    int month1, day1, month2, day2;
    month1 = (str[0] - '0') * 10 + (str[1] - '0');
    day1 = (str[3] - '0') * 10 + (str[4] - '0');
    month2 = (str[6] - '0') * 10 + (str[7] - '0');
    day2 = (str[9] - '0') * 10 + (str[10] - '0');
    if(judgeDateCorrectness(month1, day1) && judgeDateCorrectness(month2, day2)) {
        return sjtu::pair(culculateDate(month1, day1), culculateDate(month2, day2));
    }
    throw invalid_input();
}

// date
int getDate(const std::string &str) {
    if (str.length() != 5) {
        throw invalid_input();
    }
    if (!isnum(str[0]) || !isnum(str[1]) || str[2] != '-' || !isnum(str[3]) || !isnum(str[4])) {
        throw invalid_input();
    }
    int month, day;
    month = (str[0] - '0') * 10 + (str[1] - '0');
    day = (str[3] - '0') * 10 + (str[4] - '0');
    if (judgeDateCorrectness(month, day)) {
        return culculateDate(month, day);
    }
    throw invalid_input();
}

// stationNum, seatNum
int getNum(const std::string &num, wordType type = DEFAULT)
{
    int number{};
    for (int i = 0; i < num.length(); i++)
        number = number * 10 + (num[i] - '0');
    if (number != 0 && num[0] == '0')
        throw invalid_input();
    if (type == STATIONNUM && (number < 2 || number > 100) || type == SEATNUM && (number == 0 || number > 100000)) {
        throw invalid_input();
    }
    return number;
}


std::string getword(int &pointer, const std::string &line, wordType type = DEFAULT) {
    std::string temp{};
    while (line[pointer] == ' ' && pointer < line.length()) {
        pointer++;
    }
    char c;
    while (line[pointer] != ' ' && pointer < line.length()) {
        c = line[pointer];
        pointer++;
        if (type == USERNAME && !(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_')
         || type == PASSWORD && !isvisibleletter(c)
         || type == MAILADDR && !(isnum(c) || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '@' || c == '.')
         || type == PRIVILEGE && !isnum(c)
         || type == STATIONNUM && !isnum(c)
         || type == SEATNUM && !isnum(c)
         || type == PRICES && !(isnum(c) || c == '|')
         || type == STARTTIME && !(isnum(c) || c == ':')
         || type == TRAVELTIMES && !(isnum(c) || c == '|')
         || type == STOPOVERTIMES && !(isnum(c) || c == '|' || c == '_')
         || type == SALEDATE && !(isnum(c) || c == '-' || c == '|')
         || type == TYPE && !(c >= 'A' || c <= 'Z')
         || type == DATE && !(isnum(c) || c == '-'))
            throw invalid_input();
        temp.push_back(c);
    }
    if (type == USERNAME && temp.length() > 20
     || type == PASSWORD && temp.length() > 30
     || type == NAME && (temp.length() <= 3 || temp.length() >= 15 || temp.length() % 3 != 0)
     || type == MAILADDR && temp.length() > 30
     || type == PRIVILEGE && temp.length() > 2
     || type == TRAINID && temp.length() > 20
     || type == STATIONNUM && temp.length() > 3
     || type == SEATNUM && temp.length() > 6
     || type == STARTTIME && temp.length() > 5
     || type == STATION && (temp.length() % 3 != 0 || temp.length() > 30)) 
        throw invalid_input();
    return temp;
}

void Parser::parseLine(const std::string &line, Program *program) {
    int pointer{0};
    getword(pointer, line);
    std::string instrument{getword(pointer, line)};
         if (instrument == "add_user")       parseAddUser      (pointer, line, program);
    else if (instrument == "login")          parseLogin        (pointer, line, program);
    else if (instrument == "logout")         parseLogout       (pointer, line, program);
    else if (instrument == "query_profile")  parseQueryProfile (pointer, line, program);
    else if (instrument == "modify_profile") parseModifyProfile(pointer, line, program);
    else if (instrument == "add_train")      parseAddTrain     (pointer, line, program);
    else if (instrument == "delete_train")   parseDeleteTrain  (pointer, line, program);
    else if (instrument == "release_train")  parseReleaseTrain (pointer, line, program);
    else if (instrument == "query_train")    parseQueryTrain   (pointer, line, program);
    else if (instrument == "query_ticket")   parseQueryTicket  (pointer, line, program);
    else if (instrument == "query_transfer") parseQueryTransfer(pointer, line, program);
    else if (instrument == "buy_ticket")     parseBuyTicket    (pointer, line, program);
    else if (instrument == "query_order")    parseQueryOrder   (pointer, line, program);
    else if (instrument == "refund_ticket")  parseRefundTicket (pointer, line, program);
    else if (instrument == "clean")          parseClean        (pointer, line, program);
    else if (instrument == "exit")           parseExit         (pointer, line, program);
    else throw unknown_order();
}

void Parser::parseAddUser(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseAddUser" << std::endl;
    try {
        std::string key, cur_username, username, password, name, mailAddr;
        int privilege;
        bool wrong = false;
        bool getc, getu, getp, getn, getm, getg;
        getc = getu = getp = getn = getm = getg = false;
        while (pointer < line.length()) {
            key = getword(pointer, line);
            if (key.length() != 2 || key[0] != '-') {
                wrong = true;
                break;
            }
            switch (key[1]) {
                case 'c':
                    if (getc) {
                        wrong = true;
                        break;
                    }
                    cur_username = getword(pointer, line, wordType::USERNAME);
                    getc = true;
                    break;
                case 'u':
                    if (getu) {
                        wrong = true;
                        break;
                    }
                    username = getword(pointer, line, wordType::USERNAME);
                    getu = true;
                    break;
                case 'p':
                    if (getp) {
                        wrong = true;
                        break;
                    }
                    password = getword(pointer, line, wordType::PASSWORD);
                    getp = true;
                    break;
                case 'n':
                    if (getn) {
                        wrong = true;
                        break;
                    }
                    name = getword(pointer, line, wordType::NAME);
                    getn = true;
                    break;
                case 'm':
                    if (getm) {
                        wrong = true;
                        break;
                    }
                    mailAddr = getword(pointer, line, wordType::MAILADDR);
                    getm = true;
                    break;
                case 'g':
                    if (getg) {
                        wrong = true;
                        break;
                    }
                    privilege = getNum(getword(pointer, line, wordType::PRIVILEGE));
                    getg = true;
                    if (privilege > 10) {
                        wrong = true;
                    };
                    break;
                default:
                    wrong = true;
            }
            if (wrong) {
                break;
            }
        }
        if (wrong || !getc || !getu || !getp || !getn || !getm || !getg) {
            std::cout << "-1" << std::endl;
        } else {
            program->AddUser(cur_username, username, password, name, mailAddr, privilege);
        }
    } catch(...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseLogin(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseLogin" << std::endl;
    try {
        std::string key, username, password;
        bool wrong = false;
        bool getu, getp;
        getu = getp = false;
        while (pointer < line.length()) {
            key = getword(pointer, line);
            if (key.length() != 2 || key[0] != '-') {
                wrong = true;
                break;
            }
            if (key[1] == 'u') {
                if (getu) {
                    wrong = true;
                    break;
                }
                username = getword(pointer, line, wordType::USERNAME);
                getu = true;
            } else if (key[1] == 'p') {
                if (getp) {
                    wrong = true;
                    break;
                }
                password = getword(pointer, line, wordType::PASSWORD);
                getp = true;
            } else {
                wrong = true;
                break;
            }
        }
        if (wrong || !getu || !getp) {
            std::cout << "-1" << std::endl;
        } else {
            program->Login(username, password);
        }
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseLogout(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseLogout" << std::endl;
    try {
        std::string key, username;
        bool wrong = false;
        key = getword(pointer, line);
        if (key.length() != 2 || key[0] != '-') {
            std::cout << "-1" << std::endl;
            return;
        }
        username = getword(pointer, line, wordType::USERNAME);
        key = getword(pointer, line);
        if (username.empty() || !key.empty()) {
            std::cout << "-1" << std::endl;
            return;
        }
        program->Logout(username);
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseQueryProfile(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseQueryProfile" << std::endl;
    try {
        std::string key, cur_username, username;
        bool wrong = false;
        bool getc, getu;
        getc = getu = false;
        while (pointer < line.length()) {
            key = getword(pointer, line);
            if (key.length() != 2 || key[0] != '-') {
                wrong = true;
                break;
            }
            if (key[1] == 'c') {
                if (getc) {
                    wrong = true;
                    break;
                }
                cur_username = getword(pointer, line, wordType::USERNAME);
                getc = true;
            } else if (key[1] == 'u') {
                if (getu) {
                    wrong = true;
                    break;
                }
                username = getword(pointer, line, wordType::PASSWORD);
                getu = true;
            } else {
                wrong = true;
                break;
            }
        }
        if (wrong || !getc || !getu) {
            std::cout << "-1" << std::endl;
        } else {
            program->QueryProfile(cur_username, username);
        }
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseModifyProfile(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseModifyProfile" << std::endl;
    try {
        std::string key, cur_username{}, username{}, password{}, name{}, mailAddr{};
        int privilege = -1;
        bool wrong = false;
        bool getc, getu, getp, getn, getm, getg;
        getc = getu = getp = getn = getm = getg = false;
        while (pointer < line.length()) {
            key = getword(pointer, line);
            if (key.length() != 2 || key[0] != '-') {
                wrong = true;
                break;
            }
            switch (key[1]) {
                case 'c':
                    if (getc) {
                        wrong = true;
                        break;
                    }
                    cur_username = getword(pointer, line, wordType::USERNAME);
                    getc = true;
                    break;
                case 'u':
                    if (getu) {
                        wrong = true;
                        break;
                    }
                    username = getword(pointer, line, wordType::USERNAME);
                    getu = true;
                    break;
                case 'p':
                    if (getp) {
                        wrong = true;
                        break;
                    }
                    password = getword(pointer, line, wordType::PASSWORD);
                    getp = true;
                    break;
                case 'n':
                    if (getn) {
                        wrong = true;
                        break;
                    }
                    name = getword(pointer, line, wordType::NAME);
                    getn = true;
                    break;
                case 'm':
                    if (getm) {
                        wrong = true;
                        break;
                    }
                    mailAddr = getword(pointer, line, wordType::MAILADDR);
                    getm = true;
                    break;
                case 'g':
                    if (getg) {
                        wrong = true;
                        break;
                    }
                    privilege = getNum(getword(pointer, line, wordType::PRIVILEGE));
                    getg = true;
                    if (privilege > 10) {
                        wrong = true;
                    };
                    break;
                default:
                    wrong = true;
            }
            if (wrong) {
                break;
            }
        }
        if (wrong || !getc || !getu) {
            std::cout << "-1" << std::endl;
        } else {
            program->ModifyProfile(cur_username, username, password, name, mailAddr, privilege);
        }
    } catch(...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseAddTrain(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseAddTrain" << std::endl;
    try {
        std::string key, trainID;
        int stationNum, seatNum, startTime;
        sjtu::vector<std::string> stations;
        sjtu::vector<int> prices, stopoverTimes, travelTimes;
        sjtu::pair<int, int> saleDate;
        char type;
        bool wrong = false;
        bool geti, getn, getm, gets, getp, getx, gett, geto, getd, gety;
        geti = getn = getm = gets = getp = getx = gett = geto = getd = gety = false;
        while (pointer < line.length()) {
            key = getword(pointer, line);
            if (key.length() != 2 || key[0] != '-') {
                wrong = true;
                break;
            }
            switch (key[1]) {
                case 'i':
                    if (geti) {
                        wrong = true;
                        break;
                    }
                    trainID = getword(pointer, line, TRAINID);
                    geti = true;
                    break;
                case 'n':
                    if (getn) {
                        wrong = true;
                        break;
                    }
                    getn = true;
                    stationNum = getNum(getword(pointer, line, STATIONNUM), STATIONNUM);
                    if (stationNum < 2 || stationNum > 100) {
                        wrong = true;
                    }
                    break;
                case 'm':
                    if (getm) {
                        wrong = true;
                        break;
                    }
                    getm = true;
                    seatNum = getNum(getword(pointer, line, SEATNUM), SEATNUM);
                    if (seatNum > 1e5) {
                        wrong = true;
                    }
                    break;
                case 's':
                    if (gets) {
                        wrong = true;
                        break;
                    }
                    gets = true;
                    stations = getStations(getword(pointer, line, STATIONS));
                    break;
                case 'p':
                    if (getp) {
                        wrong = true;
                        break;
                    }
                    getp = true;
                    prices = getNums(getword(pointer, line, PRICES), PRICES);
                    break;
                case 'x':
                    if (getx) {
                        wrong = true;
                        break;
                    }
                    getx = true;
                    startTime = getStartTime(getword(pointer, line, STARTTIME));
                    break;
                case 't':
                    if (gett) {
                        wrong = true;
                        break;
                    }
                    gett = true;
                    travelTimes = getNums(getword(pointer, line, TRAVELTIMES), TRAVELTIMES);
                    break;
                case 'o':
                    if (geto) {
                        wrong = true;
                        break;
                    }
                    geto = true;
                    stopoverTimes = getNums(getword(pointer, line, STOPOVERTIMES), STOPOVERTIMES);
                    break;
                case 'd':
                    if (getd) {
                        wrong = true;
                        break;
                    }
                    getd = true;
                    saleDate = getSaleDate(getword(pointer, line, SALEDATE));
                    break;
                case 'y': {
                    if (gety) {
                        wrong = true;
                        break;
                    }
                    gety = true;
                    std::string str = getword(pointer, line, TYPE);
                    if (str.length() != 1) {
                        wrong = true;
                        break;
                    }
                    type = str[0];
                    break;
                }
                default:
                    wrong = true;
            }
            if (wrong) {
                break;
            }
        }
        if (wrong || !geti || !getn || !getm || !gets || !getp || !getx || !gett || !geto || !getd || !gety || prices.size() != stationNum - 1 || travelTimes.size() != stationNum - 1 || stopoverTimes.size() != stationNum - 2) {
            std::cout << "-1" << std::endl;
        } else {
            program->AddTrain(trainID, stationNum, seatNum, stations, prices, startTime, travelTimes, stopoverTimes, saleDate, type);
        }
    } catch(...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseDeleteTrain(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseDeleteTrain" << std::endl;
    try {
        std::string key, trainID;
        key = getword(pointer, line);
        if (key.length() != 2 && key != "-i") {
            std::cout << "-1" << std::endl;
            return;
        }
        trainID = getword(pointer, line, TRAINID);
        key = getword(pointer, line);
        if (trainID.empty() || !key.empty()) {
            std::cout << "-1" << std::endl;
            return;
        }
        program->DeleteTrain(trainID);
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseReleaseTrain(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseReleaseTrain" << std::endl;
    try {
        std::string key, trainID;
        key = getword(pointer, line);
        if (key.length() != 2 && key != "-i") {
            std::cout << "-1" << std::endl;
            return;
        }
        trainID = getword(pointer, line, TRAINID);
        key = getword(pointer, line);
        if (trainID.empty() || !key.empty()) {
            std::cout << "-1" << std::endl;
            return;
        }
        program->ReleaseTrain(trainID);
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseQueryTrain(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseQueryTrain" << std::endl;
    try {
        std::string key, trainID;
        int date;
        bool geti = false, getd = false;
        while (pointer != line.length()) {
            key = getword(pointer, line);
            if (key == "-i") {
                if (geti) {
                    std::cout << "-1" << std::endl;
                    return;
                }
                geti = true;
                trainID = getword(pointer, line, TRAINID);
            } else if (key == "-d") {
                if (getd) {
                    std::cout << "-1" << std::endl;
                    return;
                }
                getd = true;
                date = getDate(getword(pointer, line, DATE));
            } else {
                std::cout << "-1" << std::endl;
                return;
            }
        }
        program->QueryTrain(trainID, date);
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseQueryTicket(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseQueryTicket" << std::endl;
    try {
        std::string key, station1, station2;
        bool query_type; // 0->time, 1->cost
        int date;
        bool gets, gett, getd, getp;
        gets = gett = getd = getp = false;
        while (pointer != line.length()) {
            key = getword(pointer, line);
            if (key.length() != 2 || key[0] != '-') {
                std::cout << "-1" << std::endl;
                return;
            }
            switch (key[1]) {
                case 'd': {
                    if (gets) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    gets = true;
                    date = getDate(getword(pointer, line, DATE));
                    break;
                }
                case 's':
                    if (gets) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    gets = true;
                    station1 = getword(pointer, line, STATION);
                    break;
                case 't':
                    if (gett) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    gett = true;
                    station2 = getword(pointer, line, STATION);
                    break;
                case 'p':
                    if (getp) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    getp = true;
                    key = getword(pointer, line);
                    if (key == "time") {
                        query_type = 0;
                    } else if (key == "cost") {
                        query_type = 1;
                    } else {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    break;
                default:
                    std::cout << "-1" << std::endl;
                    return;
            }
        }
        if (!gets || !gett || !getd || !getp) {
            std::cout << "-1" << std::endl;
            return;
        }
        program->QueryTicket(station1, station2, date, query_type);
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseQueryTransfer(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseQueryTransfer" << std::endl;
    try {
        std::string key, station1, station2;
        bool query_type; // 0->time, 1->cost
        int date;
        bool gets, gett, getd, getp;
        gets = gett = getd = getp = false;
        while (pointer != line.length()) {
            key = getword(pointer, line);
            if (key.length() != 2 || key[0] != '-') {
                std::cout << "-1" << std::endl;
                return;
            }
            switch (key[1]) {
                case 'd':
                    if (gets) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    gets = true;
                    date = getDate(getword(pointer, line, DATE));
                    break;
                case 's':
                    if (gets) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    gets = true;
                    station1 = getword(pointer, line, STATION);
                    break;
                case 't':
                    if (gett) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    gett = true;
                    station2 = getword(pointer, line, STATION);
                    break;
                case 'p':
                    if (getp) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    getp = true;
                    key = getword(pointer, line);
                    if (key == "time") {
                        query_type = 0;
                    } else if (key == "cost") {
                        query_type = 1;
                    } else {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    break;
                default:
                    std::cout << "-1" << std::endl;
                    return;
            }
        }
        if (!gets || !gett || !getd || !getp) {
            std::cout << "-1" << std::endl;
            return;
        }
        program->QueryTicket(station1, station2, date, query_type);
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseBuyTicket(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseBuyTicket" << std::endl;
    try {
        std::string key, username, trainID, station1, station2;
        int date, ticketnum;
        bool buy_type;
        bool getu, geti, getd, getn, getf, gett, getq;
        getu = geti = getd = getn = getf = gett = getq = false;
        while (pointer != line.length()) {
            key = getword(pointer, line);
            if (key.length() != 2 || key[0] != '-') {
                std::cout << "-1" << std::endl;
                return;
            }
            switch (key[1]) {
                case 'u':
                    if (getu) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    getu = true;
                    username = getword(pointer, line, USERNAME);
                    break;
                case 'i':
                    if (geti) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    geti = true;
                    trainID = getword(pointer, line, TRAINID);
                    break;
                case 'd':
                    if (getd) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    getd = true;
                    date = getDate(getword(pointer, line, DATE));
                    break;
                case 'n':
                    if (getn) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    getn = true;
                    ticketnum = getNum(getword(pointer, line, SEATNUM), SEATNUM);
                    break;
                case 'f':
                    if (getf) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    getf = true;
                    station1 = getword(pointer, line, STATION);
                    break;
                case 't':
                    if (gett) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    gett = true;
                    station2 = getword(pointer, line, STATION);
                    break;
                case 'q':
                    if (getq) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    getq = true;
                    key = getword(pointer, line);
                    if (key == "false") {
                        buy_type = false;
                    } else if (key == "true") {
                        buy_type = true;
                    } else {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    break;
                default:
                    std::cout << "-1" << std::endl;
            }
        }
        if (!getu || !geti || !getd || !getn || !getf || !gett || !getq) {
            std::cout << "-1" << std::endl;
            return;
        }
        program->BuyTicket(username, trainID, date, ticketnum, station1, station2, buy_type);
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseQueryOrder(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseQueryOrder" << std::endl;
    try {
        std::string key, username;
        key = getword(pointer, line);
        if (key == "-u") {
            username = getword(pointer, line, USERNAME);
            key = getword(pointer, line);
            if (username.empty() || !key.empty()) {
                std::cout << "-1" << std::endl;
                return;
            }
            program->QueryOrder(username);
        }
    } catch(...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseRefundTicket(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseRefundTicket" << std::endl;
    try {
        std::string key, username;
        int ticketnum;
        bool getu = false, getn = false;
        while (pointer < line.length()) {
            key = getword(pointer, line);
            if (key.length() != 2 || key[0] != '-') {
                std::cout << "-1" << std::endl;
                return;
            }
            switch (key[1]) {
                case 'u':
                    if (getu) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    getu = true;
                    username = getword(pointer, line, USERNAME);
                    break;
                case 'n':
                    if (getn) {
                        std::cout << "-1" << std::endl;
                        return;
                    }
                    getn = true;
                    ticketnum = getNum(getword(pointer, line, SEATNUM));
                    break;
                default:
                    std::cout << "-1" << std::endl;
            }
        }
        if (!getu || !getn) {
            std::cout << "-1" << std::endl;
            return;
        }
        program->RefundTicket(username, ticketnum);
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseClean(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseClean" << std::endl;
    try {
        if (!getword(pointer, line).empty()) {
            std::cout << "-1" << std::endl;
            return;
        }
        program->Clean();
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}

void Parser::parseExit(int &pointer, const std::string &line, Program *program) {
    std::cout << "parseExit" << std::endl;
    try {
        if (!getword(pointer, line).empty()) {
            std::cout << "-1" << std::endl;
            return;
        }
        program->Exit();
    } catch (...) {
        std::cout << "-1" << std::endl;
    }
}