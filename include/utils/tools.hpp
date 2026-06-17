#pragma once

#include <iostream>

#include "STLite/string.hpp"

inline constexpr int sizeofint = sizeof(int);
inline constexpr int sizeofchar = sizeof(char);
inline constexpr int sizeoftrainID = sizeof(sjtu::string<20>);
inline constexpr int sizeofstation = sizeof(sjtu::string<30>);
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