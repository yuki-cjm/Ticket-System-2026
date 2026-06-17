#pragma once

#include <fstream>

#include "tools.hpp"
#include "STLite/BPT.hpp"
#include "STLite/string.hpp"
#include "STLite/vector.hpp"

struct Order {
    int state; // 0->success, 1->pending, 2->refunded
    int leavingtime, arrivingtime, price, num;
    int origin, destination, train_index;
    sjtu::string<20> trainID;
    sjtu::string<30> station1, station2;

    Order() = default;
    ~Order() = default;
};

struct OrderView {
    int price, num;
    int origin, destination, train_index;
};

class OrderManager {
  private:
    int order_count;
    std::string order_filename;
    std::fstream order_file;
    BplusTree<sjtu::string<20>, int> userorder_bpt;
    static constexpr int sizeofOrder = 8 * sizeofint + sizeoftrainID + sizeofstation * 2;
    static constexpr int sizeofOrderView = 5 * sizeofint;

    void writeOrder(int index, int state, sjtu::string<20> &trainID, sjtu::string<30> &station1, sjtu::string<30> &station2, int origin, int destination, int leavingtime, int arrivingtime, int price, int num, int train_index);

  public:
    OrderManager();
    ~OrderManager();
    void changeOrderCount(int count);
    int getOrderCount();

    void addPendingOrder(const sjtu::string<20> &username, sjtu::string<20> &trainID, sjtu::string<30> &station1, sjtu::string<30> &station2, int origin, int destination, int leavingtime, int arrivingtime, int price, int num, int train_index);
    void addSuccessOrder(const sjtu::string<20> &username, sjtu::string<20> &trainID, sjtu::string<30> &station1, sjtu::string<30> &station2, int origin, int destination, int leavingtime, int arrivingtime, int price, int num, int train_index);
    sjtu::vector<int> getOrderIndexs(const sjtu::string<20> &username);
    Order getOrder(int index);
    OrderView getOrderView(int index);
    int getOrderState(int index);
    void writeOrderState(int index, int state);

    void clean();
};