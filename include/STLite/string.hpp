#pragma once

#include <cstring>
#include <iostream>

namespace sjtu {

template<int strlength>
struct string {
    char data[strlength];
    int len;

    string() {
        len = 0;
        std::memset(data, 0, strlength);
    }

    string(const char* str) {
        len = std::strlen(str);
        std::memcpy(data, str, len);
    }

    string(const string &) = default;

    string &operator=(const string &) = default;

    int length() const {
        return len;
    }

    bool empty() const {
        return len == 0;
    }

    char &operator[](int idx) {
        return data[idx];
    }
    const char &operator[](int idx) const {
        return data[idx];
    }

    const char *c_str() const {
        return data;
    }

    string &push_back(char c) {
        data[len] = c;
        len++;
        return *this;
    }

    string &operator+=(char c) {
        return push_back(c);
    }

    void clear() {
        len = 0;
        std::memset(data, 0, strlength);
    }

    bool operator<(const string &o) const { return std::strncmp(data, o.data, strlength) < 0; }
    bool operator>(const string &o) const { return std::strncmp(data, o.data, strlength) > 0; }
    bool operator==(const string &o) const { return len == o.len && std::strncmp(data, o.data, strlength) == 0; }
    bool operator==(const char* &o) const { return std::strncmp(data, o, strlength) == 0; }
    bool operator!=(const string &o) const { return len != o.len || std::strncmp(data, o.data, strlength) != 0; }
    bool operator<=(const string &o) const { return std::strncmp(data, o.data, strlength) <= 0; }
    bool operator>=(const string &o) const { return std::strncmp(data, o.data, strlength) >= 0; }

    friend std::ostream &operator<<(std::ostream &os, const string &s) {
        for (int i = 0; i < s.len; i++) {
            os << s.data[i];
        }
        return os;
    }
};

} // namespace sjtu
