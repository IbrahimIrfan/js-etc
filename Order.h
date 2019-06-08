#ifndef ORDER_H
#define ORDER_H
#include <string>

using namespace std;

class Order {
public:
    int id;
    string symbol;
    int price;
    int qty;
    string dir;
    bool acked;

    Order();
    Order(int id, string sym, int price, int qty, string dir);
};

#endif
