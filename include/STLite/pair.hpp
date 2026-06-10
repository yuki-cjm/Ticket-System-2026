#pragma once

namespace sjtu {

template <class T1, class T2>
class pair {
   public:
    T1 first;
    T2 second;
    constexpr pair() : first(), second() {
    }
    pair(const pair &other) = default;
    pair(pair &&other) = default;
    pair(const T1 &x, const T2 &y) : first(x), second(y) {
    }
    template <class U1, class U2>
    pair(U1 &&x, U2 &&y) : first(std::forward<U1>(x)), second(std::forward<U2>(y)) {
    }
    template <class U1, class U2>
    pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {
    }
    template <class U1, class U2>
    pair(pair<U1, U2> &&other)
        : first(std::move(other.first)), second(std::move(other.second)) {
    }

    pair &operator=(const pair &) = default;
    pair &operator=(pair &&) = default;

    template <class U1, class U2>
    pair& operator=(const pair<U1, U2> &other) {
        this->first = other.first;
        this->second = other.second;
        return *this;
    }
};

}  // namespace sjtu

