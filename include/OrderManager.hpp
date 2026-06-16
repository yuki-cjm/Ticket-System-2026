#pragma once

#include <fstream>

#include "STLite/BPT.hpp"
#include "STLite/string.hpp"
#include "STLite/vector.hpp"

constexpr int sizeofint = sizeof(int);

struct Ticket {
    sjtu::string<20> trainID;
    int leavingtime, arrivingtime, price, seat;

    Ticket() = default;
};

struct Transfer {
    sjtu::string<20> train1ID, train2ID;
    int transferstation;
    int leavingtime1, arrivingtime1, price1, seat1;
    int leavingtime2, arrivingtime2, price2, seat2;

    Transfer() = default;
};

struct Order {
    int train;
    int fromstation, tostation, fromstation_index, tostation_index;
    int leavingtime, arrivingtime, price, num;
    int state; // 0->success, 1->pending, 2->refunded

    Order() = default;
};

class OrderManager {
  private:
    int order_count;
    std::string order_filename;
    std::fstream order_file;
    BplusTree<sjtu::string<20>, int> userorder_bpt;
    static constexpr int sizeoforder = 10 * sizeofint;


    void writeOrder(int index, int state, int train, int fromstation, int tostation, int fromstation_index, int tostation_index, int leavingtime, int arrivingtime, int price, int num);
  public:
    void writeOrder(int index, Order &order);
    void writeState(int index, int state);

    OrderManager();
    ~OrderManager();
    void changeOrderCount(int count);
    int getOrderCount();

    static bool compareTicketTime(const Ticket &lhs, const Ticket &rhs);
    static bool compareTicketCost(const Ticket &lhs, const Ticket &rhs);
    static bool compareTransferTime(const Transfer &lhs, const Transfer &rhs);
    static bool compareTransferCost(const Transfer &lhs, const Transfer &rhs);

    Order getOrder(int index);
    int getState(int index);
    void addPendingOrder(const sjtu::string<20> &username, int trainID, int fromstation, int tostation, int fromstation_index, int tostation_index, int leavingtime, int arrivingtime, int price, int num);
    void addSuccessOrder(const sjtu::string<20> &username, int trainID, int fromstation, int tostation, int fromstation_index, int tostation_index, int leavingtime, int arrivingtime, int price, int num);
    sjtu::vector<int> getOrderIndexs(const sjtu::string<20> &username);

    void clean();
};