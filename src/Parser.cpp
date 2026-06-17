#include <iostream>
#include <string>

#include "Parser.hpp"
#include "Program.hpp"
#include "tools.hpp"
#include "STLite/vector.hpp"
#include "STLite/string.hpp"

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

int getStations(int &pointer, const std::string &line, bool &error, sjtu::string<30> *answer)
{
    int count = 0;
    sjtu::string<30> str;

    while (line[pointer] == ' ' && pointer < line.length()) {
        pointer++;
    }
    while (line[pointer] != ' ' && pointer < line.length()) {
        if(line[pointer] == '|')
        {
            if(str.empty() || str.length() % 3 != 0) {
                error = true;
                return count;
            }
            answer[count++] = str;
            str.clear();
        } else {
            str += line[pointer];
            if (str.length() == 30) {
                error = true;
                return count;
            }
        }
        pointer++;
    }
    answer[count++] = str;
    return count;
}

// prices, travelTimes, stopocerTimes，写入 answer 数组，返回数字个数
int getNums(int &pointer, const std::string &line, bool &error, wordType type, int *answer) {
    int count = 0;
    int num = 0;

    while (line[pointer] == ' ' && pointer < line.length()) {
        pointer++;
    }

    if (line[pointer] == '_' && (pointer == line.length() - 1 || line[pointer + 1] == ' ') && type == STOPOVERTIMES) {
        pointer++;
        return count;
    }
    while (line[pointer] != ' ' && pointer < line.length()) {
        if (line[pointer] == '|') {
            if (type == PRICES && num > 1e5 || type == TRAVELTIMES && num > 1e4 || type == STOPOVERTIMES && num > 1e4) {
                error = true;
                return count;
            } else {
                answer[count++] = num;
            }
            num = 0;
        } else if (isnum(line[pointer])) {
            num = num * 10 + (line[pointer] - '0');
        } else {
            error = true;
            return count;
        }
        pointer++;
    }
    answer[count++] = num;
    return count;
}


// startTime
int getStartTime(int &pointer, const std::string &line, bool &error) {
    while (line[pointer] == ' ' && pointer < line.length()) {
        pointer++;
    }
    if (pointer + 4 >= line.length() || !isnum(line[pointer]) || !isnum(line[pointer + 1]) || line[pointer + 2] != ':' || !isnum(line[pointer + 3]) || !isnum(line[pointer + 4]) || pointer + 5 < line.length() && line[pointer + 5] != ' ') {
        error = true;
        return -1;
    }
    int ans = ((line[pointer] - '0') * 10 + (line[pointer + 1] - '0')) * hourtime + ((line[pointer + 3] - '0') * 10 + (line[pointer + 4] - '0'));
    pointer += 5;
    if (ans >= daytime) {
        error = true;
        return -1;
    } else {
        return ans;
    }
}


bool judgeDateCorrectness(int month, int day) {
    if (month < 1 || month > 12) {
        return false;
    }
    if (day < 1 || day > monthday[month]) {
        return false;
    }
    return true;
}

int culculateDate(int month, int day) {
    return sumday[month] + day;
}

// saleDate
sjtu::pair<int, int> getSaleDate(int &pointer, const std::string &line, bool &error) {
    while (line[pointer] == ' ' && pointer < line.length()) {
        pointer++;
    }
    if (pointer + 10 >= line.length() || !isnum(line[pointer]) || !isnum(line[pointer + 1]) || line[pointer + 2] != '-' || !isnum(line[pointer + 3]) || !isnum(line[pointer + 4]) || line[pointer + 5] != '|' || !isnum(line[pointer + 6]) || !isnum(line[pointer + 7]) || line[pointer + 8] != '-' || !isnum(line[pointer + 9]) || !isnum(line[pointer + 10]) || pointer + 11 < line.length() && line[pointer + 11] != ' ') {
        error = true;
        return sjtu::pair(-1, -1);
    }
    int month1 = (line[pointer] - '0') * 10 + (line[pointer + 1] - '0');
    int day1 = (line[pointer + 3] - '0') * 10 + (line[pointer + 4] - '0');
    int month2 = (line[pointer + 6] - '0') * 10 + (line[pointer + 7] - '0');
    int day2 = (line[pointer + 9] - '0') * 10 + (line[pointer + 10] - '0');
    pointer += 11;
    if (!judgeDateCorrectness(month1, day1) || !judgeDateCorrectness(month2, day2)) {
        error = true;
        return sjtu::pair(-1, -1);
    }
    return sjtu::pair(culculateDate(month1, day1), culculateDate(month2, day2));
}

