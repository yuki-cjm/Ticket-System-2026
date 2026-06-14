#include <fstream>

#include "OrderManager.hpp"
#include "STLite/BPT.hpp"
#include "STLite/string.hpp"

void OrderManager::writeOrder(int index, int state, int train, int fromstation, int tostation, int fromstation_index, int tostation_index, int leavingtime, int arrivingtime, int price, int num) {
    order_file.seekp(index * sizeoforder);
    order_file.write(reinterpret_cast<char*>(&state), sizeofint);
    order_file.write(reinterpret_cast<char*>(&train), sizeofint);
    order_file.write(reinterpret_cast<char*>(&fromstation), sizeofint);
    order_file.write(reinterpret_cast<char*>(&tostation), sizeofint);
    order_file.write(reinterpret_cast<char*>(&fromstation_index), sizeofint);
    order_file.write(reinterpret_cast<char*>(&tostation_index), sizeofint);
    order_file.write(reinterpret_cast<char*>(&leavingtime), sizeofint);
    order_file.write(reinterpret_cast<char*>(&arrivingtime), sizeofint);
    order_file.write(reinterpret_cast<char*>(&price), sizeofint);
    order_file.write(reinterpret_cast<char*>(&num), sizeofint);
}

void OrderManager::writeOrder(int index, Order &order) {
    writeOrder(index, order.state, order.train, order.fromstation, order.tostation, order.fromstation_index, order.tostation_index, order.leavingtime, order.arrivingtime, order.price, order.num);
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

bool OrderManager::compareTicketTime(const Ticket &lhs, const Ticket &rhs) {
    if (lhs.arrivingtime - lhs.leavingtime < rhs.arrivingtime - rhs.leavingtime) {
        return true;
    }
    if (lhs.arrivingtime - lhs.leavingtime > rhs.arrivingtime - rhs.leavingtime) {
        return false;
    }
    return lhs.trainID < rhs.trainID;
}

bool OrderManager::compareTicketCost(const Ticket &lhs, const Ticket &rhs) {
    if (lhs.price < rhs.price) {
        return true;
    }
    if (lhs.price > rhs.price) {
        return false;
    }
    return lhs.trainID < rhs.trainID;
}

bool OrderManager::compareTransferTime(const Transfer &lhs, const Transfer &rhs) {
    if (lhs.arrivingtime2 - lhs.leavingtime1 < rhs.arrivingtime2 - rhs.leavingtime1) {
        return true;
    }
    if (lhs.arrivingtime2 - lhs.leavingtime1 > rhs.arrivingtime2 - rhs.leavingtime1) {
        return false;
    }
    if (lhs.price1 + lhs.price2 < rhs.price1 + rhs.price2) {
        return true;
    }
    if (lhs.price1 + lhs.price2 > rhs.price1 + rhs.price2) {
        return false;
    }
    if (lhs.train1ID < rhs.train1ID) {
        return true;
    }
    if (lhs.train1ID > rhs.train1ID) {
        return false;
    }
    return lhs.train2ID < rhs.train2ID;
}

bool OrderManager::compareTransferCost(const Transfer &lhs, const Transfer &rhs) {
    if (lhs.price1 + lhs.price2 < rhs.price1 + rhs.price2) {
        return true;
    }
    if (lhs.price1 + lhs.price2 > rhs.price1 + rhs.price2) {
        return false;
    }
    if (lhs.arrivingtime2 - lhs.leavingtime1 < rhs.arrivingtime2 - rhs.leavingtime1) {
        return true;
    }
    if (lhs.arrivingtime2 - lhs.leavingtime1 > rhs.arrivingtime2 - rhs.leavingtime1) {
        return false;
    }
    if (lhs.arrivingtime1 - lhs.leavingtime1 + lhs.arrivingtime2 - lhs.leavingtime2 > rhs.arrivingtime1 - rhs.leavingtime1 + rhs.arrivingtime2 - rhs.leavingtime2) {
        return false;
    }
    if (lhs.train1ID < rhs.train1ID) {
        return true;
    }
    if (lhs.train1ID > rhs.train1ID) {
        return false;
    }
    return lhs.train2ID < rhs.train2ID;
}

Order OrderManager::getOrder(int index) {
    order_file.seekg(index * sizeoforder);
    Order order;
    order_file.read(reinterpret_cast<char*>(&order.state), sizeofint);
    order_file.read(reinterpret_cast<char*>(&order.train), sizeofint);
    order_file.read(reinterpret_cast<char*>(&order.fromstation), sizeofint);
    order_file.read(reinterpret_cast<char*>(&order.tostation), sizeofint);
    order_file.read(reinterpret_cast<char*>(&order.fromstation_index), sizeofint);
    order_file.read(reinterpret_cast<char*>(&order.tostation_index), sizeofint);
    order_file.read(reinterpret_cast<char*>(&order.leavingtime), sizeofint);
    order_file.read(reinterpret_cast<char*>(&order.arrivingtime), sizeofint);
    order_file.read(reinterpret_cast<char*>(&order.price), sizeofint);
    order_file.read(reinterpret_cast<char*>(&order.num), sizeofint);
    return order;
}

void OrderManager::addPendingOrder(const sjtu::string<20> &username, int train, int fromstation, int tostation, int fromstation_index, int tostation_index, int leavingtime, int arrivingtime, int price, int num) {
    writeOrder(order_count, 1, train, fromstation, tostation, fromstation_index, tostation_index, leavingtime, arrivingtime, price, num);
    userorder_bpt.insert(username, order_count);
    order_count++;
}

void OrderManager::addSuccessOrder(const sjtu::string<20> &username, int train, int fromstation, int tostation, int fromstation_index, int tostation_index, int leavingtime, int arrivingtime, int price, int num) {
    writeOrder(order_count, 0, train, fromstation, tostation, fromstation_index, tostation_index, leavingtime, arrivingtime, price, num);
    userorder_bpt.insert(username, order_count);
    order_count++;
}

sjtu::vector<int> OrderManager::getOrderIndexs(const sjtu::string<20> &username) {
    return userorder_bpt.find(username);
}

void OrderManager::clean() {
    order_count = 0;
    order_file.close();
    order_file.open(order_filename, std::ios::out | std::ios::trunc | std::ios::binary);
    order_file.close();
    order_file.open(order_filename, std::ios::in | std::ios::out | std::ios::binary);
    userorder_bpt.clear();
}