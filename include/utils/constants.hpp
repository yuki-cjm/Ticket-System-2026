#pragma once

#include <iostream>

#include "STLite/string.hpp"

inline constexpr int UserNameLength = 20;
inline constexpr int PassWordLength = 30;
inline constexpr int minNameLength = 6;
inline constexpr int NameLength = 15;
inline constexpr int MailAddrLength = 30;
inline constexpr int TrainIDLength = 20;
inline constexpr int StationLength = 30;

inline constexpr int maxPrivilege = 10;
inline constexpr int minStationNum = 2;
inline constexpr int maxStationNum = 40;
inline constexpr int maxSeatNum = 100000;
inline constexpr int maxPrice = 100000;
inline constexpr int maxTravelTime = 10000;
inline constexpr int maxStopoverTime = 10000;

inline constexpr int sizeofint = sizeof(int);
inline constexpr int sizeofchar = sizeof(char);
inline constexpr int sizeoftrainID = sizeof(sjtu::string<TrainIDLength>);
inline constexpr int sizeofstation = sizeof(sjtu::string<StationLength>);
inline constexpr int sizeofpassword = sizeof(sjtu::string<PassWordLength>);
inline constexpr int sizeofname = sizeof(sjtu::string<NameLength>);
inline constexpr int sizeofmailAddr = sizeof(sjtu::string<MailAddrLength>);
inline constexpr int hourtime = 60;
inline constexpr int daytime = 1440;
inline constexpr int monthday[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
inline constexpr int sumday[13] = {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};


inline int culculateTime(int date, int time) {
    return date * daytime + time;
}

inline void printNum(int num) {
    if (num >= 10) {
        std::cout << num;
    } else if (num >= 1) {
        std::cout << '0' << num;
    } else {
        std::cout << "00";
    }
}

inline void printTime(int time) {
    int day = time / daytime, month;
    for (month = 1; month <= 12; month++) {
        if (day > monthday[month]) {
            day -= monthday[month];
        } else {
            break;
        }
    }
    time %= daytime;
    printNum(month);
    std::cout << '-';
    printNum(day);
    std::cout << ' ';
    printNum(time / hourtime);
    std::cout << ':';
    printNum(time % hourtime);
}