// date
int getDate(int &pointer, const std::string &line, bool &error) {
    while (line[pointer] == ' ' && pointer < line.length()) {
        pointer++;
    }
    if (pointer + 4 >= line.length() || !isnum(line[pointer]) || !isnum(line[pointer + 1]) || line[pointer + 2] != '-' || !isnum(line[pointer + 3]) || !isnum(line[pointer + 4]) || pointer + 5 < line.length() && line[pointer + 5] != ' ') {
        error = true;
        return -1;
    }
    int month = (line[pointer] - '0') * 10 + (line[pointer + 1] - '0');
    int day = (line[pointer + 3] - '0') * 10 + (line[pointer + 4] - '0');
    pointer += 5;
    if (!judgeDateCorrectness(month, day)) {
        error = true;
        return -1;
    } else {
        return culculateDate(month, day);
    }
}

// stationNum, seatNum, privilege
int getNum(int &pointer, const std::string &line, bool &error, wordType type)
{
    int number = 0;
    while (line[pointer] == ' ' && pointer < line.length()) {
        pointer++;
    }
    int tmp = pointer;
    while (line[pointer] != ' ' && pointer< line.length()) {
        if (!isnum(line[pointer])) {
            error = true;
            return -1;
        }
        number = number * 10 + (line[pointer] - '0');
        if (type == STATIONNUM && number > 100 || type == SEATNUM && number > 100000 || type == PRIVILEGE && number > 10) {
            error = true;
            return -1;
        }
        pointer++;
    }
    if (number == 0 && line[tmp] == '0' && tmp + 1 < line.length() && line[tmp + 1] != ' ') {
        error = true;
        return -1;
    }
    if (type == STATIONNUM && number < 2) {
        error = true;
        return -1;
    }
    return number;
}

