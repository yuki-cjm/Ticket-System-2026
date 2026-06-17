#include <fstream>

#include "OrderManager.hpp"
#include "tools.hpp"
#include "STLite/BPT.hpp"
#include "STLite/string.hpp"

void OrderManager::writeOrder(int index, int state, sjtu::string<20> &trainID, sjtu::string<30> &station1, sjtu::string<30> &station2, int origin, int destination, int leavingtime, int arrivingtime, int price, int num, int train_index) {
    order_file.seekp(sizeofOrder * index);
    order_file.write(reinterpret_cast<char*>(&state), sizeofint);
    order_file.write(reinterpret_cast<char*>(&leavingtime), sizeofint);
    order_file.write(reinterpret_cast<char*>(&arrivingtime), sizeofint);
    order_file.write(reinterpret_cast<char*>(&price), sizeofint);
    order_file.write(reinterpret_cast<char*>(&num), sizeofint);
    order_file.write(reinterpret_cast<char*>(&origin), sizeofint);
    order_file.write(reinterpret_cast<char*>(&destination), sizeofint);
    order_file.write(reinterpret_cast<char*>(&train_index), sizeofint);
    order_file.write(reinterpret_cast<char*>(&trainID), sizeoftrainID);
    order_file.write(reinterpret_cast<char*>(&station1), sizeofstation);
    order_file.write(reinterpret_cast<char*>(&station2), sizeofstation);
}




OrderManager::OrderManager() : order_filename("orderdata"), userorder_bpt("orderdatabasic", "orderdataindex") {
    order_file.open(order_filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!order_file) {
        order_file.open(order_filename, std::ios::out | std::ios::binary);
        order_file.close();
        order_file.open(order_filename, std::ios::in | std::ios::out | std::ios::binary);
    }
}

OrderManager::~OrderManager() {
    order_file.close();
}

void OrderManager::changeOrderCount(int count) {
    order_count = count;
}

int OrderManager::getOrderCount() {
    return order_count;
}

void OrderManager::addPendingOrder(const sjtu::string<20> &username, sjtu::string<20> &trainID, sjtu::string<30> &station1, sjtu::string<30> &station2, int origin, int destination, int leavingtime, int arrivingtime, int price, int num, int train_index) {
    writeOrder(order_count, 1, trainID, station1, station2, origin, destination, leavingtime, arrivingtime, price, num, train_index);
    userorder_bpt.insert(username, order_count);
    order_count++;
}

void OrderManager::addSuccessOrder(const sjtu::string<20> &username, sjtu::string<20> &trainID, sjtu::string<30> &station1, sjtu::string<30> &station2, int origin, int destination, int leavingtime, int arrivingtime, int price, int num, int train_index) {
    writeOrder(order_count, 0, trainID, station1, station2, origin, destination, leavingtime, arrivingtime, price, num, train_index);
    userorder_bpt.insert(username, order_count);
    order_count++;
}

sjtu::vector<int> OrderManager::getOrderIndexs(const sjtu::string<20> &username) {
    return userorder_bpt.find(username);
}

Order OrderManager::getOrder(int index) {
    Order order;
    order_file.seekg(sizeofOrder * index);
    order_file.read(reinterpret_cast<char*>(&order), sizeofOrder);
    return order;
}

OrderView OrderManager::getOrderView(int index) {
    OrderView orderview;
    order_file.seekg(sizeofOrder * index + 3 * sizeofint);
    order_file.read(reinterpret_cast<char*>(&orderview), sizeofOrderView);
    return orderview;
}

int OrderManager::getOrderState(int index) {
    int state;
    order_file.seekg(sizeofOrder * index);
    order_file.read(reinterpret_cast<char*>(&state), sizeofint);
    return state;
}

void OrderManager::writeOrderState(int index, int state) {
    order_file.seekp(sizeofOrder * index);
    order_file.write(reinterpret_cast<char*>(&state), sizeofint);
}

void OrderManager::clean() {
    order_count = 0;
    order_file.close();
    order_file.open(order_filename, std::ios::out | std::ios::trunc | std::ios::binary);
    order_file.close();
    order_file.open(order_filename, std::ios::in | std::ios::out | std::ios::binary);
    userorder_bpt.clear();
}