// username, password, name, mailAddr, trainID, type
template <int strlength>
sjtu::string<strlength> getword(int &pointer, const std::string &line, bool &error, wordType type) {
    sjtu::string<strlength> temp;
    while (line[pointer] == ' ' && pointer < line.length()) {
        pointer++;
    }
    char c;
    while (line[pointer] != ' ' && pointer < line.length()) {
        c = line[pointer];
        pointer++;
        if (type == USERNAME && (!(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_'))
         || type == PASSWORD && !isvisibleletter(c)
         || type == MAILADDR && !(isnum(c) || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '@' || c == '.')
         || type == TYPE && !(c >= 'A' || c <= 'Z')
         || temp.length() == strlength) {
            error = true;
            return temp;
         }
        temp.push_back(c);
    }
    if (type == NAME && (temp.length() % 3 != 0 || temp.length() <= 3 || temp.length() > 15)) {
        error = true;
        return temp;
     }
    return temp;
}

std::string getword(int &pointer, const std::string &line) {
    std::string str;
    while (line[pointer] == ' ' && pointer < line.length()) {
        pointer++;
    }
    while (line[pointer] != ' ' && pointer < line.length()) {
        str.push_back(line[pointer]);
        pointer++;
    }
    return str;
}

void Parser::parseLine(const std::string &line, Program *program) {
    int pointer{0};
    std::string timestamp = getword(pointer, line);
    std::cout << timestamp << ' ';
    std::string instrument= getword(pointer, line);
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
    // std::cout << "parseAddUser" << std::endl;
    std::string key;
    sjtu::string<20> cur_username, username;
    sjtu::string<30> password, mailAddr;
    sjtu::string<15> name;
    int privilege;
    bool error = false;
    bool getc, getu, getp, getn, getm, getg;
    getc = getu = getp = getn = getm = getg = false;
    while (pointer < line.length()) {
        key = getword(pointer, line);
        if (key.length() != 2 || key[0] != '-') {
            error = true;
            break;
        }
        switch (key[1]) {
            case 'c':
                if (getc) {
                    error = true;
                    break;
                }
                cur_username = getword<20>(pointer, line, error, wordType::USERNAME);
                getc = true;
                break;
            case 'u':
                if (getu) {
                    error = true;
                    break;
                }
                username = getword<20>(pointer, line, error, wordType::USERNAME);
                getu = true;
                break;
            case 'p':
                if (getp) {
                    error = true;
                    break;
                }
                password = getword<30>(pointer, line, error, wordType::PASSWORD);
                getp = true;
                break;
            case 'n':
                if (getn) {
                    error = true;
                    break;
                }
                name = getword<15>(pointer, line, error, wordType::NAME);
                getn = true;
                break;
            case 'm':
                if (getm) {
                    error = true;
                    break;
                }
                mailAddr = getword<30>(pointer, line, error, wordType::MAILADDR);
                getm = true;
                break;
            case 'g':
                if (getg) {
                    error = true;
                    break;
                }
                privilege = getNum(pointer, line, error, wordType::PRIVILEGE);
                getg = true;
                break;
            default:
                error = true;
        }
        if (error) {
            break;
        }
    }
    if (error || !getc || !getu || !getp || !getn || !getm || !getg) {
        std::cout << "-1\n";
    } else {
        program->AddUser(cur_username, username, password, name, mailAddr, privilege);
    }
}

void Parser::parseLogin(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseLogin" << std::endl;
    std::string key;
    sjtu::string<20> username;
    sjtu::string<30> password;
    bool error = false;
    bool getu, getp;
    getu = getp = false;
    while (pointer < line.length()) {
        key = getword(pointer, line);
        if (key.length() != 2 || key[0] != '-') {
            error = true;
            break;
        }
        if (key[1] == 'u') {
            if (getu) {
                error = true;
                break;
            }
            username = getword<20>(pointer, line, error, wordType::USERNAME);
            getu = true;
        } else if (key[1] == 'p') {
            if (getp) {
                error = true;
                break;
            }
            password = getword<30>(pointer, line, error, wordType::PASSWORD);
            getp = true;
        } else {
            error = true;
            break;
        }
    }
    if (error || !getu || !getp) {
        std::cout << "-1\n";
    } else {
        program->Login(username, password);
    }
}

void Parser::parseLogout(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseLogout" << std::endl;
    std::string key;
    sjtu::string<20> username;
    bool error = false;
    key = getword(pointer, line);
    if (key.length() != 2 || key[0] != '-') {
        std::cout << "-1\n";
        return;
    }
    username = getword<20>(pointer, line, error, wordType::USERNAME);
    key = getword(pointer, line);
    if (error || username.empty() || !key.empty()) {
        std::cout << "-1\n";
        return;
    }
    program->Logout(username);
}

void Parser::parseQueryProfile(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseQueryProfile" << std::endl;
    std::string key;
    sjtu::string<20> cur_username, username;
    bool error = false;
    bool getc, getu;
    getc = getu = false;
    while (pointer < line.length()) {
        key = getword(pointer, line);
        if (key.length() != 2 || key[0] != '-') {
            error = true;
            break;
        }
        if (key[1] == 'c') {
            if (getc) {
                error = true;
                break;
            }
            cur_username = getword<20>(pointer, line, error, wordType::USERNAME);
            getc = true;
        } else if (key[1] == 'u') {
            if (getu) {
                error = true;
                break;
            }
            username = getword<20>(pointer, line, error, wordType::PASSWORD);
            getu = true;
        } else {
            error = true;
            break;
        }
    }
    if (error || !getc || !getu) {
        std::cout << "-1\n";
        return;
    }
    program->QueryProfile(cur_username, username);
}

void Parser::parseModifyProfile(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseModifyProfile" << std::endl;
    std::string key;
    sjtu::string<20> cur_username, username;
    sjtu::string<30> password;
    sjtu::string<15> name;
    sjtu::string<30> mailAddr;
    int privilege = -1;
    bool error = false;
    bool getc, getu, getp, getn, getm, getg;
    getc = getu = getp = getn = getm = getg = false;
    while (pointer < line.length()) {
        key = getword(pointer, line);
        if (key.length() != 2 || key[0] != '-') {
            error = true;
            break;
        }
        switch (key[1]) {
            case 'c':
                if (getc) {
                    error = true;
                    break;
                }
                cur_username = getword<20>(pointer, line, error, wordType::USERNAME);
                getc = true;
                break;
            case 'u':
                if (getu) {
                    error = true;
                    break;
                }
                username = getword<20>(pointer, line, error, wordType::USERNAME);
                getu = true;
                break;
            case 'p':
                if (getp) {
                    error = true;
                    break;
                }
                password = getword<30>(pointer, line, error, wordType::PASSWORD);
                getp = true;
                break;
            case 'n':
                if (getn) {
                    error = true;
                    break;
                }
                name = getword<15>(pointer, line, error, wordType::NAME);
                getn = true;
                break;
            case 'm':
                if (getm) {
                    error = true;
                    break;
                }
                mailAddr = getword<30>(pointer, line, error, wordType::MAILADDR);
                getm = true;
                break;
            case 'g':
                if (getg) {
                    error = true;
                    break;
                }
                privilege = getNum(pointer, line, error, wordType::PRIVILEGE);
                getg = true;
                break;
            default:
                error = true;
        }
        if (error) {
            break;
        }
    }
    if (error || !getc || !getu) {
        std::cout << "-1\n";
    } else {
        program->ModifyProfile(cur_username, username, password, name, mailAddr, privilege);
    }
}

void Parser::parseAddTrain(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseAddTrain" << std::endl;
    std::string key;
    sjtu::string<20> trainID;
    int stationNum, seatNum, startTime;
    sjtu::string<30> stations[100];
    int prices[100], stopoverTimes[100], travelTimes[100];
    int stationCount = 0, priceCount = 0, travelCount = 0, stopoverCount = 0;
    sjtu::pair<int, int> saleDate;
    char type;
    bool error = false;

    bool geti, getn, getm, gets, getp, getx, gett, geto, getd, gety;
    geti = getn = getm = gets = getp = getx = gett = geto = getd = gety = false;
    while (pointer < line.length()) {
        key = getword(pointer, line);
        if (key.length() != 2 || key[0] != '-') {
            error = true;
            break;
        }
        switch (key[1]) {
            case 'i':
                if (geti) {
                    error = true;
                    break;
                }
                trainID = getword<20>(pointer, line, error, TRAINID);
                geti = true;
                break;
            case 'n':
                if (getn) {
                    error = true;
                    break;
                }
                getn = true;
                stationNum = getNum(pointer, line, error, STATIONNUM);
                break;
            case 'm':
                if (getm) {
                    error = true;
                    break;
                }
                getm = true;
                seatNum = getNum(pointer, line, error, SEATNUM);
                break;
            case 's':
                if (gets) {
                    error = true;
                    break;
                }
                gets = true;
                stationCount = getStations(pointer, line, error, stations);
                break;

            case 'p':
                if (getp) {
                    error = true;
                    break;
                }
                getp = true;
                priceCount = getNums(pointer, line, error, PRICES, prices);
                break;

            case 'x':
                if (getx) {
                    error = true;
                    break;
                }
                getx = true;
                startTime = getStartTime(pointer, line, error);
                break;
            case 't':
                if (gett) {
                    error = true;
                    break;
                }
                gett = true;
                travelCount = getNums(pointer, line, error, TRAVELTIMES, travelTimes);
                break;

            case 'o':
                if (geto) {
                    error = true;
                    break;
                }
                geto = true;
                stopoverCount = getNums(pointer, line, error, STOPOVERTIMES, stopoverTimes);
                break;

            case 'd':
                if (getd) {
                    error = true;
                    break;
                }
                getd = true;
                saleDate = getSaleDate(pointer, line, error);
                if (saleDate.first <=sumday[6] || saleDate.first > sumday[9] || saleDate.second <= sumday[6] || saleDate.second > sumday[9]) {
                    error = true;
                    break;
                }
                break;
            case 'y': {
                if (gety) {
                    error = true;
                    break;
                }
                gety = true;
                sjtu::string<1> str = getword<1>(pointer, line, error, TYPE); 
                type = str[0];
                break;
            }
            default:
                error = true;
        }
        if (error) {
            break;
        }
    }
    if (error || !geti || !getn || !getm || !gets || !getp || !getx || !gett || !geto || !getd || !gety || stationCount != stationNum || priceCount != stationNum - 1 || travelCount != stationNum - 1 || stopoverCount != stationNum - 2) {

        std::cout << "-1\n";
        return;
    }
    program->AddTrain(trainID, stationNum, seatNum, stations, prices, startTime, travelTimes, stopoverTimes, saleDate, type);
}

void Parser::parseDeleteTrain(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseDeleteTrain" << std::endl;
    std::string key;
    sjtu::string<20> trainID;
    bool error = false;
    key = getword(pointer, line);
    if (key.length() != 2 && key != "-i") {
        std::cout << "-1\n";
        return;
    }
    trainID = getword<20>(pointer, line, error, TRAINID);
    key = getword(pointer, line);
    if (trainID.empty() || !key.empty() || error) {
        std::cout << "-1\n";
        return;
    }
    program->DeleteTrain(trainID);
}

void Parser::parseReleaseTrain(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseReleaseTrain" << std::endl;
    std::string key;
    sjtu::string<20> trainID;
    bool error = false;
    key = getword(pointer, line);
    if (key.length() != 2 && key != "-i") {
        std::cout << "-1\n";
        return;
    }
    trainID = getword<20>(pointer, line, error, TRAINID);
    key = getword(pointer, line);
    if (error || trainID.empty() || !key.empty()) {
        std::cout << "-1\n";
        return;
    }
    program->ReleaseTrain(trainID);
}

void Parser::parseQueryTrain(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseQueryTrain" << std::endl;
    std::string key;
    sjtu::string<20> trainID;
    int date;
    bool geti = false, getd = false;
    bool error = false;
    while (pointer != line.length()) {
        key = getword(pointer, line);
        if (key == "-i") {
            if (geti) {
                error = true;
                break;
            }
            geti = true;
            trainID = getword<20>(pointer, line, error, TRAINID);
        } else if (key == "-d") {
            if (getd) {
                error = true;
                break;
            }
            getd = true;
            date = getDate(pointer, line, error);
        } else {
            error = true;
            break;
        }
    }
    if (error || !geti || !getd) {
        std::cout << "-1\n";
        return;
    }
    program->QueryTrain(trainID, date);
}

void Parser::parseQueryTicket(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseQueryTicket" << std::endl;
    std::string key;
    sjtu::string<30> station1, station2;
    bool query_type = false; // 0->time, 1->cost
    int date;
    bool gets, gett, getd, getp;
    gets = gett = getd = getp = false;
    bool error = false;
    while (pointer != line.length()) {
        key = getword(pointer, line);
        if (key.length() != 2 || key[0] != '-') {
            error = true;
            break;
        }
        switch (key[1]) {
            case 'd': {
                if (getd) {
                    error = true;
                    break;
                }
                getd = true;
                date = getDate(pointer, line, error);
                break;
            }
            case 's':
                if (gets) {
                    error = true;
                    break;
                }
                gets = true;
                station1 = getword<30>(pointer, line, error, STATION);
                break;
            case 't':
                if (gett) {
                    error = true;
                    break;
                }
                gett = true;
                station2 = getword<30>(pointer, line, error, STATION);
                break;
            case 'p':
                if (getp) {
                    error = true;
                    break;
                }
                getp = true;
                key = getword(pointer, line);
                if (key == "time") {
                    query_type = 0;
                } else if (key == "cost") {
                    query_type = 1;
                } else {
                    error = true;
                }
                break;
            default:
                error = true;
        }
        if (error) {
            break;
        }
    }
    if (error || !gets || !gett || !getd) {
        std::cout << "-1\n";
        return;
    }
    program->QueryTicket(station1, station2, date, query_type);
}

void Parser::parseQueryTransfer(int &pointer, const std::string &line, Program *program) {
    std::string key;
    sjtu::string<30> station1, station2;
    bool query_type = false; // 0->time, 1->cost
    int date;
    bool gets, gett, getd, getp;
    gets = gett = getd = getp = false;
    bool error = false;
    while (pointer != line.length()) {
        key = getword(pointer, line);
        if (key.length() != 2 || key[0] != '-') {
            error = true;
            break;
        }
        switch (key[1]) {
            case 'd':
                if (getd) {
                    error = true;
                    break;
                }
                getd = true;
                date = getDate(pointer, line, error);
                break;
            case 's':
                if (gets) {
                    error = true;
                    break;
                }
                gets = true;
                station1 = getword<30>(pointer, line, error, STATION);
                break;
            case 't':
                if (gett) {
                    error = true;
                    break;
                }
                gett = true;
                station2 = getword<30>(pointer, line, error, STATION);
                break;
            case 'p':
                if (getp) {
                    error = true;
                    break;
                }
                getp = true;
                key = getword(pointer, line);
                if (key == "time") {
                    query_type = 0;
                } else if (key == "cost") {
                    query_type = 1;
                } else {
                    error = true;
                }
                break;
            default:
                error = true;
        }
        if (error) {
            break;
        }
    }
    if (error || !gets || !gett || !getd) {
        std::cout << "-1\n";
        return;
    }
    program->QueryTransfer(station1, station2, date, query_type);
}

void Parser::parseBuyTicket(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseBuyTicket" << std::endl;
    std::string key;
    sjtu::string<20> username, trainID;
    sjtu::string<30> station1, station2;
    int date, ticketnum;
    bool buy_type = false;
    bool getu, geti, getd, getn, getf, gett, getq;
    getu = geti = getd = getn = getf = gett = getq = false;
    bool error = false;
    while (pointer != line.length()) {
        key = getword(pointer, line);
        if (key.length() != 2 || key[0] != '-') {
            error = true;
            break;
        }
        switch (key[1]) {
            case 'u':
                if (getu) {
                    error = true;
                    break;
                }
                getu = true;
                username = getword<20>(pointer, line, error, USERNAME);
                break;
            case 'i':
                if (geti) {
                    error = true;
                    break;
                }
                geti = true;
                trainID = getword<20>(pointer, line, error, TRAINID);
            break;
            case 'd':
                if (getd) {
                    error = true;
                    break;
                }
                getd = true;
                date = getDate(pointer, line, error);
                break;
            case 'n':
                if (getn) {
                    error = true;
                    break;
                }
                getn = true;
                ticketnum = getNum(pointer, line, error, SEATNUM);
                break;
            case 'f':
                if (getf) {
                    error = true;
                    break;
                }
                getf = true;
                station1 = getword<30>(pointer, line, error, STATION);
                break;
            case 't':
                if (gett) {
                    error = true;
                    break;
                }
                gett = true;
                station2 = getword<30>(pointer, line, error, STATION);
                break;
            case 'q':
                if (getq) {
                    error = true;
                    break;
                }
                getq = true;
                key = getword(pointer, line);
                if (key == "false") {
                    buy_type = false;
                } else if (key == "true") {
                    buy_type = true;
                } else {
                    error = true;
                }
                break;
            default:
                error = true;
        }
        if (error) {
            break;
        }
    }
    if (error || !getu || !geti || !getd || !getn || !getf || !gett) {
        std::cout << "-1\n";
        return;
    }
    program->BuyTicket(username, trainID, date, ticketnum, station1, station2, buy_type);
}

void Parser::parseQueryOrder(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseQueryOrder" << std::endl;
    std::string key;
    sjtu::string<20> username;
    bool error = false;
    key = getword(pointer, line);
    if (key == "-u") {
        username = getword<20>(pointer, line, error, USERNAME);
        key = getword(pointer, line);
        if (error || username.empty() || !key.empty()) {
            std::cout << "-1\n";
            return;
        }
        program->QueryOrder(username);
    } else {
        std::cout << "-1\n";
    }
}

void Parser::parseRefundTicket(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseRefundTicket" << std::endl;
    std::string key;
    sjtu::string<20> username;
    int ticketnum = 1;
    bool getu = false, getn = false;
    bool error = false;
     while (pointer < line.length()) {
        key = getword(pointer, line);
        if (key.length() != 2 || key[0] != '-') {
            error = true;
            break;
        }
        switch (key[1]) {
            case 'u':
                if (getu) {
                    error = true;
                    break;
                }
                getu = true;
                username = getword<20>(pointer, line, error, USERNAME);
                break;
            case 'n':
                if (getn) {
                    error = true;
                    break;
                }
                getn = true;
                ticketnum = getNum(pointer, line, error, SEATNUM);
                break;
            default:
                error = true;
        }
        if (error) {
            break;
        }
    }
    if (error || !getu) {
        std::cout << "-1\n";
        return;
    }
    program->RefundTicket(username, ticketnum);
}

void Parser::parseClean(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseClean" << std::endl;
    bool error = false;
    if (!getword(pointer, line).empty()) {
        std::cout << "-1\n";
        return;
    }
    program->Clean();
}

void Parser::parseExit(int &pointer, const std::string &line, Program *program) {
    // std::cout << "parseExit" << std::endl;
    bool error = false;
    if (!getword(pointer, line).empty()) {
        std::cout << "-1\n";
        return;
    }
    program->Exit();
}